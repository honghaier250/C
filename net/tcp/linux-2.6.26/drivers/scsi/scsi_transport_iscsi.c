/*
 * iSCSI transport class definitions
 *
 * Copyright (C) IBM Corporation, 2004
 * Copyright (C) Mike Christie, 2004 - 2005
 * Copyright (C) Dmitry Yusupov, 2004 - 2005
 * Copyright (C) Alex Aizman, 2004 - 2005
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#include <linux/module.h>
#include <linux/mutex.h>
#include <net/tcp.h>
#include <scsi/scsi.h>
#include <scsi/scsi_host.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_transport.h>
#include <scsi/scsi_transport_iscsi.h>
#include <scsi/iscsi_if.h>

#define ISCSI_SESSION_ATTRS 19
#define ISCSI_CONN_ATTRS 13
#define ISCSI_HOST_ATTRS 4
#define ISCSI_TRANSPORT_VERSION "2.0-869"

struct iscsi_internal {
    int daemon_pid;
    struct scsi_transport_template t;
    struct iscsi_transport *iscsi_transport;
    struct list_head list;
    struct device dev;

    struct device_attribute *host_attrs[ISCSI_HOST_ATTRS + 1];
    struct transport_container conn_cont;
    struct device_attribute *conn_attrs[ISCSI_CONN_ATTRS + 1];
    struct transport_container session_cont;
    struct device_attribute *session_attrs[ISCSI_SESSION_ATTRS + 1];
};

static atomic_t iscsi_session_nr; /* sysfs session id for next new session */
static struct workqueue_struct *iscsi_eh_timer_workq;

/*
 * list of registered transports and lock that must
 * be held while accessing list. The iscsi_transport_lock must
 * be acquired after the rx_queue_mutex.
 */
static LIST_HEAD(iscsi_transports);
static DEFINE_SPINLOCK(iscsi_transport_lock);

#define to_iscsi_internal(tmpl) \
    container_of(tmpl, struct iscsi_internal, t)

#define dev_to_iscsi_internal(_dev) \
    container_of(_dev, struct iscsi_internal, dev)

static void iscsi_transport_release(struct device *dev)
{
    struct iscsi_internal *priv = dev_to_iscsi_internal(dev);
    kfree(priv);
}

/*
 * iscsi_transport_class represents the iscsi_transports that are
 * registered.
 */
static struct class iscsi_transport_class = {
    .name = "iscsi_transport",
    .dev_release = iscsi_transport_release,
};

static ssize_t
show_transport_handle(struct device *dev, struct device_attribute *attr,
              char *buf)
{
    struct iscsi_internal *priv = dev_to_iscsi_internal(dev);
    return sprintf(buf, "%llu\n", (unsigned long long)iscsi_handle(priv->iscsi_transport));
}
static DEVICE_ATTR(handle, S_IRUGO, show_transport_handle, NULL);

