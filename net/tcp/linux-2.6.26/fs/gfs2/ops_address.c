/*
 * Copyright (C) Sistina Software, Inc.  1997-2003 All rights reserved.
 * Copyright (C) 2004-2008 Red Hat, Inc.  All rights reserved.
 *
 * This copyrighted material is made available to anyone wishing to use,
 * modify, copy, or redistribute it subject to the terms and conditions
 * of the GNU General Public License version 2.
 */

#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/completion.h>
#include <linux/buffer_head.h>
#include <linux/pagemap.h>
#include <linux/pagevec.h>
#include <linux/mpage.h>
#include <linux/fs.h>
#include <linux/writeback.h>
#include <linux/swap.h>
#include <linux/gfs2_ondisk.h>
#include <linux/lm_interface.h>
#include <linux/backing-dev.h>

#include "gfs2.h"
#include "incore.h"
#include "bmap.h"
#include "glock.h"
#include "inode.h"
#include "log.h"
#include "meta_io.h"
#include "ops_address.h"
#include "quota.h"
#include "trans.h"
#include "rgrp.h"
#include "super.h"
#include "util.h"
#include "glops.h"


static void gfs2_page_add_databufs(struct gfs2_inode *ip, struct page *page,
                   unsigned int from, unsigned int to)
{
    struct buffer_head *head = page_buffers(page);
    unsigned int bsize = head->b_size;
    struct buffer_head *bh;
    unsigned int start, end;

    for (bh = head, start = 0; bh != head || !start;
         bh = bh->b_this_page, start = end) {
        end = start + bsize;
        if (end <= from || start >= to)
            continue;
        if (gfs2_is_jdata(ip))
            set_buffer_uptodate(bh);
        gfs2_trans_add_bh(ip->i_gl, bh, 0);
    }
}

/**
 * gfs2_get_block_noalloc - Fills in a buffer head with details about a block
 * @inode: The inode
 * @lblock: The block number to look up
 * @bh_result: The buffer head to return the result in
 * @create: Non-zero if we may add block to the file
 *
 * Returns: errno
 */

static int gfs2_get_block_noalloc(struct inode *inode, sector_t lblock,
                  struct buffer_head *bh_result, int create)
{
    int error;

    error = gfs2_block_map(inode, lblock, bh_result, 0);
    if (error)
        return error;
    if (!buffer_mapped(bh_result))
        return -EIO;
    return 0;
}

static int gfs2_get_block_direct(struct inode *inode, sector_t lblock,
                 struct buffer_head *bh_result, int create)
{
    return gfs2_block_map(inode, lblock, bh_result, 0);
}

/**
 * gfs2_writepage_common - Common bits of writepage
 * @page: The page to be written
 * @wbc: The writeback control
 *
 * Returns: 1 if writepage is ok, otherwise an error code or zero if no error.
 */

static int gfs2_writepage_common(struct page *page,
                 struct writeback_control *wbc)
{
    struct inode *inode = page->mapping->host;
    struct gfs2_inode *ip = GFS2_I(inode);
    struct gfs2_sbd *sdp = GFS2_SB(inode);
    loff_t i_size = i_size_read(inode);
    pgoff_t end_index = i_size >> PAGE_CACHE_SHIFT;
    unsigned offset;

    if (gfs2_assert_withdraw(sdp, gfs2_glock_is_held_excl(ip->i_gl)))
        goto out;
    if (current->journal_info)
        goto redirty;
    /* Is the page fully outside i_size? (truncate in progress) */
    offset = i_size & (PAGE_CACHE_SIZE-1);
    if (page->index > end_index || (page->index == end_index && !offset)) {
        page->mapping->a_ops->invalidatepage(page, 0);
        goto out;
    }
    return 1;
redirty:
    redirty_page_for_writepage(wbc, page);
out:
    unlock_page(page);
    return 0;
}

/**
 * gfs2_writeback_writepage - Write page for writeback mappings
 * @page: The page
 * @wbc: The writeback control
 *
 */

static int gfs2_writeback_writepage(struct page *page,
                    struct writeback_control *wbc)
{
    int ret;

    ret = gfs2_writepage_common(page, wbc);
    if (ret <= 0)
        return ret;

    ret = mpage_writepage(page, gfs2_get_block_noalloc, wbc);
    if (ret == -EAGAIN)
        ret = block_write_full_page(page, gfs2_get_block_noalloc, wbc);
    return ret;
}

/**
 * gfs2_ordered_writepage - Write page for ordered data files
 * @page: The page to write
 * @wbc: The writeback control
 *
 */

static int gfs2_ordered_writepage(struct page *page,
                  struct writeback_control *wbc)
{
    struct inode *inode = page->mapping->host;
    struct gfs2_inode *ip = GFS2_I(inode);
    int ret;

