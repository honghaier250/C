#ifndef __NET_WIRELESS_H
#define __NET_WIRELESS_H

/*
 * 802.11 device management
 *
 * Copyright 2007    Johannes Berg <johannes@sipsolutions.net>
 */

#include <linux/netdevice.h>
#include <linux/debugfs.h>
#include <linux/list.h>
#include <net/cfg80211.h>

/**
 * enum ieee80211_band - supported frequency bands
 *
 * The bands are assigned this way because the supported
 * bitrates differ in these bands.
 *
 * @IEEE80211_BAND_2GHZ: 2.4GHz ISM band
 * @IEEE80211_BAND_5GHZ: around 5GHz band (4.9-5.7)
 */
enum ieee80211_band {
    IEEE80211_BAND_2GHZ,
    IEEE80211_BAND_5GHZ,

    /* keep last */
    IEEE80211_NUM_BANDS
};

/**
 * enum ieee80211_channel_flags - channel flags
 *
 * Channel flags set by the regulatory control code.
 *
 * @IEEE80211_CHAN_DISABLED: This channel is disabled.
 * @IEEE80211_CHAN_PASSIVE_SCAN: Only passive scanning is permitted
 *    on this channel.
 * @IEEE80211_CHAN_NO_IBSS: IBSS is not allowed on this channel.
 * @IEEE80211_CHAN_RADAR: Radar detection is required on this channel.
 */
enum ieee80211_channel_flags {
    IEEE80211_CHAN_DISABLED        = 1<<0,
    IEEE80211_CHAN_PASSIVE_SCAN    = 1<<1,
    IEEE80211_CHAN_NO_IBSS        = 1<<2,
    IEEE80211_CHAN_RADAR        = 1<<3,
};

/**
 * struct ieee80211_channel - channel definition
 *
 * This structure describes a single channel for use
 * with cfg80211.
 *
 * @center_freq: center frequency in MHz
 * @hw_value: hardware-specific value for the channel
 * @flags: channel flags from &enum ieee80211_channel_flags.
 * @orig_flags: channel flags at registration time, used by regulatory
 *    code to support devices with additional restrictions
 * @band: band this channel belongs to.
 * @max_antenna_gain: maximum antenna gain in dBi
 * @max_power: maximum transmission power (in dBm)
 * @orig_mag: internal use
 * @orig_mpwr: internal use
 */
struct ieee80211_channel {
    enum ieee80211_band band;
    u16 center_freq;
    u16 hw_value;
    u32 flags;
    int max_antenna_gain;
    int max_power;
    u32 orig_flags;
    int orig_mag, orig_mpwr;
};

/**
 * enum ieee80211_rate_flags - rate flags
 *
 * Hardware/specification flags for rates. These are structured
 * in a way that allows using the same bitrate structure for
 * different bands/PHY modes.
 *
 * @IEEE80211_RATE_SHORT_PREAMBLE: Hardware can send with short
 *    preamble on this bitrate; only relevant in 2.4GHz band and
 *    with CCK rates.
 * @IEEE80211_RATE_MANDATORY_A: This bitrate is a mandatory rate
 *    when used with 802.11a (on the 5 GHz band); filled by the
 *    core code when registering the wiphy.
 * @IEEE80211_RATE_MANDATORY_B: This bitrate is a mandatory rate
 *    when used with 802.11b (on the 2.4 GHz band); filled by the
 *    core code when registering the wiphy.
 * @IEEE80211_RATE_MANDATORY_G: This bitrate is a mandatory rate
 *    when used with 802.11g (on the 2.4 GHz band); filled by the
 *    core code when registering the wiphy.
 * @IEEE80211_RATE_ERP_G: This is an ERP rate in 802.11g mode.
 */
enum ieee80211_rate_flags {
    IEEE80211_RATE_SHORT_PREAMBLE    = 1<<0,
    IEEE80211_RATE_MANDATORY_A    = 1<<1,
    IEEE80211_RATE_MANDATORY_B    = 1<<2,
    IEEE80211_RATE_MANDATORY_G    = 1<<3,
    IEEE80211_RATE_ERP_G        = 1<<4,
};

/**
 * struct ieee80211_rate - bitrate definition
 *
 * This structure describes a bitrate that an 802.11 PHY can
 * operate with. The two values @hw_value and @hw_value_short
 * are only for driver use when pointers to this structure are
 * passed around.
 *
 * @flags: rate-specific flags
 * @bitrate: bitrate in units of 100 Kbps
 * @hw_value: driver/hardware value for this rate
 * @hw_value_short: driver/hardware value for this rate when
 *    short preamble is used
 */
struct ieee80211_rate {
    u32 flags;
    u16 bitrate;
    u16 hw_value, hw_value_short;
};

/**
 * struct ieee80211_ht_info - describing STA's HT capabilities
 *
 * This structure describes most essential parameters needed
 * to describe 802.11n HT capabilities for an STA.
 *
 * @ht_supported: is HT supported by STA, 0: no, 1: yes
 * @cap: HT capabilities map as described in 802.11n spec
 * @ampdu_factor: Maximum A-MPDU length factor
 * @ampdu_density: Minimum A-MPDU spacing
 * @supp_mcs_set: Supported MCS set as described in 802.11n spec
 */
struct ieee80211_ht_info {
    u16 cap; /* use IEEE80211_HT_CAP_ */
    u8 ht_supported;
    u8 ampdu_factor;
    u8 ampdu_density;
    u8 supp_mcs_set[16];
};