#define show_transport_attr(name, format)                \
static ssize_t                                \
show_transport_##name(struct device *dev,                 \
              struct device_attribute *attr,char *buf)        \
{                                    \
    struct iscsi_internal *priv = dev_to_iscsi_internal(dev);    \
    return sprintf(buf, format"\n", priv->iscsi_transport->name);    \
}                                    \
static DEVICE_ATTR(name, S_IRUGO, show_transport_##name, NULL);

show_transport_attr(caps, "0x%x");
show_transport_attr(max_lun, "%d");
show_transport_attr(max_conn, "%d");
show_transport_attr(max_cmd_len, "%d");

static struct attribute *iscsi_transport_attrs[] = {
    &dev_attr_handle.attr,
    &dev_attr_caps.attr,
    &dev_attr_max_lun.attr,
    &dev_attr_max_conn.attr,
    &dev_attr_max_cmd_len.attr,
    NULL,
};

static struct attribute_group iscsi_transport_group = {
    .attrs = iscsi_transport_attrs,
};



static int iscsi_setup_host(struct transport_container *tc, struct device *dev,
                struct device *cdev)
{
    struct Scsi_Host *shost = dev_to_shost(dev);
    struct iscsi_host *ihost = shost->shost_data;

    memset(ihost, 0, sizeof(*ihost));
    INIT_LIST_HEAD(&ihost->sessions);
    mutex_init(&ihost->mutex);
    atomic_set(&ihost->nr_scans, 0);

    snprintf(ihost->scan_workq_name, KOBJ_NAME_LEN, "iscsi_scan_%d",
        shost->host_no);
    ihost->scan_workq = create_singlethread_workqueue(
                        ihost->scan_workq_name);
    if (!ihost->scan_workq)
        return -ENOMEM;
    return 0;
}

static int iscsi_remove_host(struct transport_container *tc, struct device *dev,
                 struct device *cdev)
{
    struct Scsi_Host *shost = dev_to_shost(dev);
    struct iscsi_host *ihost = shost->shost_data;

    destroy_workqueue(ihost->scan_workq);
    return 0;
}

static DECLARE_TRANSPORT_CLASS(iscsi_host_class,
                   "iscsi_host",
                   iscsi_setup_host,
                   iscsi_remove_host,
                   NULL);

static DECLARE_TRANSPORT_CLASS(iscsi_session_class,
                   "iscsi_session",
                   NULL,
                   NULL,
                   NULL);

static DECLARE_TRANSPORT_CLASS(iscsi_connection_class,
                   "iscsi_connection",
                   NULL,
                   NULL,
                   NULL);

static struct sock *nls;
static DEFINE_MUTEX(rx_queue_mutex);

static LIST_HEAD(sesslist);
static DEFINE_SPINLOCK(sesslock);
static LIST_HEAD(connlist);
static DEFINE_SPINLOCK(connlock);

static uint32_t iscsi_conn_get_sid(struct iscsi_cls_conn *conn)
{
    struct iscsi_cls_session *sess = iscsi_dev_to_session(conn->dev.parent);
    return sess->sid;
}

/*
 * Returns the matching session to a given sid
 */
static struct iscsi_cls_session *iscsi_session_lookup(uint32_t sid)
{
    unsigned long flags;
    struct iscsi_cls_session *sess;

    spin_lock_irqsave(&sesslock, flags);
    list_for_each_entry(sess, &sesslist, sess_list) {
        if (sess->sid == sid) {
            spin_unlock_irqrestore(&sesslock, flags);
            return sess;
        }
    }
    spin_unlock_irqrestore(&sesslock, flags);
    return NULL;
}

/*
 * Returns the matching connection to a given sid / cid tuple
 */
static struct iscsi_cls_conn *iscsi_conn_lookup(uint32_t sid, uint32_t cid)
{
    unsigned long flags;
    struct iscsi_cls_conn *conn;

    spin_lock_irqsave(&connlock, flags);
    list_for_each_entry(conn, &connlist, conn_list) {
        if ((conn->cid == cid) && (iscsi_conn_get_sid(conn) == sid)) {
            spin_unlock_irqrestore(&connlock, flags);
            return conn;
        }
    }
    spin_unlock_irqrestore(&connlock, flags);
    return NULL;
}

/*
 * The following functions can be used by LLDs that allocate
 * their own scsi_hosts or by software iscsi LLDs
 */
static struct {
    int value;
    char *name;
} iscsi_session_state_names[] = {
    { ISCSI_SESSION_LOGGED_IN,    "LOGGED_IN" },
    { ISCSI_SESSION_FAILED,        "FAILED" },
    { ISCSI_SESSION_FREE,        "FREE" },
};

static const char *iscsi_session_state_name(int state)
{
    int i;
    char *name = NULL;

    for (i = 0; i < ARRAY_SIZE(iscsi_session_state_names); i++) {
        if (iscsi_session_state_names[i].value == state) {
            name = iscsi_session_state_names[i].name;
            break;
        }
    }
    return name;
}

int iscsi_session_chkready(struct iscsi_cls_session *session)
{
    unsigned long flags;
    int err;

    spin_lock_irqsave(&session->lock, flags);
    switch (session->state) {
    case ISCSI_SESSION_LOGGED_IN:
        err = 0;
        break;
    case ISCSI_SESSION_FAILED:
        err = DID_IMM_RETRY << 16;
        break;
    case ISCSI_SESSION_FREE:
        err = DID_NO_CONNECT << 16;
        break;
    default:
        err = DID_NO_CONNECT << 16;
        break;
    }
    spin_unlock_irqrestore(&session->lock, flags);
    return err;
}
EXPORT_SYMBOL_GPL(iscsi_session_chkready);

static void iscsi_session_release(struct device *dev)
{
    struct iscsi_cls_session *session = iscsi_dev_to_session(dev);
    struct Scsi_Host *shost;

    shost = iscsi_session_to_shost(session);
    scsi_host_put(shost);
    kfree(session);
}

static int iscsi_is_session_dev(const struct device *dev)
{
    return dev->release == iscsi_session_release;
}

/**
 * iscsi_scan_finished - helper to report when running scans are done
 * @shost: scsi host
 * @time: scan run time
 *
 * This function can be used by drives like qla4xxx to report to the scsi
 * layer when the scans it kicked off at module load time are done.
 */
int iscsi_scan_finished(struct Scsi_Host *shost, unsigned long time)
{
    struct iscsi_host *ihost = shost->shost_data;
    /*
     * qla4xxx will have kicked off some session unblocks before calling
     * scsi_scan_host, so just wait for them to complete.
     */
    return !atomic_read(&ihost->nr_scans);
}
EXPORT_SYMBOL_GPL(iscsi_scan_finished);

static int iscsi_user_scan(struct Scsi_Host *shost, uint channel,
               uint id, uint lun)
{
    struct iscsi_host *ihost = shost->shost_data;
    struct iscsi_cls_session *session;

    mutex_lock(&ihost->mutex);
    list_for_each_entry(session, &ihost->sessions, host_list) {
        if ((channel == SCAN_WILD_CARD || channel == 0) &&
            (id == SCAN_WILD_CARD || id == session->target_id))
            scsi_scan_target(&session->dev, 0,
                     session->target_id, lun, 1);
    }
    mutex_unlock(&ihost->mutex);

    return 0;
}

static void iscsi_scan_session(struct work_struct *work)
{
    struct iscsi_cls_session *session =
            container_of(work, struct iscsi_cls_session, scan_work);
    struct Scsi_Host *shost = iscsi_session_to_shost(session);
    struct iscsi_host *ihost = shost->shost_data;
    unsigned long flags;

    spin_lock_irqsave(&session->lock, flags);
    if (session->state != ISCSI_SESSION_LOGGED_IN) {
        spin_unlock_irqrestore(&session->lock, flags);
        goto done;
    }
    spin_unlock_irqrestore(&session->lock, flags);

    scsi_scan_target(&session->dev, 0, session->target_id,
             SCAN_WILD_CARD, 1);
done:
    atomic_dec(&ihost->nr_scans);
}

static void session_recovery_timedout(struct work_struct *work)
{
    struct iscsi_cls_session *session =
        container_of(work, struct iscsi_cls_session,
                 recovery_work.work);
    unsigned long flags;

    iscsi_cls_session_printk(KERN_INFO, session,
                 "session recovery timed out after %d secs\n",
                 session->recovery_tmo);

    spin_lock_irqsave(&session->lock, flags);
    switch (session->state) {
    case ISCSI_SESSION_FAILED:
        session->state = ISCSI_SESSION_FREE;
        break;
    case ISCSI_SESSION_LOGGED_IN:
    case ISCSI_SESSION_FREE:
        /* we raced with the unblock's flush */
        spin_unlock_irqrestore(&session->lock, flags);
        return;
    }
    spin_unlock_irqrestore(&session->lock, flags);

    if (session->transport->session_recovery_timedout)
        session->transport->session_recovery_timedout(session);

    scsi_target_unblock(&session->dev);
}

static void __iscsi_unblock_session(struct work_struct *work)
{
    struct iscsi_cls_session *session =
            container_of(work, struct iscsi_cls_session,
                     unblock_work);
    struct Scsi_Host *shost = iscsi_session_to_shost(session);
    struct iscsi_host *ihost = shost->shost_data;
    unsigned long flags;

    /*
     * The recovery and unblock work get run from the same workqueue,
     * so try to cancel it if it was going to run after this unblock.
     */
    cancel_delayed_work(&session->recovery_work);
    spin_lock_irqsave(&session->lock, flags);
    session->state = ISCSI_SESSION_LOGGED_IN;
    spin_unlock_irqrestore(&session->lock, flags);
    /* start IO */
    scsi_target_unblock(&session->dev);
    /*
     * Only do kernel scanning if the driver is properly hooked into
     * the async scanning code (drivers like iscsi_tcp do login and
     * scanning from userspace).
     */
    if (shost->hostt->scan_finished) {
        if (queue_work(ihost->scan_workq, &session->scan_work))
            atomic_inc(&ihost->nr_scans);
    }
}

/**
 * iscsi_unblock_session - set a session as logged in and start IO.
 * @session: iscsi session
 *
 * Mark a session as ready to accept IO.
 */
void iscsi_unblock_session(struct iscsi_cls_session *session)
{
    queue_work(iscsi_eh_timer_workq, &session->unblock_work);
    /*
     * make sure all the events have completed before tell the driver
     * it is safe
     */
    flush_workqueue(iscsi_eh_timer_workq);
}
EXPORT_SYMBOL_GPL(iscsi_unblock_session);

static void __iscsi_block_session(struct work_struct *work)
{
    struct iscsi_cls_session *session =
            container_of(work, struct iscsi_cls_session,
                     block_work);
    unsigned long flags;

    spin_lock_irqsave(&session->lock, flags);
    session->state = ISCSI_SESSION_FAILED;
    spin_unlock_irqrestore(&session->lock, flags);
    scsi_target_block(&session->dev);
    queue_delayed_work(iscsi_eh_timer_workq, &session->recovery_work,
               session->recovery_tmo * HZ);
}

void iscsi_block_session(struct iscsi_cls_session *session)
{
    queue_work(iscsi_eh_timer_workq, &session->block_work);
}
EXPORT_SYMBOL_GPL(iscsi_block_session);

static void __iscsi_unbind_session(struct work_struct *work)
{
    struct iscsi_cls_session *session =
            container_of(work, struct iscsi_cls_session,
                     unbind_work);
    struct Scsi_Host *shost = iscsi_session_to_shost(session);
    struct iscsi_host *ihost = shost->shost_data;

    /* Prevent new scans and make sure scanning is not in progress */
    mutex_lock(&ihost->mutex);
    if (list_empty(&session->host_list)) {
        mutex_unlock(&ihost->mutex);
        return;
    }
    list_del_init(&session->host_list);
    mutex_unlock(&ihost->mutex);

    scsi_remove_target(&session->dev);
    iscsi_session_event(session, ISCSI_KEVENT_UNBIND_SESSION);
}

static int iscsi_unbind_session(struct iscsi_cls_session *session)
{
    struct Scsi_Host *shost = iscsi_session_to_shost(session);
    struct iscsi_host *ihost = shost->shost_data;

    return queue_work(ihost->scan_workq, &session->unbind_work);
}

struct iscsi_cls_session *
iscsi_alloc_session(struct Scsi_Host *shost,
            struct iscsi_transport *transport)
{
    struct iscsi_cls_session *session;

    session = kzalloc(sizeof(*session) + transport->sessiondata_size,
              GFP_KERNEL);
    if (!session)
        return NULL;

    session->transport = transport;
    session->recovery_tmo = 120;
    session->state = ISCSI_SESSION_FREE;
    INIT_DELAYED_WORK(&session->recovery_work, session_recovery_timedout);
    INIT_LIST_HEAD(&session->host_list);
    INIT_LIST_HEAD(&session->sess_list);
    INIT_WORK(&session->unblock_work, __iscsi_unblock_session);
    INIT_WORK(&session->block_work, __iscsi_block_session);
    INIT_WORK(&session->unbind_work, __iscsi_unbind_session);
    INIT_WORK(&session->scan_work, iscsi_scan_session);
    spin_lock_init(&session->lock);

    /* this is released in the dev's release function */
    scsi_host_get(shost);
    session->dev.parent = &shost->shost_gendev;
    session->dev.release = iscsi_session_release;
    device_initialize(&session->dev);
    if (transport->sessiondata_size)
        session->dd_data = &session[1];
    return session;
}
EXPORT_SYMBOL_GPL(iscsi_alloc_session);

int iscsi_add_session(struct iscsi_cls_session *session, unsigned int target_id)
{
    struct Scsi_Host *shost = iscsi_session_to_shost(session);
    struct iscsi_host *ihost;
    unsigned long flags;
    int err;

    ihost = shost->shost_data;
    session->sid = atomic_add_return(1, &iscsi_session_nr);
    session->target_id = target_id;

    snprintf(session->dev.bus_id, BUS_ID_SIZE, "session%u",
         session->sid);
    err = device_add(&session->dev);
    if (err) {
        iscsi_cls_session_printk(KERN_ERR, session,
                     "could not register session's dev\n");
        goto release_host;
    }
    transport_register_device(&session->dev);

    spin_lock_irqsave(&sesslock, flags);
    list_add(&session->sess_list, &sesslist);
    spin_unlock_irqrestore(&sesslock, flags);

    mutex_lock(&ihost->mutex);
    list_add(&session->host_list, &ihost->sessions);
    mutex_unlock(&ihost->mutex);

    iscsi_session_event(session, ISCSI_KEVENT_CREATE_SESSION);
    return 0;

release_host:
    scsi_host_put(shost);
    return err;
}
EXPORT_SYMBOL_GPL(iscsi_add_session);

/**
 * iscsi_create_session - create iscsi class session
 * @shost: scsi host
 * @transport: iscsi transport
 * @target_id: which target
 *
 * This can be called from a LLD or iscsi_transport.
 */
struct iscsi_cls_session *
iscsi_create_session(struct Scsi_Host *shost,
             struct iscsi_transport *transport,
             unsigned int target_id)
{
    struct iscsi_cls_session *session;

    session = iscsi_alloc_session(shost, transport);
    if (!session)
        return NULL;

    if (iscsi_add_session(session, target_id)) {
        iscsi_free_session(session);
        return NULL;
    }
    return session;
}
EXPORT_SYMBOL_GPL(iscsi_create_session);

static void iscsi_conn_release(struct device *dev)
{
    struct iscsi_cls_conn *conn = iscsi_dev_to_conn(dev);
    struct device *parent = conn->dev.parent;

    kfree(conn);
    put_device(parent);
}

static int iscsi_is_conn_dev(const struct device *dev)
{
    return dev->release == iscsi_conn_release;
}

static int iscsi_iter_destroy_conn_fn(struct device *dev, void *data)
{
    if (!iscsi_is_conn_dev(dev))
        return 0;
    return iscsi_destroy_conn(iscsi_dev_to_conn(dev));
}

void iscsi_remove_session(struct iscsi_cls_session *session)
{
    struct Scsi_Host *shost = iscsi_session_to_shost(session);
    struct iscsi_host *ihost = shost->shost_data;
    unsigned long flags;
    int err;

    spin_lock_irqsave(&sesslock, flags);
    list_del(&session->sess_list);
    spin_unlock_irqrestore(&sesslock, flags);

    /* make sure there are no blocks/unblocks queued */
    flush_workqueue(iscsi_eh_timer_workq);
    /* make sure the timedout callout is not running */
    if (!cancel_delayed_work(&session->recovery_work))
        flush_workqueue(iscsi_eh_timer_workq);
    /*
     * If we are blocked let commands flow again. The lld or iscsi
     * layer should set up the queuecommand to fail commands.
     * We assume that LLD will not be calling block/unblock while
     * removing the session.
     */
    spin_lock_irqsave(&session->lock, flags);
    session->state = ISCSI_SESSION_FREE;
    spin_unlock_irqrestore(&session->lock, flags);

    scsi_target_unblock(&session->dev);
    /* flush running scans then delete devices */
    flush_workqueue(ihost->scan_workq);
    __iscsi_unbind_session(&session->unbind_work);

    /* hw iscsi may not have removed all connections from session */
    err = device_for_each_child(&session->dev, NULL,
                    iscsi_iter_destroy_conn_fn);
    if (err)
        iscsi_cls_session_printk(KERN_ERR, session,
                     "Could not delete all connections "
                     "for session. Error %d.\n", err);

    transport_unregister_device(&session->dev);
    device_del(&session->dev);
}
EXPORT_SYMBOL_GPL(iscsi_remove_session);

void iscsi_free_session(struct iscsi_cls_session *session)
{
    iscsi_session_event(session, ISCSI_KEVENT_DESTROY_SESSION);
    put_device(&session->dev);
}
EXPORT_SYMBOL_GPL(iscsi_free_session);

/**
 * iscsi_destroy_session - destroy iscsi session
 * @session: iscsi_session
 *
 * Can be called by a LLD or iscsi_transport. There must not be
 * any running connections.
 */
int iscsi_destroy_session(struct iscsi_cls_session *session)
{
    iscsi_remove_session(session);
    iscsi_free_session(session);
    return 0;
}
EXPORT_SYMBOL_GPL(iscsi_destroy_session);

/**
 * iscsi_create_conn - create iscsi class connection
 * @session: iscsi cls session
 * @cid: connection id
 *
 * This can be called from a LLD or iscsi_transport. The connection
 * is child of the session so cid must be unique for all connections
 * on the session.
 *
 * Since we do not support MCS, cid will normally be zero. In some cases
 * for software iscsi we could be trying to preallocate a connection struct
 * in which case there could be two connection structs and cid would be
 * non-zero.
 */
struct iscsi_cls_conn *
iscsi_create_conn(struct iscsi_cls_session *session, uint32_t cid)
{
    struct iscsi_transport *transport = session->transport;
    struct iscsi_cls_conn *conn;
    unsigned long flags;
    int err;

    conn = kzalloc(sizeof(*conn) + transport->conndata_size, GFP_KERNEL);
    if (!conn)
        return NULL;

    if (transport->conndata_size)
        conn->dd_data = &conn[1];

    INIT_LIST_HEAD(&conn->conn_list);
    conn->transport = transport;
    conn->cid = cid;

    /* this is released in the dev's release function */
    if (!get_device(&session->dev))
        goto free_conn;

    snprintf(conn->dev.bus_id, BUS_ID_SIZE, "connection%d:%u",
         session->sid, cid);
    conn->dev.parent = &session->dev;
    conn->dev.release = iscsi_conn_release;
    err = device_register(&conn->dev);
    if (err) {
        iscsi_cls_session_printk(KERN_ERR, session, "could not "
                     "register connection's dev\n");
        goto release_parent_ref;
    }
    transport_register_device(&conn->dev);

    spin_lock_irqsave(&connlock, flags);
    list_add(&conn->conn_list, &connlist);
    conn->active = 1;
    spin_unlock_irqrestore(&connlock, flags);
    return conn;

release_parent_ref:
    put_device(&session->dev);
free_conn:
    kfree(conn);
    return NULL;
}

EXPORT_SYMBOL_GPL(iscsi_create_conn);

/**
 * iscsi_destroy_conn - destroy iscsi class connection
 * @conn: iscsi cls session
 *
 * This can be called from a LLD or iscsi_transport.
 */
int iscsi_destroy_conn(struct iscsi_cls_conn *conn)
{
    unsigned long flags;

    spin_lock_irqsave(&connlock, flags);
    conn->active = 0;
    list_del(&conn->conn_list);
    spin_unlock_irqrestore(&connlock, flags);

    transport_unregister_device(&conn->dev);
    device_unregister(&conn->dev);
    return 0;
}
EXPORT_SYMBOL_GPL(iscsi_destroy_conn);

/*
 * iscsi interface functions
 */
static struct iscsi_internal *
iscsi_if_transport_lookup(struct iscsi_transport *tt)
{
    struct iscsi_internal *priv;
    unsigned long flags;

    spin_lock_irqsave(&iscsi_transport_lock, flags);
    list_for_each_entry(priv, &iscsi_transports, list) {
        if (tt == priv->iscsi_transport) {
            spin_unlock_irqrestore(&iscsi_transport_lock, flags);
            return priv;
        }
    }
    spin_unlock_irqrestore(&iscsi_transport_lock, flags);
    return NULL;
}

static int
iscsi_broadcast_skb(struct sk_buff *skb, gfp_t gfp)
{
    int rc;

    rc = netlink_broadcast(nls, skb, 0, 1, gfp);
    if (rc < 0) {
        printk(KERN_ERR "iscsi: can not broadcast skb (%d)\n", rc);
        return rc;
    }

    return 0;
}

static int
iscsi_unicast_skb(struct sk_buff *skb, int pid)
{
    int rc;

    rc = netlink_unicast(nls, skb, pid, MSG_DONTWAIT);
    if (rc < 0) {
        printk(KERN_ERR "iscsi: can not unicast skb (%d)\n", rc);
        return rc;
    }

    return 0;
}

int iscsi_recv_pdu(struct iscsi_cls_conn *conn, struct iscsi_hdr *hdr,
           char *data, uint32_t data_size)
{
    struct nlmsghdr    *nlh;
    struct sk_buff *skb;
    struct iscsi_uevent *ev;
    char *pdu;
    struct iscsi_internal *priv;
    int len = NLMSG_SPACE(sizeof(*ev) + sizeof(struct iscsi_hdr) +
                  data_size);

    priv = iscsi_if_transport_lookup(conn->transport);
    if (!priv)
        return -EINVAL;

    skb = alloc_skb(len, GFP_ATOMIC);
    if (!skb) {
        iscsi_conn_error(conn, ISCSI_ERR_CONN_FAILED);
        iscsi_cls_conn_printk(KERN_ERR, conn, "can not deliver "
                      "control PDU: OOM\n");
        return -ENOMEM;
    }

    nlh = __nlmsg_put(skb, priv->daemon_pid, 0, 0, (len - sizeof(*nlh)), 0);
    ev = NLMSG_DATA(nlh);
    memset(ev, 0, sizeof(*ev));
    ev->transport_handle = iscsi_handle(conn->transport);
    ev->type = ISCSI_KEVENT_RECV_PDU;
    ev->r.recv_req.cid = conn->cid;
    ev->r.recv_req.sid = iscsi_conn_get_sid(conn);
    pdu = (char*)ev + sizeof(*ev);
    memcpy(pdu, hdr, sizeof(struct iscsi_hdr));
    memcpy(pdu + sizeof(struct iscsi_hdr), data, data_size);

    return iscsi_unicast_skb(skb, priv->daemon_pid);
}
EXPORT_SYMBOL_GPL(iscsi_recv_pdu);

void iscsi_conn_error(struct iscsi_cls_conn *conn, enum iscsi_err error)
{
    struct nlmsghdr    *nlh;
    struct sk_buff    *skb;
    struct iscsi_uevent *ev;
    struct iscsi_internal *priv;
    int len = NLMSG_SPACE(sizeof(*ev));

    priv = iscsi_if_transport_lookup(conn->transport);
    if (!priv)
        return;

    skb = alloc_skb(len, GFP_ATOMIC);
    if (!skb) {
        iscsi_cls_conn_printk(KERN_ERR, conn, "gracefully ignored "
                      "conn error (%d)\n", error);
        return;
    }

    nlh = __nlmsg_put(skb, priv->daemon_pid, 0, 0, (len - sizeof(*nlh)), 0);
    ev = NLMSG_DATA(nlh);
    ev->transport_handle = iscsi_handle(conn->transport);
    ev->type = ISCSI_KEVENT_CONN_ERROR;
    ev->r.connerror.error = error;
    ev->r.connerror.cid = conn->cid;
    ev->r.connerror.sid = iscsi_conn_get_sid(conn);

    iscsi_broadcast_skb(skb, GFP_ATOMIC);

    iscsi_cls_conn_printk(KERN_INFO, conn, "detected conn error (%d)\n",
                  error);
}
EXPORT_SYMBOL_GPL(iscsi_conn_error);

static int
iscsi_if_send_reply(int pid, int seq, int type, int done, int multi,
              void *payload, int size)
{
    struct sk_buff    *skb;
    struct nlmsghdr    *nlh;
    int len = NLMSG_SPACE(size);
    int flags = multi ? NLM_F_MULTI : 0;
    int t = done ? NLMSG_DONE : type;

    skb = alloc_skb(len, GFP_ATOMIC);
    if (!skb) {
        printk(KERN_ERR "Could not allocate skb to send reply.\n");
        return -ENOMEM;
    }

    nlh = __nlmsg_put(skb, pid, seq, t, (len - sizeof(*nlh)), 0);
    nlh->nlmsg_flags = flags;
    memcpy(NLMSG_DATA(nlh), payload, size);
    return iscsi_unicast_skb(skb, pid);
}

static int
iscsi_if_get_stats(struct iscsi_transport *transport, struct nlmsghdr *nlh)
{
    struct iscsi_uevent *ev = NLMSG_DATA(nlh);
    struct iscsi_stats *stats;
    struct sk_buff *skbstat;
    struct iscsi_cls_conn *conn;
    struct nlmsghdr    *nlhstat;
    struct iscsi_uevent *evstat;
    struct iscsi_internal *priv;
    int len = NLMSG_SPACE(sizeof(*ev) +
                  sizeof(struct iscsi_stats) +
                  sizeof(struct iscsi_stats_custom) *
                  ISCSI_STATS_CUSTOM_MAX);
    int err = 0;

    priv = iscsi_if_transport_lookup(transport);
    if (!priv)
        return -EINVAL;

    conn = iscsi_conn_lookup(ev->u.get_stats.sid, ev->u.get_stats.cid);
    if (!conn)
        return -EEXIST;

    do {
        int actual_size;

        skbstat = alloc_skb(len, GFP_ATOMIC);
        if (!skbstat) {
            iscsi_cls_conn_printk(KERN_ERR, conn, "can not "
                          "deliver stats: OOM\n");
            return -ENOMEM;
        }

        nlhstat = __nlmsg_put(skbstat, priv->daemon_pid, 0, 0,
                      (len - sizeof(*nlhstat)), 0);
        evstat = NLMSG_DATA(nlhstat);
        memset(evstat, 0, sizeof(*evstat));
        evstat->transport_handle = iscsi_handle(conn->transport);
        evstat->type = nlh->nlmsg_type;
        evstat->u.get_stats.cid =
            ev->u.get_stats.cid;
        evstat->u.get_stats.sid =
            ev->u.get_stats.sid;
        stats = (struct iscsi_stats *)
            ((char*)evstat + sizeof(*evstat));
        memset(stats, 0, sizeof(*stats));

        transport->get_stats(conn, stats);
        actual_size = NLMSG_SPACE(sizeof(struct iscsi_uevent) +
                      sizeof(struct iscsi_stats) +
                      sizeof(struct iscsi_stats_custom) *
                      stats->custom_length);
        actual_size -= sizeof(*nlhstat);
        actual_size = NLMSG_LENGTH(actual_size);
        skb_trim(skbstat, NLMSG_ALIGN(actual_size));
        nlhstat->nlmsg_len = actual_size;

        err = iscsi_unicast_skb(skbstat, priv->daemon_pid);
    } while (err < 0 && err != -ECONNREFUSED);

    return err;
}

/**
 * iscsi_session_event - send session destr. completion event
 * @session: iscsi class session
 * @event: type of event
 */
int iscsi_session_event(struct iscsi_cls_session *session,
            enum iscsi_uevent_e event)
{
    struct iscsi_internal *priv;
    struct Scsi_Host *shost;
    struct iscsi_uevent *ev;
    struct sk_buff  *skb;
    struct nlmsghdr *nlh;
    int rc, len = NLMSG_SPACE(sizeof(*ev));

    priv = iscsi_if_transport_lookup(session->transport);
    if (!priv)
        return -EINVAL;
    shost = iscsi_session_to_shost(session);

    skb = alloc_skb(len, GFP_KERNEL);
    if (!skb) {
        iscsi_cls_session_printk(KERN_ERR, session,
                     "Cannot notify userspace of session "
                     "event %u\n", event);
        return -ENOMEM;
    }

    nlh = __nlmsg_put(skb, priv->daemon_pid, 0, 0, (len - sizeof(*nlh)), 0);
    ev = NLMSG_DATA(nlh);
    ev->transport_handle = iscsi_handle(session->transport);

    ev->type = event;
    switch (event) {
    case ISCSI_KEVENT_DESTROY_SESSION:
        ev->r.d_session.host_no = shost->host_no;
        ev->r.d_session.sid = session->sid;
        break;
    case ISCSI_KEVENT_CREATE_SESSION:
        ev->r.c_session_ret.host_no = shost->host_no;
        ev->r.c_session_ret.sid = session->sid;
        break;
    case ISCSI_KEVENT_UNBIND_SESSION:
        ev->r.unbind_session.host_no = shost->host_no;
        ev->r.unbind_session.sid = session->sid;
        break;
    default:
        iscsi_cls_session_printk(KERN_ERR, session, "Invalid event "
                     "%u.\n", event);
        kfree_skb(skb);
        return -EINVAL;
    }

    /*
     * this will occur if the daemon is not up, so we just warn
     * the user and when the daemon is restarted it will handle it
     */
    rc = iscsi_broadcast_skb(skb, GFP_KERNEL);
    if (rc < 0)
        iscsi_cls_session_printk(KERN_ERR, session,
                     "Cannot notify userspace of session "
                     "event %u. Check iscsi daemon\n",
                     event);
    return rc;
}
EXPORT_SYMBOL_GPL(iscsi_session_event);

static int
iscsi_if_create_session(struct iscsi_internal *priv, struct iscsi_uevent *ev)
{
    struct iscsi_transport *transport = priv->iscsi_transport;
    struct iscsi_cls_session *session;
    uint32_t hostno;

    session = transport->create_session(transport, &priv->t,
                        ev->u.c_session.cmds_max,
                        ev->u.c_session.queue_depth,
                        ev->u.c_session.initial_cmdsn,
                        &hostno);
    if (!session)
        return -ENOMEM;

    ev->r.c_session_ret.host_no = hostno;
    ev->r.c_session_ret.sid = session->sid;
    return 0;
}

static int
iscsi_if_create_conn(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
    struct iscsi_cls_conn *conn;
    struct iscsi_cls_session *session;

    session = iscsi_session_lookup(ev->u.c_conn.sid);
    if (!session) {
        printk(KERN_ERR "iscsi: invalid session %d.\n",
               ev->u.c_conn.sid);
        return -EINVAL;
    }

    conn = transport->create_conn(session, ev->u.c_conn.cid);
    if (!conn) {
        iscsi_cls_session_printk(KERN_ERR, session,
                     "couldn't create a new connection.");
        return -ENOMEM;
    }

    ev->r.c_conn_ret.sid = session->sid;
    ev->r.c_conn_ret.cid = conn->cid;
    return 0;
}

static int
iscsi_if_destroy_conn(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
    struct iscsi_cls_conn *conn;

    conn = iscsi_conn_lookup(ev->u.d_conn.sid, ev->u.d_conn.cid);
    if (!conn)
        return -EINVAL;

    if (transport->destroy_conn)
        transport->destroy_conn(conn);
    return 0;
}

static int
iscsi_set_param(struct iscsi_transport *transport, struct iscsi_uevent *ev)
{
    char *data = (char*)ev + sizeof(*ev);
    struct iscsi_cls_conn *conn;
    struct iscsi_cls_session *session;
    int err = 0, value = 0;

    session = iscsi_session_lookup(ev->u.set_param.sid);
    conn = iscsi_conn_lookup(ev->u.set_param.sid, ev->u.set_param.cid);
    if (!conn || !session)
        return -EINVAL;

    switch (ev->u.set_param.param) {
    case ISCSI_PARAM_SESS_RECOVERY_TMO:
        sscanf(data, "%d", &value);
        if (value != 0)
            session->recovery_tmo = value;
        break;
    default:
        err = transport->set_param(conn, ev->u.set_param.param,
                       data, ev->u.set_param.len);
    }

    return err;
}

static int
iscsi_if_transport_ep(struct iscsi_transport *transport,
              struct iscsi_uevent *ev, int msg_type)
{
    struct sockaddr *dst_addr;
    int rc = 0;

    switch (msg_type) {
    case ISCSI_UEVENT_TRANSPORT_EP_CONNECT:
        if (!transport->ep_connect)
            return -EINVAL;

        dst_addr = (struct sockaddr *)((char*)ev + sizeof(*ev));
        rc = transport->ep_connect(dst_addr,
                       ev->u.ep_connect.non_blocking,
                       &ev->r.ep_connect_ret.handle);
        break;
    case ISCSI_UEVENT_TRANSPORT_EP_POLL:
        if (!transport->ep_poll)
            return -EINVAL;

        ev->r.retcode = transport->ep_poll(ev->u.ep_poll.ep_handle,
                           ev->u.ep_poll.timeout_ms);
        break;
    case ISCSI_UEVENT_TRANSPORT_EP_DISCONNECT:
        if (!transport->ep_disconnect)
            return -EINVAL;

        transport->ep_disconnect(ev->u.ep_disconnect.ep_handle);
        break;
    }
    return rc;
}

static int
iscsi_tgt_dscvr(struct iscsi_transport *transport,
        struct iscsi_uevent *ev)
{
    struct Scsi_Host *shost;
    struct sockaddr *dst_addr;
    int err;

    if (!transport->tgt_dscvr)
        return -EINVAL;

    shost = scsi_host_lookup(ev->u.tgt_dscvr.host_no);
    if (IS_ERR(shost)) {
        printk(KERN_ERR "target discovery could not find host no %u\n",
               ev->u.tgt_dscvr.host_no);
        return -ENODEV;
    }


    dst_addr = (struct sockaddr *)((char*)ev + sizeof(*ev));
    err = transport->tgt_dscvr(shost, ev->u.tgt_dscvr.type,
                   ev->u.tgt_dscvr.enable, dst_addr);
    scsi_host_put(shost);
    return err;
}

static int
iscsi_set_host_param(struct iscsi_transport *transport,
             struct iscsi_uevent *ev)
{
    char *data = (char*)ev + sizeof(*ev);
    struct Scsi_Host *shost;
    int err;

    if (!transport->set_host_param)
        return -ENOSYS;

    shost = scsi_host_lookup(ev->u.set_host_param.host_no);
    if (IS_ERR(shost)) {
        printk(KERN_ERR "set_host_param could not find host no %u\n",
               ev->u.set_host_param.host_no);
        return -ENODEV;
    }

    err = transport->set_host_param(shost, ev->u.set_host_param.param,
                    data, ev->u.set_host_param.len);
    scsi_host_put(shost);
    return err;
}

static int
iscsi_if_recv_msg(struct sk_buff *skb, struct nlmsghdr *nlh)
{
    int err = 0;
    struct iscsi_uevent *ev = NLMSG_DATA(nlh);
    struct iscsi_transport *transport = NULL;
    struct iscsi_internal *priv;
    struct iscsi_cls_session *session;
    struct iscsi_cls_conn *conn;

    priv = iscsi_if_transport_lookup(iscsi_ptr(ev->transport_handle));
    if (!priv)
        return -EINVAL;
    transport = priv->iscsi_transport;

    if (!try_module_get(transport->owner))
        return -EINVAL;

    priv->daemon_pid = NETLINK_CREDS(skb)->pid;

    switch (nlh->nlmsg_type) {
    case ISCSI_UEVENT_CREATE_SESSION:
        err = iscsi_if_create_session(priv, ev);
        break;
    case ISCSI_UEVENT_DESTROY_SESSION:
        session = iscsi_session_lookup(ev->u.d_session.sid);
        if (session)
            transport->destroy_session(session);
        else
            err = -EINVAL;
        break;
    case ISCSI_UEVENT_UNBIND_SESSION:
        session = iscsi_session_lookup(ev->u.d_session.sid);
        if (session)
            iscsi_unbind_session(session);
        else
            err = -EINVAL;
        break;
    case ISCSI_UEVENT_CREATE_CONN:
        err = iscsi_if_create_conn(transport, ev);
        break;
    case ISCSI_UEVENT_DESTROY_CONN:
        err = iscsi_if_destroy_conn(transport, ev);
        break;
    case ISCSI_UEVENT_BIND_CONN:
        session = iscsi_session_lookup(ev->u.b_conn.sid);
        conn = iscsi_conn_lookup(ev->u.b_conn.sid, ev->u.b_conn.cid);

        if (session && conn)
            ev->r.retcode =    transport->bind_conn(session, conn,
                    ev->u.b_conn.transport_eph,
                    ev->u.b_conn.is_leading);
        else
            err = -EINVAL;
        break;
    case ISCSI_UEVENT_SET_PARAM:
        err = iscsi_set_param(transport, ev);
        break;
    case ISCSI_UEVENT_START_CONN:
        conn = iscsi_conn_lookup(ev->u.start_conn.sid, ev->u.start_conn.cid);
        if (conn)
            ev->r.retcode = transport->start_conn(conn);
        else
            err = -EINVAL;
        break;
    case ISCSI_UEVENT_STOP_CONN:
        conn = iscsi_conn_lookup(ev->u.stop_conn.sid, ev->u.stop_conn.cid);
        if (conn)
            transport->stop_conn(conn, ev->u.stop_conn.flag);
        else
            err = -EINVAL;
        break;
    case ISCSI_UEVENT_SEND_PDU:
        conn = iscsi_conn_lookup(ev->u.send_pdu.sid, ev->u.send_pdu.cid);
        if (conn)
            ev->r.retcode =    transport->send_pdu(conn,
                (struct iscsi_hdr*)((char*)ev + sizeof(*ev)),
                (char*)ev + sizeof(*ev) + ev->u.send_pdu.hdr_size,
                ev->u.send_pdu.data_size);
        else
            err = -EINVAL;
        break;
    case ISCSI_UEVENT_GET_STATS:
        err = iscsi_if_get_stats(transport, nlh);
        break;
    case ISCSI_UEVENT_TRANSPORT_EP_CONNECT:
    case ISCSI_UEVENT_TRANSPORT_EP_POLL:
    case ISCSI_UEVENT_TRANSPORT_EP_DISCONNECT:
        err = iscsi_if_transport_ep(transport, ev, nlh->nlmsg_type);
        break;
    case ISCSI_UEVENT_TGT_DSCVR:
        err = iscsi_tgt_dscvr(transport, ev);
        break;
    case ISCSI_UEVENT_SET_HOST_PARAM:
        err = iscsi_set_host_param(transport, ev);
        break;
    default:
        err = -ENOSYS;
        break;
    }

    module_put(transport->owner);
    return err;
}

/*
 * Get message from skb.  Each message is processed by iscsi_if_recv_msg.
 * Malformed skbs with wrong lengths or invalid creds are not processed.
 */
static void
iscsi_if_rx(struct sk_buff *skb)
{
    mutex_lock(&rx_queue_mutex);
    while (skb->len >= NLMSG_SPACE(0)) {
        int err;
        uint32_t rlen;
        struct nlmsghdr    *nlh;
        struct iscsi_uevent *ev;

        nlh = nlmsg_hdr(skb);
        if (nlh->nlmsg_len < sizeof(*nlh) ||
            skb->len < nlh->nlmsg_len) {
            break;
        }

        ev = NLMSG_DATA(nlh);
        rlen = NLMSG_ALIGN(nlh->nlmsg_len);
        if (rlen > skb->len)
            rlen = skb->len;

        err = iscsi_if_recv_msg(skb, nlh);
        if (err) {
            ev->type = ISCSI_KEVENT_IF_ERROR;
            ev->iferror = err;
        }
        do {
            /*
             * special case for GET_STATS:
             * on success - sending reply and stats from
             * inside of if_recv_msg(),
             * on error - fall through.
             */
            if (ev->type == ISCSI_UEVENT_GET_STATS && !err)
                break;
            err = iscsi_if_send_reply(
                NETLINK_CREDS(skb)->pid, nlh->nlmsg_seq,
                nlh->nlmsg_type, 0, 0, ev, sizeof(*ev));
        } while (err < 0 && err != -ECONNREFUSED);
        skb_pull(skb, rlen);
    }
    mutex_unlock(&rx_queue_mutex);
}

#define ISCSI_CLASS_ATTR(_prefix,_name,_mode,_show,_store)        \
struct device_attribute dev_attr_##_prefix##_##_name =    \
    __ATTR(_name,_mode,_show,_store)

/*
 * iSCSI connection attrs
 */
#define iscsi_conn_attr_show(param)                    \
static ssize_t                                \
show_conn_param_##param(struct device *dev,                 \
            struct device_attribute *attr, char *buf)    \
{                                    \
    struct iscsi_cls_conn *conn = iscsi_dev_to_conn(dev->parent);    \
    struct iscsi_transport *t = conn->transport;            \
    return t->get_conn_param(conn, param, buf);            \
}