    ret = gfs2_writepage_common(page, wbc);
    if (ret <= 0)
        return ret;

    if (!page_has_buffers(page)) {
        create_empty_buffers(page, inode->i_sb->s_blocksize,
                     (1 << BH_Dirty)|(1 << BH_Uptodate));
    }
    gfs2_page_add_databufs(ip, page, 0, inode->i_sb->s_blocksize-1);
    return block_write_full_page(page, gfs2_get_block_noalloc, wbc);
}

/**
 * __gfs2_jdata_writepage - The core of jdata writepage
 * @page: The page to write
 * @wbc: The writeback control
 *
 * This is shared between writepage and writepages and implements the
 * core of the writepage operation. If a transaction is required then
 * PageChecked will have been set and the transaction will have
 * already been started before this is called.
 */

static int __gfs2_jdata_writepage(struct page *page, struct writeback_control *wbc)
{
    struct inode *inode = page->mapping->host;
    struct gfs2_inode *ip = GFS2_I(inode);
    struct gfs2_sbd *sdp = GFS2_SB(inode);

    if (PageChecked(page)) {
        ClearPageChecked(page);
        if (!page_has_buffers(page)) {
            create_empty_buffers(page, inode->i_sb->s_blocksize,
                         (1 << BH_Dirty)|(1 << BH_Uptodate));
        }
        gfs2_page_add_databufs(ip, page, 0, sdp->sd_vfs->s_blocksize-1);
    }
    return block_write_full_page(page, gfs2_get_block_noalloc, wbc);
}

/**
 * gfs2_jdata_writepage - Write complete page
 * @page: Page to write
 *
 * Returns: errno
 *
 */

static int gfs2_jdata_writepage(struct page *page, struct writeback_control *wbc)
{
    struct inode *inode = page->mapping->host;
    struct gfs2_sbd *sdp = GFS2_SB(inode);
    int error;
    int done_trans = 0;

    error = gfs2_writepage_common(page, wbc);
    if (error <= 0)
        return error;

    if (PageChecked(page)) {
        if (wbc->sync_mode != WB_SYNC_ALL)
            goto out_ignore;
        error = gfs2_trans_begin(sdp, RES_DINODE + 1, 0);
        if (error)
            goto out_ignore;
        done_trans = 1;
    }
    error = __gfs2_jdata_writepage(page, wbc);
    if (done_trans)
        gfs2_trans_end(sdp);
    return error;

out_ignore:
    redirty_page_for_writepage(wbc, page);
    unlock_page(page);
    return 0;
}

/**
 * gfs2_writeback_writepages - Write a bunch of dirty pages back to disk
 * @mapping: The mapping to write
 * @wbc: Write-back control
 *
 * For the data=writeback case we can already ignore buffer heads
 * and write whole extents at once. This is a big reduction in the
 * number of I/O requests we send and the bmap calls we make in this case.
 */
static int gfs2_writeback_writepages(struct address_space *mapping,
                     struct writeback_control *wbc)
{
    return mpage_writepages(mapping, wbc, gfs2_get_block_noalloc);
}

/**
 * gfs2_write_jdata_pagevec - Write back a pagevec's worth of pages
 * @mapping: The mapping
 * @wbc: The writeback control
 * @writepage: The writepage function to call for each page
 * @pvec: The vector of pages
 * @nr_pages: The number of pages to write
 *
 * Returns: non-zero if loop should terminate, zero otherwise
 */

static int gfs2_write_jdata_pagevec(struct address_space *mapping,
                    struct writeback_control *wbc,
                    struct pagevec *pvec,
                    int nr_pages, pgoff_t end)
{
    struct inode *inode = mapping->host;
    struct gfs2_sbd *sdp = GFS2_SB(inode);
    loff_t i_size = i_size_read(inode);
    pgoff_t end_index = i_size >> PAGE_CACHE_SHIFT;
    unsigned offset = i_size & (PAGE_CACHE_SIZE-1);
    unsigned nrblocks = nr_pages * (PAGE_CACHE_SIZE/inode->i_sb->s_blocksize);
    struct backing_dev_info *bdi = mapping->backing_dev_info;
    int i;
    int ret;

    ret = gfs2_trans_begin(sdp, nrblocks, nrblocks);
    if (ret < 0)
        return ret;

    for(i = 0; i < nr_pages; i++) {
        struct page *page = pvec->pages[i];

        lock_page(page);

        if (unlikely(page->mapping != mapping)) {
            unlock_page(page);
            continue;
        }

        if (!wbc->range_cyclic && page->index > end) {
            ret = 1;
            unlock_page(page);
            continue;
        }

        if (wbc->sync_mode != WB_SYNC_NONE)
            wait_on_page_writeback(page);

        if (PageWriteback(page) ||
            !clear_page_dirty_for_io(page)) {
            unlock_page(page);
            continue;
        }

        /* Is the page fully outside i_size? (truncate in progress) */
        if (page->index > end_index || (page->index == end_index && !offset)) {
            page->mapping->a_ops->invalidatepage(page, 0);
            unlock_page(page);
            continue;
        }

        ret = __gfs2_jdata_writepage(page, wbc);

        if (ret || (--(wbc->nr_to_write) <= 0))
            ret = 1;
        if (wbc->nonblocking && bdi_write_congested(bdi)) {
            wbc->encountered_congestion = 1;
            ret = 1;
        }

    }
    gfs2_trans_end(sdp);
    return ret;
}

