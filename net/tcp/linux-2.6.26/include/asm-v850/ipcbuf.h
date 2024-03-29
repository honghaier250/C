#ifndef __V850E_IPCBUF_H__
#define __V850E_IPCBUF_H__

/*
 * The user_ipc_perm structure for v850e architecture.
 * Note extra padding because this structure is passed back and forth
 * between kernel and user space.
 *
 * Pad space is left for:
 * - 32-bit mode_t and seq
 * - 2 miscellaneous 32-bit values
 */

struct ipc64_perm
{
    __kernel_key_t        key;
    __kernel_uid32_t    uid;
    __kernel_gid32_t    gid;
    __kernel_uid32_t    cuid;
    __kernel_gid32_t    cgid;
    __kernel_mode_t        mode;
    unsigned short        __pad1;
    unsigned short        seq;
    unsigned short        __pad2;
    unsigned long        __unused1;
    unsigned long        __unused2;
};

#endif /* __V850E_IPCBUF_H__ */