#define iscsi_conn_attr(field, param)                    \
    iscsi_conn_attr_show(param)                    \
static ISCSI_CLASS_ATTR(conn, field, S_IRUGO, show_conn_param_##param,    \
            NULL);

iscsi_conn_attr(max_recv_dlength, ISCSI_PARAM_MAX_RECV_DLENGTH);
iscsi_conn_attr(max_xmit_dlength, ISCSI_PARAM_MAX_XMIT_DLENGTH);
iscsi_conn_attr(header_digest, ISCSI_PARAM_HDRDGST_EN);
iscsi_conn_attr(data_digest, ISCSI_PARAM_DATADGST_EN);
iscsi_conn_attr(ifmarker, ISCSI_PARAM_IFMARKER_EN);
iscsi_conn_attr(ofmarker, ISCSI_PARAM_OFMARKER_EN);
iscsi_conn_attr(persistent_port, ISCSI_PARAM_PERSISTENT_PORT);
iscsi_conn_attr(port, ISCSI_PARAM_CONN_PORT);
iscsi_conn_attr(exp_statsn, ISCSI_PARAM_EXP_STATSN);
iscsi_conn_attr(persistent_address, ISCSI_PARAM_PERSISTENT_ADDRESS);
iscsi_conn_attr(address, ISCSI_PARAM_CONN_ADDRESS);
iscsi_conn_attr(ping_tmo, ISCSI_PARAM_PING_TMO);
iscsi_conn_attr(recv_tmo, ISCSI_PARAM_RECV_TMO);