/**
 * gfs2_write_cache_jdata - Like write_cache_pages but different
 * @mapping: The mapping to write
 * @wbc: The writeback control
 * @writepage: The writepage function to call
 * @data: The data to pass to writepage
 *
 * The reason that we use our own function here is that we need to
 * start transactions before we grab page locks. This allows us
 * to get the ordering right.
 */

static int gfs2_write_cache_jdata(struct address_space *mapping,
                  struct writeback_control *wbc)
{
    struct backing_dev_info *bdi = mapping->backing_dev_info;
    int ret = 0;
    int done = 0;
    struct pagevec pvec;
    int nr_pages;
    pgoff_t index;
    pgoff_t end;
    int scanned = 0;
    int range_whole = 0;

    if (wbc->nonblocking && bdi_write_congested(bdi)) {
        wbc->encountered_congestion = 1;
        return 0;
    }

    pagevec_init(&pvec, 0);
    if (wbc->range_cyclic) {
        index = mapping->writeback_index; /* Start from prev offset */
        end = -1;
    } else {
        index = wbc->range_start >> PAGE_CACHE_SHIFT;
        end = wbc->range_end >> PAGE_CACHE_SHIFT;
        if (wbc->range_start == 0 && wbc->range_end == LLONG_MAX)
            range_whole = 1;
        scanned = 1;
    }

retry:
     while (!done && (index <= end) &&
        (nr_pages = pagevec_lookup_tag(&pvec, mapping, &index,
                           PAGECACHE_TAG_DIRTY,
                           min(end - index, (pgoff_t)PAGEVEC_SIZE-1) + 1))) {
        scanned = 1;
        ret = gfs2_write_jdata_pagevec(mapping, wbc, &pvec, nr_pages, end);
        if (ret)
            done = 1;
        if (ret > 0)
            ret = 0;

        pagevec_release(&pvec);
        cond_resched();
    }

    if (!scanned && !done) {
        /*
         * We hit the last page and there is more work to be done: wrap
         * back to the start of the file
         */
        scanned = 1;
        index = 0;
        goto retry;
    }

    if (wbc->range_cyclic || (range_whole && wbc->nr_to_write > 0))
        mapping->writeback_index = index;
    return ret;
}


/**
 * gfs2_jdata_writepages - Write a bunch of dirty pages back to disk
 * @mapping: The mapping to write
 * @wbc: The writeback control
 * 
 */

static int gfs2_jdata_writepages(struct address_space *mapping,
                 struct writeback_control *wbc)
{
    struct gfs2_inode *ip = GFS2_I(mapping->host);
    struct gfs2_sbd *sdp = GFS2_SB(mapping->host);
    int ret;

    ret = gfs2_write_cache_jdata(mapping, wbc);
    if (ret == 0 && wbc->sync_mode == WB_SYNC_ALL) {
        gfs2_log_flush(sdp, ip->i_gl);
        ret = gfs2_write_cache_jdata(mapping, wbc);
    }
    return ret;
}

/**
 * stuffed_readpage - Fill in a Linux page with stuffed file data
 * @ip: the inode
 * @page: the page
 *
 * Returns: errno
 */

static int stuffed_readpage(struct gfs2_inode *ip, struct page *page)
{
    struct buffer_head *dibh;
    void *kaddr;
    int error;

    /*
     * Due to the order of unstuffing files and ->fault(), we can be
     * asked for a zero page in the case of a stuffed file being extended,
     * so we need to supply one here. It doesn't happen often.
     */
    if (unlikely(page->index)) {
        zero_user(page, 0, PAGE_CACHE_SIZE);
        return 0;
    }

    error = gfs2_meta_inode_buffer(ip, &dibh);
    if (error)
        return error;

    kaddr = kmap_atomic(page, KM_USER0);
    memcpy(kaddr, dibh->b_data + sizeof(struct gfs2_dinode),
           ip->i_di.di_size);
    memset(kaddr + ip->i_di.di_size, 0, PAGE_CACHE_SIZE - ip->i_di.di_size);
    kunmap_atomic(kaddr, KM_USER0);
    flush_dcache_page(page);
    brelse(dibh);
    SetPageUptodate(page);

    return 0;
}


