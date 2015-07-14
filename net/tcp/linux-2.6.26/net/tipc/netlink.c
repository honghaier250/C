/*
 * net/tipc/netlink.c: TIPC configuration handling
 *
 * Copyright (c) 2005-2006, Ericsson AB
 * Copyright (c) 2005, Wind River Systems
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the names of the copyright holders nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "core.h"
#include "config.h"
#include <net/genetlink.h>

static int handle_cmd(struct sk_buff *skb, struct genl_info *info)
{
    struct sk_buff *rep_buf;
    struct nlmsghdr *rep_nlh;
    struct nlmsghdr *req_nlh = info->nlhdr;
    struct tipc_genlmsghdr *req_userhdr = info->userhdr;
    int hdr_space = NLMSG_SPACE(GENL_HDRLEN + TIPC_GENL_HDRLEN);

    if ((req_userhdr->cmd & 0xC000) && (!capable(CAP_NET_ADMIN)))
        rep_buf = tipc_cfg_reply_error_string(TIPC_CFG_NOT_NET_ADMIN);
    else
        rep_buf = tipc_cfg_do_cmd(req_userhdr->dest,
                      req_userhdr->cmd,
                      NLMSG_DATA(req_nlh) + GENL_HDRLEN + TIPC_GENL_HDRLEN,
                      NLMSG_PAYLOAD(req_nlh, GENL_HDRLEN + TIPC_GENL_HDRLEN),
                      hdr_space);

    if (rep_buf) {
        skb_push(rep_buf, hdr_space);
        rep_nlh = nlmsg_hdr(rep_buf);
        memcpy(rep_nlh, req_nlh, hdr_space);
        rep_nlh->nlmsg_len = rep_buf->len;
        genlmsg_unicast(rep_buf, NETLINK_CB(skb).pid);
    }

    return 0;
}

static struct genl_family family = {
    .id        = GENL_ID_GENERATE,
    .name        = TIPC_GENL_NAME,
    .version    = TIPC_GENL_VERSION,
    .hdrsize    = TIPC_GENL_HDRLEN,
    .maxattr    = 0,
};

static struct genl_ops ops = {
    .cmd        = TIPC_GENL_CMD,
    .doit        = handle_cmd,
};

static int family_registered = 0;

int tipc_netlink_start(void)
{


    if (genl_register_family(&family))
        goto err;

    family_registered = 1;

    if (genl_register_ops(&family, &ops))
        goto err_unregister;

    return 0;

 err_unregister:
    genl_unregister_family(&family);
    family_registered = 0;
 err:
    err("Failed to register netlink interface\n");
    return -EFAULT;
}

void tipc_netlink_stop(void)
{
    if (family_registered) {
        genl_unregister_family(&family);
        family_registered = 0;
    }
}