/*
 * iSCSI session attrs
 */
#define iscsi_session_attr_show(param, perm)                \
static ssize_t                                \
show_session_param_##param(struct device *dev,                \
               struct device_attribute *attr, char *buf)    \
{                                    \
    struct iscsi_cls_session *session =                 \
        iscsi_dev_to_session(dev->parent);            \
    struct iscsi_transport *t = session->transport;            \
                                    \
    if (perm && !capable(CAP_SYS_ADMIN))                \
        return -EACCES;                        \
    return t->get_session_param(session, param, buf);        \
}

#define iscsi_session_attr(field, param, perm)                \
    iscsi_session_attr_show(param, perm)                \
static ISCSI_CLASS_ATTR(sess, field, S_IRUGO, show_session_param_##param, \
            NULL);

iscsi_session_attr(targetname, ISCSI_PARAM_TARGET_NAME, 0);
iscsi_session_attr(initial_r2t, ISCSI_PARAM_INITIAL_R2T_EN, 0);
iscsi_session_attr(max_outstanding_r2t, ISCSI_PARAM_MAX_R2T, 0);
iscsi_session_attr(immediate_data, ISCSI_PARAM_IMM_DATA_EN, 0);
iscsi_session_attr(first_burst_len, ISCSI_PARAM_FIRST_BURST, 0);
iscsi_session_attr(max_burst_len, ISCSI_PARAM_MAX_BURST, 0);
iscsi_session_attr(data_pdu_in_order, ISCSI_PARAM_PDU_INORDER_EN, 0);
iscsi_session_attr(data_seq_in_order, ISCSI_PARAM_DATASEQ_INORDER_EN, 0);
iscsi_session_attr(erl, ISCSI_PARAM_ERL, 0);
iscsi_session_attr(tpgt, ISCSI_PARAM_TPGT, 0);
iscsi_session_attr(username, ISCSI_PARAM_USERNAME, 1);
iscsi_session_attr(username_in, ISCSI_PARAM_USERNAME_IN, 1);
iscsi_session_attr(password, ISCSI_PARAM_PASSWORD, 1);
iscsi_session_attr(password_in, ISCSI_PARAM_PASSWORD_IN, 1);
iscsi_session_attr(fast_abort, ISCSI_PARAM_FAST_ABORT, 0);
iscsi_session_attr(abort_tmo, ISCSI_PARAM_ABORT_TMO, 0);
iscsi_session_attr(lu_reset_tmo, ISCSI_PARAM_LU_RESET_TMO, 0);