/**
 * __gfs2_readpage - readpage
 * @file: The file to read a page for
 * @page: The page to read
 *
 * This is the core of gfs2's readpage. Its used by the internal file
 * reading code as in that case we already hold the glock. Also its
 * called by gfs2_readpage() once the required lock has been granted.
 *
 */

static int __gfs2_readpage(void *file, struct page *page)
{
    struct gfs2_inode *ip = GFS2_I(page->mapping->host);
    struct gfs2_sbd *sdp = GFS2_SB(page->mapping->host);
    int error;

    if (gfs2_is_stuffed(ip)) {
        error = stuffed_readpage(ip, page);
        unlock_page(page);
    } else {
        error = mpage_readpage(page, gfs2_block_map);
    }

    if (unlikely(test_bit(SDF_SHUTDOWN, &sdp->sd_flags)))
        return -EIO;

    return error;
}

/**
 * gfs2_readpage - read a page of a file
 * @file: The file to read
 * @page: The page of the file
 *
 * This deals with the locking required. We use a trylock in order to
 * avoid the page lock / glock ordering problems returning AOP_TRUNCATED_PAGE
 * in the event that we are unable to get the lock.
 */

static int gfs2_readpage(struct file *file, struct page *page)
{
    struct gfs2_inode *ip = GFS2_I(page->mapping->host);
    struct gfs2_holder *gh;
    int error;

    gh = gfs2_glock_is_locked_by_me(ip->i_gl);
    if (!gh) {
        gh = kmalloc(sizeof(struct gfs2_holder), GFP_NOFS);
        if (!gh)
            return -ENOBUFS;
        gfs2_holder_init(ip->i_gl, LM_ST_SHARED, GL_ATIME, gh);
        unlock_page(page);
        error = gfs2_glock_nq_atime(gh);
        if (likely(error != 0))
            goto out;
        return AOP_TRUNCATED_PAGE;
    }
    error = __gfs2_readpage(file, page);
    gfs2_glock_dq(gh);
out:
    gfs2_holder_uninit(gh);
    kfree(gh);
    return error;
}

/**
 * gfs2_internal_read - read an internal file
 * @ip: The gfs2 inode
 * @ra_state: The readahead state (or NULL for no readahead)
 * @buf: The buffer to fill
 * @pos: The file position
 * @size: The amount to read
 *
 */

int gfs2_internal_read(struct gfs2_inode *ip, struct file_ra_state *ra_state,
                       char *buf, loff_t *pos, unsigned size)
{
    struct address_space *mapping = ip->i_inode.i_mapping;
    unsigned long index = *pos / PAGE_CACHE_SIZE;
    unsigned offset = *pos & (PAGE_CACHE_SIZE - 1);
    unsigned copied = 0;
    unsigned amt;
    struct page *page;
    void *p;

    do {
        amt = size - copied;
        if (offset + size > PAGE_CACHE_SIZE)
            amt = PAGE_CACHE_SIZE - offset;
        page = read_cache_page(mapping, index, __gfs2_readpage, NULL);
        if (IS_ERR(page))
            return PTR_ERR(page);
        p = kmap_atomic(page, KM_USER0);
        memcpy(buf + copied, p + offset, amt);
        kunmap_atomic(p, KM_USER0);
        mark_page_accessed(page);
        page_cache_release(page);
        copied += amt;
        index++;
        offset = 0;
    } while(copied < size);
    (*pos) += size;
    return size;
}

/**
 * gfs2_readpages - Read a bunch of pages at once
 *
 * Some notes:
 * 1. This is only for readahead, so we can simply ignore any things
 *    which are slightly inconvenient (such as locking conflicts between
 *    the page lock and the glock) and return having done no I/O. Its
 *    obviously not something we'd want to do on too regular a basis.
 *    Any I/O we ignore at this time will be done via readpage later.
 * 2. We don't handle stuffed files here we let readpage do the honours.
 * 3. mpage_readpages() does most of the heavy lifting in the common case.
 * 4. gfs2_block_map() is relied upon to set BH_Boundary in the right places.
 */

static int gfs2_readpages(struct file *file, struct address_space *mapping,
              struct list_head *pages, unsigned nr_pages)
{
    struct inode *inode = mapping->host;
    struct gfs2_inode *ip = GFS2_I(inode);
    struct gfs2_sbd *sdp = GFS2_SB(inode);
    struct gfs2_holder gh;
    int ret;

