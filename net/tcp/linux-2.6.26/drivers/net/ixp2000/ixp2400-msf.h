/*
 * Generic library functions for the MSF (Media and Switch Fabric) unit
 * found on the Intel IXP2400 network processor.
 *
 * Copyright (C) 2004, 2005 Lennert Buytenhek <buytenh@wantstofly.org>
 * Dedicated to Marija Kulikova.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of the
 * License, or (at your option) any later version.
 */

#ifndef __IXP2400_MSF_H
#define __IXP2400_MSF_H

struct ixp2400_msf_parameters
{
    u32                rx_mode;
    unsigned            rxclk01_multiplier:2;
    unsigned            rxclk23_multiplier:2;
    unsigned            rx_poll_ports:6;
    u32                rx_channel_mode[4];

    u32                tx_mode;
    unsigned            txclk01_multiplier:2;
    unsigned            txclk23_multiplier:2;
    unsigned            tx_poll_ports:6;
    u32                tx_channel_mode[4];
};

void ixp2400_msf_init(struct ixp2400_msf_parameters *mp);

#define IXP2400_PLL_MULTIPLIER_48        0x00
#define IXP2400_PLL_MULTIPLIER_24        0x01
#define IXP2400_PLL_MULTIPLIER_16        0x02
#define IXP2400_PLL_MULTIPLIER_12        0x03

#define IXP2400_RX_MODE_CSIX            0x00400000
#define IXP2400_RX_MODE_UTOPIA_POS        0x00000000
#define IXP2400_RX_MODE_WIDTH_MASK        0x00300000
#define IXP2400_RX_MODE_1x16_2x8        0x00300000
#define IXP2400_RX_MODE_4x8            0x00200000
#define IXP2400_RX_MODE_2x16            0x00100000
#define IXP2400_RX_MODE_1x32            0x00000000
#define IXP2400_RX_MODE_MPHY            0x00080000
#define IXP2400_RX_MODE_SPHY            0x00000000
#define IXP2400_RX_MODE_MPHY_32            0x00040000
#define IXP2400_RX_MODE_MPHY_4            0x00000000
#define IXP2400_RX_MODE_MPHY_POLLED_STATUS    0x00020000
#define IXP2400_RX_MODE_MPHY_DIRECT_STATUS    0x00000000
#define IXP2400_RX_MODE_CBUS_FULL_DUPLEX    0x00010000
#define IXP2400_RX_MODE_CBUS_SIMPLEX        0x00000000
#define IXP2400_RX_MODE_MPHY_LEVEL2        0x00004000
#define IXP2400_RX_MODE_MPHY_LEVEL3        0x00000000
#define IXP2400_RX_MODE_CBUS_8BIT        0x00002000
#define IXP2400_RX_MODE_CBUS_4BIT        0x00000000
#define IXP2400_RX_MODE_CSIX_SINGLE_FREELIST    0x00000200
#define IXP2400_RX_MODE_CSIX_SPLIT_FREELISTS    0x00000000
#define IXP2400_RX_MODE_RBUF_SIZE_MASK        0x0000000c
#define IXP2400_RX_MODE_RBUF_SIZE_256        0x00000008
#define IXP2400_RX_MODE_RBUF_SIZE_128        0x00000004
#define IXP2400_RX_MODE_RBUF_SIZE_64        0x00000000

#define IXP2400_PORT_RX_MODE_SLAVE        0x00000040
#define IXP2400_PORT_RX_MODE_MASTER        0x00000000
#define IXP2400_PORT_RX_MODE_POS_PHY_L3        0x00000020
#define IXP2400_PORT_RX_MODE_POS_PHY_L2        0x00000000
#define IXP2400_PORT_RX_MODE_POS_PHY        0x00000010
#define IXP2400_PORT_RX_MODE_UTOPIA        0x00000000
#define IXP2400_PORT_RX_MODE_EVEN_PARITY    0x0000000c
#define IXP2400_PORT_RX_MODE_ODD_PARITY        0x00000008
#define IXP2400_PORT_RX_MODE_NO_PARITY        0x00000000
#define IXP2400_PORT_RX_MODE_UTOPIA_BIG_CELLS    0x00000002
#define IXP2400_PORT_RX_MODE_UTOPIA_NORMAL_CELLS    0x00000000
#define IXP2400_PORT_RX_MODE_2_CYCLE_DECODE    0x00000001
#define IXP2400_PORT_RX_MODE_1_CYCLE_DECODE    0x00000000

#define IXP2400_TX_MODE_CSIX            0x00400000
#define IXP2400_TX_MODE_UTOPIA_POS        0x00000000
#define IXP2400_TX_MODE_WIDTH_MASK        0x00300000
#define IXP2400_TX_MODE_1x16_2x8        0x00300000
#define IXP2400_TX_MODE_4x8            0x00200000
#define IXP2400_TX_MODE_2x16            0x00100000
#define IXP2400_TX_MODE_1x32            0x00000000
#define IXP2400_TX_MODE_MPHY            0x00080000
#define IXP2400_TX_MODE_SPHY            0x00000000
#define IXP2400_TX_MODE_MPHY_32            0x00040000
#define IXP2400_TX_MODE_MPHY_4            0x00000000
#define IXP2400_TX_MODE_MPHY_POLLED_STATUS    0x00020000
#define IXP2400_TX_MODE_MPHY_DIRECT_STATUS    0x00000000
#define IXP2400_TX_MODE_CBUS_FULL_DUPLEX    0x00010000
#define IXP2400_TX_MODE_CBUS_SIMPLEX        0x00000000
#define IXP2400_TX_MODE_MPHY_LEVEL2        0x00004000
#define IXP2400_TX_MODE_MPHY_LEVEL3        0x00000000
#define IXP2400_TX_MODE_CBUS_8BIT        0x00002000
#define IXP2400_TX_MODE_CBUS_4BIT        0x00000000
#define IXP2400_TX_MODE_TBUF_SIZE_MASK        0x0000000c
#define IXP2400_TX_MODE_TBUF_SIZE_256        0x00000008
#define IXP2400_TX_MODE_TBUF_SIZE_128        0x00000004
#define IXP2400_TX_MODE_TBUF_SIZE_64        0x00000000

#define IXP2400_PORT_TX_MODE_SLAVE        0x00000040
#define IXP2400_PORT_TX_MODE_MASTER        0x00000000
#define IXP2400_PORT_TX_MODE_POS_PHY        0x00000010
#define IXP2400_PORT_TX_MODE_UTOPIA        0x00000000
#define IXP2400_PORT_TX_MODE_EVEN_PARITY    0x0000000c
#define IXP2400_PORT_TX_MODE_ODD_PARITY        0x00000008
#define IXP2400_PORT_TX_MODE_NO_PARITY        0x00000000
#define IXP2400_PORT_TX_MODE_UTOPIA_BIG_CELLS    0x00000002
#define IXP2400_PORT_TX_MODE_2_CYCLE_DECODE    0x00000001
#define IXP2400_PORT_TX_MODE_1_CYCLE_DECODE    0x00000000


#endif