static ssize_t
show_priv_session_state(struct device *dev, struct device_attribute *attr,
            char *buf)
{
    struct iscsi_cls_session *session = iscsi_dev_to_session(dev->parent);
    return sprintf(buf, "%s\n", iscsi_session_state_name(session->state));
}
static ISCSI_CLASS_ATTR(priv_sess, state, S_IRUGO, show_priv_session_state,
            NULL);

#define iscsi_priv_session_attr_show(field, format)            \
static ssize_t                                \
show_priv_session_##field(struct device *dev,                 \
              struct device_attribute *attr, char *buf)    \
{                                    \
    struct iscsi_cls_session *session =                 \
            iscsi_dev_to_session(dev->parent);        \
    return sprintf(buf, format"\n", session->field);        \
}

#define iscsi_priv_session_attr(field, format)                \
    iscsi_priv_session_attr_show(field, format)            \
static ISCSI_CLASS_ATTR(priv_sess, field, S_IRUGO, show_priv_session_##field, \
            NULL)
iscsi_priv_session_attr(recovery_tmo, "%d");

/*
 * iSCSI host attrs
 */
#define iscsi_host_attr_show(param)                    \
static ssize_t                                \
show_host_param_##param(struct device *dev,                 \
            struct device_attribute *attr, char *buf)    \
{                                    \
    struct Scsi_Host *shost = transport_class_to_shost(dev);    \
    struct iscsi_internal *priv = to_iscsi_internal(shost->transportt); \
    return priv->iscsi_transport->get_host_param(shost, param, buf); \
}