    gfs2_holder_init(ip->i_gl, LM_ST_SHARED, GL_ATIME, &gh);
    ret = gfs2_glock_nq_atime(&gh);
    if (unlikely(ret))
        goto out_uninit;
    if (!gfs2_is_stuffed(ip))
        ret = mpage_readpages(mapping, pages, nr_pages, gfs2_block_map);
    gfs2_glock_dq(&gh);
out_uninit:
    gfs2_holder_uninit(&gh);
    if (unlikely(test_bit(SDF_SHUTDOWN, &sdp->sd_flags)))
        ret = -EIO;
    return ret;
}

/**
 * gfs2_write_begin - Begin to write to a file
 * @file: The file to write to
 * @mapping: The mapping in which to write
 * @pos: The file offset at which to start writing
 * @len: Length of the write
 * @flags: Various flags
 * @pagep: Pointer to return the page
 * @fsdata: Pointer to return fs data (unused by GFS2)
 *
 * Returns: errno
 */

static int gfs2_write_begin(struct file *file, struct address_space *mapping,
                loff_t pos, unsigned len, unsigned flags,
                struct page **pagep, void **fsdata)
{
    struct gfs2_inode *ip = GFS2_I(mapping->host);
    struct gfs2_sbd *sdp = GFS2_SB(mapping->host);
    unsigned int data_blocks, ind_blocks, rblocks;
    int alloc_required;
    int error = 0;
    struct gfs2_alloc *al;
    pgoff_t index = pos >> PAGE_CACHE_SHIFT;
    unsigned from = pos & (PAGE_CACHE_SIZE - 1);
    unsigned to = from + len;
    struct page *page;

    gfs2_holder_init(ip->i_gl, LM_ST_EXCLUSIVE, GL_ATIME, &ip->i_gh);
    error = gfs2_glock_nq_atime(&ip->i_gh);
    if (unlikely(error))
        goto out_uninit;

    gfs2_write_calc_reserv(ip, len, &data_blocks, &ind_blocks);
    error = gfs2_write_alloc_required(ip, pos, len, &alloc_required);
    if (error)
        goto out_unlock;

    if (alloc_required) {
        al = gfs2_alloc_get(ip);
        if (!al) {
            error = -ENOMEM;
            goto out_unlock;
        }

        error = gfs2_quota_lock_check(ip);
        if (error)
            goto out_alloc_put;

        al->al_requested = data_blocks + ind_blocks;
        error = gfs2_inplace_reserve(ip);
        if (error)
            goto out_qunlock;
    }

    rblocks = RES_DINODE + ind_blocks;
    if (gfs2_is_jdata(ip))
        rblocks += data_blocks ? data_blocks : 1;
    if (ind_blocks || data_blocks)
        rblocks += RES_STATFS + RES_QUOTA;

    error = gfs2_trans_begin(sdp, rblocks,
                 PAGE_CACHE_SIZE/sdp->sd_sb.sb_bsize);
    if (error)
        goto out_trans_fail;

    error = -ENOMEM;
    page = __grab_cache_page(mapping, index);
    *pagep = page;
    if (unlikely(!page))
        goto out_endtrans;

    if (gfs2_is_stuffed(ip)) {
        error = 0;
        if (pos + len > sdp->sd_sb.sb_bsize - sizeof(struct gfs2_dinode)) {
            error = gfs2_unstuff_dinode(ip, page);
            if (error == 0)
                goto prepare_write;
        } else if (!PageUptodate(page)) {
            error = stuffed_readpage(ip, page);
        }
        goto out;
    }

prepare_write:
    error = block_prepare_write(page, from, to, gfs2_block_map);
out:
    if (error == 0)
        return 0;

    page_cache_release(page);
    if (pos + len > ip->i_inode.i_size)
        vmtruncate(&ip->i_inode, ip->i_inode.i_size);
out_endtrans:
    gfs2_trans_end(sdp);
out_trans_fail:
    if (alloc_required) {
        gfs2_inplace_release(ip);
out_qunlock:
        gfs2_quota_unlock(ip);
out_alloc_put:
        gfs2_alloc_put(ip);
    }
out_unlock:
    gfs2_glock_dq(&ip->i_gh);
out_uninit:
    gfs2_holder_uninit(&ip->i_gh);
    return error;
}

/**
 * adjust_fs_space - Adjusts the free space available due to gfs2_grow
 * @inode: the rindex inode
 */
