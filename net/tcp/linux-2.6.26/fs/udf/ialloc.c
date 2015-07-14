/*
 * ialloc.c
 *
 * PURPOSE
 *    Inode allocation handling routines for the OSTA-UDF(tm) filesystem.
 *
 * COPYRIGHT
 *    This file is distributed under the terms of the GNU General Public
 *    License (GPL). Copies of the GPL can be obtained from:
 *        ftp://prep.ai.mit.edu/pub/gnu/GPL
 *    Each contributing author retains all rights to their own work.
 *
 *  (C) 1998-2001 Ben Fennema
 *
 * HISTORY
 *
 *  02/24/99 blf  Created.
 *
 */

#include "udfdecl.h"
#include <linux/fs.h>
#include <linux/quotaops.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include "udf_i.h"
#include "udf_sb.h"

void udf_free_inode(struct inode *inode)
{
    struct super_block *sb = inode->i_sb;
    struct udf_sb_info *sbi = UDF_SB(sb);

    /*
     * Note: we must free any quota before locking the superblock,
     * as writing the quota to disk may need the lock as well.
     */
    DQUOT_FREE_INODE(inode);
    DQUOT_DROP(inode);

    clear_inode(inode);

    mutex_lock(&sbi->s_alloc_mutex);
    if (sbi->s_lvid_bh) {
        struct logicalVolIntegrityDescImpUse *lvidiu =
                            udf_sb_lvidiu(sbi);
        if (S_ISDIR(inode->i_mode))
            le32_add_cpu(&lvidiu->numDirs, -1);
        else
            le32_add_cpu(&lvidiu->numFiles, -1);

        mark_buffer_dirty(sbi->s_lvid_bh);
    }
    mutex_unlock(&sbi->s_alloc_mutex);

    udf_free_blocks(sb, NULL, UDF_I(inode)->i_location, 0, 1);
}

struct inode *udf_new_inode(struct inode *dir, int mode, int *err)
{
    struct super_block *sb = dir->i_sb;
    struct udf_sb_info *sbi = UDF_SB(sb);
    struct inode *inode;
    int block;
    uint32_t start = UDF_I(dir)->i_location.logicalBlockNum;
    struct udf_inode_info *iinfo;
    struct udf_inode_info *dinfo = UDF_I(dir);

    inode = new_inode(sb);

    if (!inode) {
        *err = -ENOMEM;
        return NULL;
    }
    *err = -ENOSPC;

    iinfo = UDF_I(inode);
    iinfo->i_unique = 0;
    iinfo->i_lenExtents = 0;
    iinfo->i_next_alloc_block = 0;
    iinfo->i_next_alloc_goal = 0;
    iinfo->i_strat4096 = 0;

    block = udf_new_block(dir->i_sb, NULL,
                  dinfo->i_location.partitionReferenceNum,
                  start, err);
    if (*err) {
        iput(inode);
        return NULL;
    }

    mutex_lock(&sbi->s_alloc_mutex);
    if (sbi->s_lvid_bh) {
        struct logicalVolIntegrityDesc *lvid =
            (struct logicalVolIntegrityDesc *)
            sbi->s_lvid_bh->b_data;
        struct logicalVolIntegrityDescImpUse *lvidiu =
                            udf_sb_lvidiu(sbi);
        struct logicalVolHeaderDesc *lvhd;
        uint64_t uniqueID;
        lvhd = (struct logicalVolHeaderDesc *)
                (lvid->logicalVolContentsUse);
        if (S_ISDIR(mode))
            le32_add_cpu(&lvidiu->numDirs, 1);
        else
            le32_add_cpu(&lvidiu->numFiles, 1);
        iinfo->i_unique = uniqueID = le64_to_cpu(lvhd->uniqueID);
        if (!(++uniqueID & 0x00000000FFFFFFFFUL))
            uniqueID += 16;
        lvhd->uniqueID = cpu_to_le64(uniqueID);
        mark_buffer_dirty(sbi->s_lvid_bh);
    }
    inode->i_mode = mode;
    inode->i_uid = current->fsuid;
    if (dir->i_mode & S_ISGID) {
        inode->i_gid = dir->i_gid;
        if (S_ISDIR(mode))
            mode |= S_ISGID;
    } else {
        inode->i_gid = current->fsgid;
    }

    iinfo->i_location.logicalBlockNum = block;
    iinfo->i_location.partitionReferenceNum =
                dinfo->i_location.partitionReferenceNum;
    inode->i_ino = udf_get_lb_pblock(sb, iinfo->i_location, 0);
    inode->i_blocks = 0;
    iinfo->i_lenEAttr = 0;
    iinfo->i_lenAlloc = 0;
    iinfo->i_use = 0;
    if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_EXTENDED_FE)) {
        iinfo->i_efe = 1;
        if (UDF_VERS_USE_EXTENDED_FE > sbi->s_udfrev)
            sbi->s_udfrev = UDF_VERS_USE_EXTENDED_FE;
        iinfo->i_ext.i_data = kzalloc(inode->i_sb->s_blocksize -
                        sizeof(struct extendedFileEntry),
                        GFP_KERNEL);
    } else {
        iinfo->i_efe = 0;
        iinfo->i_ext.i_data = kzalloc(inode->i_sb->s_blocksize -
                        sizeof(struct fileEntry),
                        GFP_KERNEL);
    }
    if (!iinfo->i_ext.i_data) {
        iput(inode);
        *err = -ENOMEM;
        mutex_unlock(&sbi->s_alloc_mutex);
        return NULL;
    }
    if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_AD_IN_ICB))
        iinfo->i_alloc_type = ICBTAG_FLAG_AD_IN_ICB;
    else if (UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_USE_SHORT_AD))
        iinfo->i_alloc_type = ICBTAG_FLAG_AD_SHORT;
    else
        iinfo->i_alloc_type = ICBTAG_FLAG_AD_LONG;
    inode->i_mtime = inode->i_atime = inode->i_ctime =
        iinfo->i_crtime = current_fs_time(inode->i_sb);
    insert_inode_hash(inode);
    mark_inode_dirty(inode);
    mutex_unlock(&sbi->s_alloc_mutex);

    if (DQUOT_ALLOC_INODE(inode)) {
        DQUOT_DROP(inode);
        inode->i_flags |= S_NOQUOTA;
        inode->i_nlink = 0;
        iput(inode);
        *err = -EDQUOT;
        return NULL;
    }

    *err = 0;
    return inode;
}