#define iscsi_host_attr(field, param)                    \
    iscsi_host_attr_show(param)                    \
static ISCSI_CLASS_ATTR(host, field, S_IRUGO, show_host_param_##param,    \
            NULL);

iscsi_host_attr(netdev, ISCSI_HOST_PARAM_NETDEV_NAME);
iscsi_host_attr(hwaddress, ISCSI_HOST_PARAM_HWADDRESS);
iscsi_host_attr(ipaddress, ISCSI_HOST_PARAM_IPADDRESS);
iscsi_host_attr(initiatorname, ISCSI_HOST_PARAM_INITIATOR_NAME);

#define SETUP_PRIV_SESSION_RD_ATTR(field)                \
do {                                    \
    priv->session_attrs[count] = &dev_attr_priv_sess_##field; \
    count++;                            \
} while (0)


#define SETUP_SESSION_RD_ATTR(field, param_flag)            \
do {                                    \
    if (tt->param_mask & param_flag) {                \
        priv->session_attrs[count] = &dev_attr_sess_##field; \
        count++;                        \
    }                                \
} while (0)

#define SETUP_CONN_RD_ATTR(field, param_flag)                \
do {                                    \
    if (tt->param_mask & param_flag) {                \
        priv->conn_attrs[count] = &dev_attr_conn_##field; \
        count++;                        \
    }                                \
} while (0)