static void adjust_fs_space(struct inode *inode)
{
    struct gfs2_sbd *sdp = inode->i_sb->s_fs_info;
    struct gfs2_statfs_change_host *m_sc = &sdp->sd_statfs_master;
    struct gfs2_statfs_change_host *l_sc = &sdp->sd_statfs_local;
    u64 fs_total, new_free;

    /* Total up the file system space, according to the latest rindex. */
    fs_total = gfs2_ri_total(sdp);

    spin_lock(&sdp->sd_statfs_spin);
    if (fs_total > (m_sc->sc_total + l_sc->sc_total))
        new_free = fs_total - (m_sc->sc_total + l_sc->sc_total);
    else
        new_free = 0;
    spin_unlock(&sdp->sd_statfs_spin);
    fs_warn(sdp, "File system extended by %llu blocks.\n",
        (unsigned long long)new_free);
    gfs2_statfs_change(sdp, new_free, new_free, 0);
}

/**
 * gfs2_stuffed_write_end - Write end for stuffed files
 * @inode: The inode
 * @dibh: The buffer_head containing the on-disk inode
 * @pos: The file position
 * @len: The length of the write
 * @copied: How much was actually copied by the VFS
 * @page: The page
 *
 * This copies the data from the page into the inode block after
 * the inode data structure itself.
 *
 * Returns: errno
 */
static int gfs2_stuffed_write_end(struct inode *inode, struct buffer_head *dibh,
                  loff_t pos, unsigned len, unsigned copied,
                  struct page *page)
{
    struct gfs2_inode *ip = GFS2_I(inode);
    struct gfs2_sbd *sdp = GFS2_SB(inode);
    u64 to = pos + copied;
    void *kaddr;
    unsigned char *buf = dibh->b_data + sizeof(struct gfs2_dinode);
    struct gfs2_dinode *di = (struct gfs2_dinode *)dibh->b_data;

    BUG_ON((pos + len) > (dibh->b_size - sizeof(struct gfs2_dinode)));
    kaddr = kmap_atomic(page, KM_USER0);
    memcpy(buf + pos, kaddr + pos, copied);
    memset(kaddr + pos + copied, 0, len - copied);
    flush_dcache_page(page);
    kunmap_atomic(kaddr, KM_USER0);

    if (!PageUptodate(page))
        SetPageUptodate(page);
    unlock_page(page);
    page_cache_release(page);

    if (inode->i_size < to) {
        i_size_write(inode, to);
        ip->i_di.di_size = inode->i_size;
        di->di_size = cpu_to_be64(inode->i_size);
        mark_inode_dirty(inode);
    }

    if (inode == sdp->sd_rindex)
        adjust_fs_space(inode);

    brelse(dibh);
    gfs2_trans_end(sdp);
    gfs2_glock_dq(&ip->i_gh);
    gfs2_holder_uninit(&ip->i_gh);
    return copied;
}

/**
 * gfs2_write_end
 * @file: The file to write to
 * @mapping: The address space to write to
 * @pos: The file position
 * @len: The length of the data
 * @copied:
 * @page: The page that has been written
 * @fsdata: The fsdata (unused in GFS2)
 *
 * The main write_end function for GFS2. We have a separate one for
 * stuffed files as they are slightly different, otherwise we just
 * put our locking around the VFS provided functions.
 *
 * Returns: errno
 */

static int gfs2_write_end(struct file *file, struct address_space *mapping,
              loff_t pos, unsigned len, unsigned copied,
              struct page *page, void *fsdata)
{
    struct inode *inode = page->mapping->host;
    struct gfs2_inode *ip = GFS2_I(inode);
    struct gfs2_sbd *sdp = GFS2_SB(inode);
    struct buffer_head *dibh;
    struct gfs2_alloc *al = ip->i_alloc;
    struct gfs2_dinode *di;
    unsigned int from = pos & (PAGE_CACHE_SIZE - 1);
    unsigned int to = from + len;
    int ret;

    BUG_ON(gfs2_glock_is_locked_by_me(ip->i_gl) == NULL);

    ret = gfs2_meta_inode_buffer(ip, &dibh);
    if (unlikely(ret)) {
        unlock_page(page);
        page_cache_release(page);
        goto failed;
    }

    gfs2_trans_add_bh(ip->i_gl, dibh, 1);

    if (gfs2_is_stuffed(ip))
        return gfs2_stuffed_write_end(inode, dibh, pos, len, copied, page);

    if (!gfs2_is_writeback(ip))
        gfs2_page_add_databufs(ip, page, from, to);

    ret = generic_write_end(file, mapping, pos, len, copied, page, fsdata);

    if (likely(ret >= 0) && (inode->i_size > ip->i_di.di_size)) {
        di = (struct gfs2_dinode *)dibh->b_data;
        ip->i_di.di_size = inode->i_size;
        di->di_size = cpu_to_be64(inode->i_size);
        mark_inode_dirty(inode);
    }

    if (inode == sdp->sd_rindex)
        adjust_fs_space(inode);