/**
 * struct ieee80211_supported_band - frequency band definition
 *
 * This structure describes a frequency band a wiphy
 * is able to operate in.
 *
 * @channels: Array of channels the hardware can operate in
 *    in this band.
 * @band: the band this structure represents
 * @n_channels: Number of channels in @channels
 * @bitrates: Array of bitrates the hardware can operate with
 *    in this band. Must be sorted to give a valid "supported
 *    rates" IE, i.e. CCK rates first, then OFDM.
 * @n_bitrates: Number of bitrates in @bitrates
 */
struct ieee80211_supported_band {
    struct ieee80211_channel *channels;
    struct ieee80211_rate *bitrates;
    enum ieee80211_band band;
    int n_channels;
    int n_bitrates;
    struct ieee80211_ht_info ht_info;
};

/**
 * struct wiphy - wireless hardware description
 * @idx: the wiphy index assigned to this item
 * @class_dev: the class device representing /sys/class/ieee80211/<wiphy-name>
 */
struct wiphy {
    /* assign these fields before you register the wiphy */

    /* permanent MAC address */
    u8 perm_addr[ETH_ALEN];

    /* If multiple wiphys are registered and you're handed e.g.
     * a regular netdev with assigned ieee80211_ptr, you won't
     * know whether it points to a wiphy your driver has registered
     * or not. Assign this to something global to your driver to
     * help determine whether you own this wiphy or not. */
    void *privid;

    struct ieee80211_supported_band *bands[IEEE80211_NUM_BANDS];

    /* fields below are read-only, assigned by cfg80211 */

    /* the item in /sys/class/ieee80211/ points to this,
     * you need use set_wiphy_dev() (see below) */
    struct device dev;

    /* dir in debugfs: ieee80211/<wiphyname> */
    struct dentry *debugfsdir;

    char priv[0] __attribute__((__aligned__(NETDEV_ALIGN)));
};

/** struct wireless_dev - wireless per-netdev state
 *
 * This structure must be allocated by the driver/stack
 * that uses the ieee80211_ptr field in struct net_device
 * (this is intentional so it can be allocated along with
 * the netdev.)
 *
 * @wiphy: pointer to hardware description
 */
struct wireless_dev {
    struct wiphy *wiphy;

    /* private to the generic wireless code */
    struct list_head list;
    struct net_device *netdev;
};

/**
 * wiphy_priv - return priv from wiphy
 */
static inline void *wiphy_priv(struct wiphy *wiphy)
{
    BUG_ON(!wiphy);
    return &wiphy->priv;
}

/**
 * set_wiphy_dev - set device pointer for wiphy
 */
static inline void set_wiphy_dev(struct wiphy *wiphy, struct device *dev)
{
    wiphy->dev.parent = dev;
}

/**
 * wiphy_dev - get wiphy dev pointer
 */
static inline struct device *wiphy_dev(struct wiphy *wiphy)
{
    return wiphy->dev.parent;
}

/**
 * wiphy_name - get wiphy name
 */
static inline char *wiphy_name(struct wiphy *wiphy)
{
    return wiphy->dev.bus_id;
}

/**
 * wdev_priv - return wiphy priv from wireless_dev
 */
static inline void *wdev_priv(struct wireless_dev *wdev)
{
    BUG_ON(!wdev);
    return wiphy_priv(wdev->wiphy);
}

/**
 * wiphy_new - create a new wiphy for use with cfg80211
 *
 * create a new wiphy and associate the given operations with it.
 * @sizeof_priv bytes are allocated for private use.
 *
 * the returned pointer must be assigned to each netdev's
 * ieee80211_ptr for proper operation.
 */
struct wiphy *wiphy_new(struct cfg80211_ops *ops, int sizeof_priv);

/**
 * wiphy_register - register a wiphy with cfg80211
 *
 * register the given wiphy
 *
 * Returns a non-negative wiphy index or a negative error code.
 */
extern int wiphy_register(struct wiphy *wiphy);

/**
 * wiphy_unregister - deregister a wiphy from cfg80211
 *
 * unregister a device with the given priv pointer.
 * After this call, no more requests can be made with this priv
 * pointer, but the call may sleep to wait for an outstanding
 * request that is being handled.
 */
extern void wiphy_unregister(struct wiphy *wiphy);

/**
 * wiphy_free - free wiphy
 */
extern void wiphy_free(struct wiphy *wiphy);

/**
 * ieee80211_channel_to_frequency - convert channel number to frequency
 */
extern int ieee80211_channel_to_frequency(int chan);

/**
 * ieee80211_frequency_to_channel - convert frequency to channel number
 */
extern int ieee80211_frequency_to_channel(int freq);

/*
 * Name indirection necessary because the ieee80211 code also has
 * a function named "ieee80211_get_channel", so if you include
 * cfg80211's header file you get cfg80211's version, if you try
 * to include both header files you'll (rightfully!) get a symbol
 * clash.
 */
extern struct ieee80211_channel *__ieee80211_get_channel(struct wiphy *wiphy,
                             int freq);

/**
 * ieee80211_get_channel - get channel struct from wiphy for specified frequency
 */
static inline struct ieee80211_channel *
ieee80211_get_channel(struct wiphy *wiphy, int freq)
{
    return __ieee80211_get_channel(wiphy, freq);
}
#endif /* __NET_WIRELESS_H */