#define SETUP_HOST_RD_ATTR(field, param_flag)                \
do {                                    \
    if (tt->host_param_mask & param_flag) {                \
        priv->host_attrs[count] = &dev_attr_host_##field; \
        count++;                        \
    }                                \
} while (0)

static int iscsi_session_match(struct attribute_container *cont,
               struct device *dev)
{
    struct iscsi_cls_session *session;
    struct Scsi_Host *shost;
    struct iscsi_internal *priv;

    if (!iscsi_is_session_dev(dev))
        return 0;

    session = iscsi_dev_to_session(dev);
    shost = iscsi_session_to_shost(session);
    if (!shost->transportt)
        return 0;

    priv = to_iscsi_internal(shost->transportt);
    if (priv->session_cont.ac.class != &iscsi_session_class.class)
        return 0;

    return &priv->session_cont.ac == cont;
}

static int iscsi_conn_match(struct attribute_container *cont,
               struct device *dev)
{
    struct iscsi_cls_session *session;
    struct iscsi_cls_conn *conn;
    struct Scsi_Host *shost;
    struct iscsi_internal *priv;

    if (!iscsi_is_conn_dev(dev))
        return 0;

    conn = iscsi_dev_to_conn(dev);
    session = iscsi_dev_to_session(conn->dev.parent);
    shost = iscsi_session_to_shost(session);

    if (!shost->transportt)
        return 0;

    priv = to_iscsi_internal(shost->transportt);
    if (priv->conn_cont.ac.class != &iscsi_connection_class.class)
        return 0;

    return &priv->conn_cont.ac == cont;
}

static int iscsi_host_match(struct attribute_container *cont,
                struct device *dev)
{
    struct Scsi_Host *shost;
    struct iscsi_internal *priv;

    if (!scsi_is_host_device(dev))
        return 0;

    shost = dev_to_shost(dev);
    if (!shost->transportt  ||
        shost->transportt->host_attrs.ac.class != &iscsi_host_class.class)
        return 0;

        priv = to_iscsi_internal(shost->transportt);
        return &priv->t.host_attrs.ac == cont;
}