    brelse(dibh);
    gfs2_trans_end(sdp);
failed:
    if (al) {
        gfs2_inplace_release(ip);
        gfs2_quota_unlock(ip);
        gfs2_alloc_put(ip);
    }
    gfs2_glock_dq(&ip->i_gh);
    gfs2_holder_uninit(&ip->i_gh);
    return ret;
}

/**
 * gfs2_set_page_dirty - Page dirtying function
 * @page: The page to dirty
 *
 * Returns: 1 if it dirtyed the page, or 0 otherwise
 */
 
static int gfs2_set_page_dirty(struct page *page)
{
    SetPageChecked(page);
    return __set_page_dirty_buffers(page);
}

/**
 * gfs2_bmap - Block map function
 * @mapping: Address space info
 * @lblock: The block to map
 *
 * Returns: The disk address for the block or 0 on hole or error
 */

static sector_t gfs2_bmap(struct address_space *mapping, sector_t lblock)
{
    struct gfs2_inode *ip = GFS2_I(mapping->host);
    struct gfs2_holder i_gh;
    sector_t dblock = 0;
    int error;

    error = gfs2_glock_nq_init(ip->i_gl, LM_ST_SHARED, LM_FLAG_ANY, &i_gh);
    if (error)
        return 0;

    if (!gfs2_is_stuffed(ip))
        dblock = generic_block_bmap(mapping, lblock, gfs2_block_map);

    gfs2_glock_dq_uninit(&i_gh);

    return dblock;
}

static void gfs2_discard(struct gfs2_sbd *sdp, struct buffer_head *bh)
{
    struct gfs2_bufdata *bd;

    lock_buffer(bh);
    gfs2_log_lock(sdp);
    clear_buffer_dirty(bh);
    bd = bh->b_private;
    if (bd) {
        if (!list_empty(&bd->bd_le.le_list) && !buffer_pinned(bh))
            list_del_init(&bd->bd_le.le_list);
        else
            gfs2_remove_from_journal(bh, current->journal_info, 0);
    }
    bh->b_bdev = NULL;
    clear_buffer_mapped(bh);
    clear_buffer_req(bh);
    clear_buffer_new(bh);
    gfs2_log_unlock(sdp);
    unlock_buffer(bh);
}

static void gfs2_invalidatepage(struct page *page, unsigned long offset)
{
    struct gfs2_sbd *sdp = GFS2_SB(page->mapping->host);
    struct buffer_head *bh, *head;
    unsigned long pos = 0;

    BUG_ON(!PageLocked(page));
    if (offset == 0)
        ClearPageChecked(page);
    if (!page_has_buffers(page))
        goto out;

    bh = head = page_buffers(page);
    do {
        if (offset <= pos)
            gfs2_discard(sdp, bh);
        pos += bh->b_size;
        bh = bh->b_this_page;
    } while (bh != head);
out:
    if (offset == 0)
        try_to_release_page(page, 0);
}

/**
 * gfs2_ok_for_dio - check that dio is valid on this file
 * @ip: The inode
 * @rw: READ or WRITE
 * @offset: The offset at which we are reading or writing
 *
 * Returns: 0 (to ignore the i/o request and thus fall back to buffered i/o)
 *          1 (to accept the i/o request)
 */
static int gfs2_ok_for_dio(struct gfs2_inode *ip, int rw, loff_t offset)
{
    /*
     * Should we return an error here? I can't see that O_DIRECT for
     * a stuffed file makes any sense. For now we'll silently fall
     * back to buffered I/O
     */
    if (gfs2_is_stuffed(ip))
        return 0;

    if (offset > i_size_read(&ip->i_inode))
        return 0;
    return 1;
}



static ssize_t gfs2_direct_IO(int rw, struct kiocb *iocb,
                  const struct iovec *iov, loff_t offset,
                  unsigned long nr_segs)
{
    struct file *file = iocb->ki_filp;
    struct inode *inode = file->f_mapping->host;
    struct gfs2_inode *ip = GFS2_I(inode);
    struct gfs2_holder gh;
    int rv;

    /*
     * Deferred lock, even if its a write, since we do no allocation
     * on this path. All we need change is atime, and this lock mode
     * ensures that other nodes have flushed their buffered read caches
     * (i.e. their page cache entries for this inode). We do not,
     * unfortunately have the option of only flushing a range like
     * the VFS does.
     */
    gfs2_holder_init(ip->i_gl, LM_ST_DEFERRED, GL_ATIME, &gh);
    rv = gfs2_glock_nq_atime(&gh);
    if (rv)
        return rv;
    rv = gfs2_ok_for_dio(ip, rw, offset);
    if (rv != 1)
        goto out; /* dio not valid, fall back to buffered i/o */

    rv = blockdev_direct_IO_no_locking(rw, iocb, inode, inode->i_sb->s_bdev,
                       iov, offset, nr_segs,
                       gfs2_get_block_direct, NULL);
out:
    gfs2_glock_dq_m(1, &gh);
    gfs2_holder_uninit(&gh);
    return rv;
}

