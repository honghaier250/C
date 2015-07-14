/* audit -- definition of audit_context structure and supporting types 
 *
 * Copyright 2003-2004 Red Hat, Inc.
 * Copyright 2005 Hewlett-Packard Development Company, L.P.
 * Copyright 2005 IBM Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/fs.h>
#include <linux/audit.h>
#include <linux/skbuff.h>

/* 0 = no checking
   1 = put_count checking
   2 = verbose put_count checking
*/
#define AUDIT_DEBUG 0

/* At task start time, the audit_state is set in the audit_context using
   a per-task filter.  At syscall entry, the audit_state is augmented by
   the syscall filter. */
enum audit_state {
    AUDIT_DISABLED,        /* Do not create per-task audit_context.
                 * No syscall-specific audit records can
                 * be generated. */
    AUDIT_SETUP_CONTEXT,    /* Create the per-task audit_context,
                 * but don't necessarily fill it in at
                 * syscall entry time (i.e., filter
                 * instead). */
    AUDIT_BUILD_CONTEXT,    /* Create the per-task audit_context,
                 * and always fill it in at syscall
                 * entry time.  This makes a full
                 * syscall record available if some
                 * other part of the kernel decides it
                 * should be recorded. */
    AUDIT_RECORD_CONTEXT    /* Create the per-task audit_context,
                 * always fill it in at syscall entry
                 * time, and always write out the audit
                 * record at syscall exit time.  */
};

/* Rule lists */
struct audit_parent;

struct audit_watch {
    atomic_t        count;    /* reference count */
    char            *path;    /* insertion path */
    dev_t            dev;    /* associated superblock device */
    unsigned long        ino;    /* associated inode number */
    struct audit_parent    *parent; /* associated parent */
    struct list_head    wlist;    /* entry in parent->watches list */
    struct list_head    rules;    /* associated rules */
};

struct audit_tree;
struct audit_chunk;

struct audit_entry {
    struct list_head    list;
    struct rcu_head        rcu;
    struct audit_krule    rule;
};

#ifdef CONFIG_AUDIT
extern int audit_enabled;
extern int audit_ever_enabled;
#endif

extern int audit_pid;

#define AUDIT_INODE_BUCKETS    32
extern struct list_head audit_inode_hash[AUDIT_INODE_BUCKETS];

static inline int audit_hash_ino(u32 ino)
{
    return (ino & (AUDIT_INODE_BUCKETS-1));
}

extern int audit_match_class(int class, unsigned syscall);
extern int audit_comparator(const u32 left, const u32 op, const u32 right);
extern int audit_compare_dname_path(const char *dname, const char *path,
                    int *dirlen);
extern struct sk_buff *        audit_make_reply(int pid, int seq, int type,
                         int done, int multi,
                         void *payload, int size);
extern void            audit_send_reply(int pid, int seq, int type,
                         int done, int multi,
                         void *payload, int size);
extern void            audit_panic(const char *message);

struct audit_netlink_list {
    int pid;
    struct sk_buff_head q;
};

int audit_send_list(void *);

struct inotify_watch;
/* Inotify handle */
extern struct inotify_handle *audit_ih;

extern void audit_free_parent(struct inotify_watch *);
extern void audit_handle_ievent(struct inotify_watch *, u32, u32, u32,
                const char *, struct inode *);
extern int selinux_audit_rule_update(void);

extern struct mutex audit_filter_mutex;
extern void audit_free_rule_rcu(struct rcu_head *);
extern struct list_head audit_filter_list[];

#ifdef CONFIG_AUDIT_TREE
extern struct audit_chunk *audit_tree_lookup(const struct inode *);
extern void audit_put_chunk(struct audit_chunk *);
extern int audit_tree_match(struct audit_chunk *, struct audit_tree *);
extern int audit_make_tree(struct audit_krule *, char *, u32);
extern int audit_add_tree_rule(struct audit_krule *);
extern int audit_remove_tree_rule(struct audit_krule *);
extern void audit_trim_trees(void);
extern int audit_tag_tree(char *old, char *new);
extern void audit_schedule_prune(void);
extern void audit_prune_trees(void);
extern const char *audit_tree_path(struct audit_tree *);
extern void audit_put_tree(struct audit_tree *);
#else
#define audit_remove_tree_rule(rule) BUG()
#define audit_add_tree_rule(rule) -EINVAL
#define audit_make_tree(rule, str, op) -EINVAL
#define audit_trim_trees() (void)0
#define audit_put_tree(tree) (void)0
#define audit_tag_tree(old, new) -EINVAL
#define audit_tree_path(rule) ""    /* never called */
#endif

extern char *audit_unpack_string(void **, size_t *, size_t);

extern pid_t audit_sig_pid;
extern uid_t audit_sig_uid;
extern u32 audit_sig_sid;

#ifdef CONFIG_AUDITSYSCALL
extern int __audit_signal_info(int sig, struct task_struct *t);
static inline int audit_signal_info(int sig, struct task_struct *t)
{
    if (unlikely((audit_pid && t->tgid == audit_pid) ||
             (audit_signals && !audit_dummy_context())))
        return __audit_signal_info(sig, t);
    return 0;
}
extern enum audit_state audit_filter_inodes(struct task_struct *,
                        struct audit_context *);
extern void audit_set_auditable(struct audit_context *);
#else
#define audit_signal_info(s,t) AUDIT_DISABLED
#define audit_filter_inodes(t,c) AUDIT_DISABLED
#define audit_set_auditable(c)
#endif