struct scsi_transport_template *
iscsi_register_transport(struct iscsi_transport *tt)
{
    struct iscsi_internal *priv;
    unsigned long flags;
    int count = 0, err;

    BUG_ON(!tt);

    priv = iscsi_if_transport_lookup(tt);
    if (priv)
        return NULL;

    priv = kzalloc(sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return NULL;
    INIT_LIST_HEAD(&priv->list);
    priv->daemon_pid = -1;
    priv->iscsi_transport = tt;
    priv->t.user_scan = iscsi_user_scan;

    priv->dev.class = &iscsi_transport_class;
    snprintf(priv->dev.bus_id, BUS_ID_SIZE, "%s", tt->name);
    err = device_register(&priv->dev);
    if (err)
        goto free_priv;

    err = sysfs_create_group(&priv->dev.kobj, &iscsi_transport_group);
    if (err)
        goto unregister_dev;

    /* host parameters */
    priv->t.host_attrs.ac.attrs = &priv->host_attrs[0];
    priv->t.host_attrs.ac.class = &iscsi_host_class.class;
    priv->t.host_attrs.ac.match = iscsi_host_match;
    priv->t.host_size = sizeof(struct iscsi_host);
    transport_container_register(&priv->t.host_attrs);

    SETUP_HOST_RD_ATTR(netdev, ISCSI_HOST_NETDEV_NAME);
    SETUP_HOST_RD_ATTR(ipaddress, ISCSI_HOST_IPADDRESS);
    SETUP_HOST_RD_ATTR(hwaddress, ISCSI_HOST_HWADDRESS);
    SETUP_HOST_RD_ATTR(initiatorname, ISCSI_HOST_INITIATOR_NAME);
    BUG_ON(count > ISCSI_HOST_ATTRS);
    priv->host_attrs[count] = NULL;
    count = 0;

    /* connection parameters */
    priv->conn_cont.ac.attrs = &priv->conn_attrs[0];
    priv->conn_cont.ac.class = &iscsi_connection_class.class;
    priv->conn_cont.ac.match = iscsi_conn_match;
    transport_container_register(&priv->conn_cont);

    SETUP_CONN_RD_ATTR(max_recv_dlength, ISCSI_MAX_RECV_DLENGTH);
    SETUP_CONN_RD_ATTR(max_xmit_dlength, ISCSI_MAX_XMIT_DLENGTH);
    SETUP_CONN_RD_ATTR(header_digest, ISCSI_HDRDGST_EN);
    SETUP_CONN_RD_ATTR(data_digest, ISCSI_DATADGST_EN);
    SETUP_CONN_RD_ATTR(ifmarker, ISCSI_IFMARKER_EN);
    SETUP_CONN_RD_ATTR(ofmarker, ISCSI_OFMARKER_EN);
    SETUP_CONN_RD_ATTR(address, ISCSI_CONN_ADDRESS);
    SETUP_CONN_RD_ATTR(port, ISCSI_CONN_PORT);
    SETUP_CONN_RD_ATTR(exp_statsn, ISCSI_EXP_STATSN);
    SETUP_CONN_RD_ATTR(persistent_address, ISCSI_PERSISTENT_ADDRESS);
    SETUP_CONN_RD_ATTR(persistent_port, ISCSI_PERSISTENT_PORT);
    SETUP_CONN_RD_ATTR(ping_tmo, ISCSI_PING_TMO);
    SETUP_CONN_RD_ATTR(recv_tmo, ISCSI_RECV_TMO);

    BUG_ON(count > ISCSI_CONN_ATTRS);
    priv->conn_attrs[count] = NULL;
    count = 0;

    /* session parameters */
    priv->session_cont.ac.attrs = &priv->session_attrs[0];
    priv->session_cont.ac.class = &iscsi_session_class.class;
    priv->session_cont.ac.match = iscsi_session_match;
    transport_container_register(&priv->session_cont);

    SETUP_SESSION_RD_ATTR(initial_r2t, ISCSI_INITIAL_R2T_EN);
    SETUP_SESSION_RD_ATTR(max_outstanding_r2t, ISCSI_MAX_R2T);
    SETUP_SESSION_RD_ATTR(immediate_data, ISCSI_IMM_DATA_EN);
    SETUP_SESSION_RD_ATTR(first_burst_len, ISCSI_FIRST_BURST);
    SETUP_SESSION_RD_ATTR(max_burst_len, ISCSI_MAX_BURST);
    SETUP_SESSION_RD_ATTR(data_pdu_in_order, ISCSI_PDU_INORDER_EN);
    SETUP_SESSION_RD_ATTR(data_seq_in_order, ISCSI_DATASEQ_INORDER_EN);
    SETUP_SESSION_RD_ATTR(erl, ISCSI_ERL);
    SETUP_SESSION_RD_ATTR(targetname, ISCSI_TARGET_NAME);
    SETUP_SESSION_RD_ATTR(tpgt, ISCSI_TPGT);
    SETUP_SESSION_RD_ATTR(password, ISCSI_USERNAME);
    SETUP_SESSION_RD_ATTR(password_in, ISCSI_USERNAME_IN);
    SETUP_SESSION_RD_ATTR(username, ISCSI_PASSWORD);
    SETUP_SESSION_RD_ATTR(username_in, ISCSI_PASSWORD_IN);
    SETUP_SESSION_RD_ATTR(fast_abort, ISCSI_FAST_ABORT);
    SETUP_SESSION_RD_ATTR(abort_tmo, ISCSI_ABORT_TMO);
    SETUP_SESSION_RD_ATTR(lu_reset_tmo,ISCSI_LU_RESET_TMO);
    SETUP_PRIV_SESSION_RD_ATTR(recovery_tmo);
    SETUP_PRIV_SESSION_RD_ATTR(state);

    BUG_ON(count > ISCSI_SESSION_ATTRS);
    priv->session_attrs[count] = NULL;

    spin_lock_irqsave(&iscsi_transport_lock, flags);
    list_add(&priv->list, &iscsi_transports);
    spin_unlock_irqrestore(&iscsi_transport_lock, flags);

    printk(KERN_NOTICE "iscsi: registered transport (%s)\n", tt->name);
    return &priv->t;

unregister_dev:
    device_unregister(&priv->dev);
free_priv:
    kfree(priv);
    return NULL;
}
EXPORT_SYMBOL_GPL(iscsi_register_transport);

int iscsi_unregister_transport(struct iscsi_transport *tt)
{
    struct iscsi_internal *priv;
    unsigned long flags;

    BUG_ON(!tt);

    mutex_lock(&rx_queue_mutex);

    priv = iscsi_if_transport_lookup(tt);
    BUG_ON (!priv);

    spin_lock_irqsave(&iscsi_transport_lock, flags);
    list_del(&priv->list);
    spin_unlock_irqrestore(&iscsi_transport_lock, flags);

    transport_container_unregister(&priv->conn_cont);
    transport_container_unregister(&priv->session_cont);
    transport_container_unregister(&priv->t.host_attrs);

    sysfs_remove_group(&priv->dev.kobj, &iscsi_transport_group);
    device_unregister(&priv->dev);
    mutex_unlock(&rx_queue_mutex);

    return 0;
}
EXPORT_SYMBOL_GPL(iscsi_unregister_transport);

static __init int iscsi_transport_init(void)
{
    int err;

    printk(KERN_INFO "Loading iSCSI transport class v%s.\n",
        ISCSI_TRANSPORT_VERSION);

    atomic_set(&iscsi_session_nr, 0);

    err = class_register(&iscsi_transport_class);
    if (err)
        return err;

    err = transport_class_register(&iscsi_host_class);
    if (err)
        goto unregister_transport_class;

    err = transport_class_register(&iscsi_connection_class);
    if (err)
        goto unregister_host_class;

    err = transport_class_register(&iscsi_session_class);
    if (err)
        goto unregister_conn_class;

    nls = netlink_kernel_create(&init_net, NETLINK_ISCSI, 1, iscsi_if_rx, NULL,
            THIS_MODULE);
    if (!nls) {
        err = -ENOBUFS;
        goto unregister_session_class;
    }

    iscsi_eh_timer_workq = create_singlethread_workqueue("iscsi_eh");
    if (!iscsi_eh_timer_workq)
        goto release_nls;

    return 0;

release_nls:
    netlink_kernel_release(nls);
unregister_session_class:
    transport_class_unregister(&iscsi_session_class);
unregister_conn_class:
    transport_class_unregister(&iscsi_connection_class);
unregister_host_class:
    transport_class_unregister(&iscsi_host_class);
unregister_transport_class:
    class_unregister(&iscsi_transport_class);
    return err;
}

static void __exit iscsi_transport_exit(void)
{
    destroy_workqueue(iscsi_eh_timer_workq);
    netlink_kernel_release(nls);
    transport_class_unregister(&iscsi_connection_class);
    transport_class_unregister(&iscsi_session_class);
    transport_class_unregister(&iscsi_host_class);
    class_unregister(&iscsi_transport_class);
}

module_init(iscsi_transport_init);
module_exit(iscsi_transport_exit);

MODULE_AUTHOR("Mike Christie <michaelc@cs.wisc.edu>, "
          "Dmitry Yusupov <dmitry_yus@yahoo.com>, "
          "Alex Aizman <itn780@yahoo.com>");
MODULE_DESCRIPTION("iSCSI Transport Interface");
MODULE_LICENSE("GPL");
MODULE_VERSION(ISCSI_TRANSPORT_VERSION);