/**
 * gfs2_releasepage - free the metadata associated with a page
 * @page: the page that's being released
 * @gfp_mask: passed from Linux VFS, ignored by us
 *
 * Call try_to_free_buffers() if the buffers in this page can be
 * released.
 *
 * Returns: 0
 */

int gfs2_releasepage(struct page *page, gfp_t gfp_mask)
{
    struct inode *aspace = page->mapping->host;
    struct gfs2_sbd *sdp = aspace->i_sb->s_fs_info;
    struct buffer_head *bh, *head;
    struct gfs2_bufdata *bd;

    if (!page_has_buffers(page))
        return 0;

    gfs2_log_lock(sdp);
    head = bh = page_buffers(page);
    do {
        if (atomic_read(&bh->b_count))
            goto cannot_release;
        bd = bh->b_private;
        if (bd && bd->bd_ail)
            goto cannot_release;
        gfs2_assert_warn(sdp, !buffer_pinned(bh));
        gfs2_assert_warn(sdp, !buffer_dirty(bh));
        bh = bh->b_this_page;
    } while(bh != head);
    gfs2_log_unlock(sdp);

    head = bh = page_buffers(page);
    do {
        gfs2_log_lock(sdp);
        bd = bh->b_private;
        if (bd) {
            gfs2_assert_warn(sdp, bd->bd_bh == bh);
            gfs2_assert_warn(sdp, list_empty(&bd->bd_list_tr));
            if (!list_empty(&bd->bd_le.le_list)) {
                if (!buffer_pinned(bh))
                    list_del_init(&bd->bd_le.le_list);
                else
                    bd = NULL;
            }
            if (bd)
                bd->bd_bh = NULL;
            bh->b_private = NULL;
        }
        gfs2_log_unlock(sdp);
        if (bd)
            kmem_cache_free(gfs2_bufdata_cachep, bd);

        bh = bh->b_this_page;
    } while (bh != head);

    return try_to_free_buffers(page);
cannot_release:
    gfs2_log_unlock(sdp);
    return 0;
}

static const struct address_space_operations gfs2_writeback_aops = {
    .writepage = gfs2_writeback_writepage,
    .writepages = gfs2_writeback_writepages,
    .readpage = gfs2_readpage,
    .readpages = gfs2_readpages,
    .sync_page = block_sync_page,
    .write_begin = gfs2_write_begin,
    .write_end = gfs2_write_end,
    .bmap = gfs2_bmap,
    .invalidatepage = gfs2_invalidatepage,
    .releasepage = gfs2_releasepage,
    .direct_IO = gfs2_direct_IO,
    .migratepage = buffer_migrate_page,
};

static const struct address_space_operations gfs2_ordered_aops = {
    .writepage = gfs2_ordered_writepage,
    .readpage = gfs2_readpage,
    .readpages = gfs2_readpages,
    .sync_page = block_sync_page,
    .write_begin = gfs2_write_begin,
    .write_end = gfs2_write_end,
    .set_page_dirty = gfs2_set_page_dirty,
    .bmap = gfs2_bmap,
    .invalidatepage = gfs2_invalidatepage,
    .releasepage = gfs2_releasepage,
    .direct_IO = gfs2_direct_IO,
    .migratepage = buffer_migrate_page,
};

static const struct address_space_operations gfs2_jdata_aops = {
    .writepage = gfs2_jdata_writepage,
    .writepages = gfs2_jdata_writepages,
    .readpage = gfs2_readpage,
    .readpages = gfs2_readpages,
    .sync_page = block_sync_page,
    .write_begin = gfs2_write_begin,
    .write_end = gfs2_write_end,
    .set_page_dirty = gfs2_set_page_dirty,
    .bmap = gfs2_bmap,
    .invalidatepage = gfs2_invalidatepage,
    .releasepage = gfs2_releasepage,
};

void gfs2_set_aops(struct inode *inode)
{
    struct gfs2_inode *ip = GFS2_I(inode);

    if (gfs2_is_writeback(ip))
        inode->i_mapping->a_ops = &gfs2_writeback_aops;
    else if (gfs2_is_ordered(ip))
        inode->i_mapping->a_ops = &gfs2_ordered_aops;
    else if (gfs2_is_jdata(ip))
        inode->i_mapping->a_ops = &gfs2_jdata_aops;
    else
        BUG();
}

