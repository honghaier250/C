/*
 *    Video for Linux Two
 *
 *    A generic video device interface for the LINUX operating system
 *    using a set of device structures/vectors for low level operations.
 *
 *    This file replaces the videodev.c file that comes with the
 *    regular kernel distribution.
 *
 *    This program is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU General Public License
 *    as published by the Free Software Foundation; either version
 *    2 of the License, or (at your option) any later version.
 *
 * Author:    Bill Dirks <bill@thedirks.org>
 *        based on code by Alan Cox, <alan@cymru.net>
 *
 */

/*
 * Video capture interface for Linux
 *
 *    A generic video device interface for the LINUX operating system
 *    using a set of device structures/vectors for low level operations.
 *
 *        This program is free software; you can redistribute it and/or
 *        modify it under the terms of the GNU General Public License
 *        as published by the Free Software Foundation; either version
 *        2 of the License, or (at your option) any later version.
 *
 * Author:    Alan Cox, <alan@redhat.com>
 *
 * Fixes:
 */

/*
 * Video4linux 1/2 integration by Justin Schoeman
 * <justin@suntiger.ee.up.ac.za>
 * 2.4 PROCFS support ported from 2.4 kernels by
 *  Iñaki García Etxebarria <garetxe@euskalnet.net>
 * Makefile fix by "W. Michael Petullo" <mike@flyn.org>
 * 2.4 devfs support ported from 2.4 kernels by
 *  Dan Merillat <dan@merillat.org>
 * Added Gerd Knorrs v4l1 enhancements (Justin Schoeman)
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/div64.h>
#define __OLD_VIDIOC_ /* To allow fixing old calls*/
#include <media/v4l2-common.h>
#include <media/v4l2-chip-ident.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

#include <linux/videodev.h>

MODULE_AUTHOR("Bill Dirks, Justin Schoeman, Gerd Knorr");
MODULE_DESCRIPTION("misc helper functions for v4l2 device drivers");
MODULE_LICENSE("GPL");

/*
 *
 *    V 4 L 2   D R I V E R   H E L P E R   A P I
 *
 */

/*
 *  Video Standard Operations (contributed by Michael Schimek)
 */


/* ----------------------------------------------------------------- */
/* priority handling                                                 */

#define V4L2_PRIO_VALID(val) (val == V4L2_PRIORITY_BACKGROUND   || \
                  val == V4L2_PRIORITY_INTERACTIVE  || \
                  val == V4L2_PRIORITY_RECORD)

int v4l2_prio_init(struct v4l2_prio_state *global)
{
    memset(global,0,sizeof(*global));
    return 0;
}
EXPORT_SYMBOL(v4l2_prio_init);

int v4l2_prio_change(struct v4l2_prio_state *global, enum v4l2_priority *local,
             enum v4l2_priority new)
{
    if (!V4L2_PRIO_VALID(new))
        return -EINVAL;
    if (*local == new)
        return 0;

    atomic_inc(&global->prios[new]);
    if (V4L2_PRIO_VALID(*local))
        atomic_dec(&global->prios[*local]);
    *local = new;
    return 0;
}
EXPORT_SYMBOL(v4l2_prio_change);

int v4l2_prio_open(struct v4l2_prio_state *global, enum v4l2_priority *local)
{
    return v4l2_prio_change(global,local,V4L2_PRIORITY_DEFAULT);
}
EXPORT_SYMBOL(v4l2_prio_open);

int v4l2_prio_close(struct v4l2_prio_state *global, enum v4l2_priority *local)
{
    if (V4L2_PRIO_VALID(*local))
        atomic_dec(&global->prios[*local]);
    return 0;
}
EXPORT_SYMBOL(v4l2_prio_close);

enum v4l2_priority v4l2_prio_max(struct v4l2_prio_state *global)
{
    if (atomic_read(&global->prios[V4L2_PRIORITY_RECORD]) > 0)
        return V4L2_PRIORITY_RECORD;
    if (atomic_read(&global->prios[V4L2_PRIORITY_INTERACTIVE]) > 0)
        return V4L2_PRIORITY_INTERACTIVE;
    if (atomic_read(&global->prios[V4L2_PRIORITY_BACKGROUND]) > 0)
        return V4L2_PRIORITY_BACKGROUND;
    return V4L2_PRIORITY_UNSET;
}
EXPORT_SYMBOL(v4l2_prio_max);

int v4l2_prio_check(struct v4l2_prio_state *global, enum v4l2_priority *local)
{
    if (*local < v4l2_prio_max(global))
        return -EBUSY;
    return 0;
}
EXPORT_SYMBOL(v4l2_prio_check);

/* ----------------------------------------------------------------- */

/* Helper functions for control handling                 */

/* Check for correctness of the ctrl's value based on the data from
   struct v4l2_queryctrl and the available menu items. Note that
   menu_items may be NULL, in that case it is ignored. */
int v4l2_ctrl_check(struct v4l2_ext_control *ctrl, struct v4l2_queryctrl *qctrl,
        const char **menu_items)
{
    if (qctrl->flags & V4L2_CTRL_FLAG_DISABLED)
        return -EINVAL;
    if (qctrl->flags & V4L2_CTRL_FLAG_GRABBED)
        return -EBUSY;
    if (qctrl->type == V4L2_CTRL_TYPE_BUTTON ||
        qctrl->type == V4L2_CTRL_TYPE_INTEGER64 ||
        qctrl->type == V4L2_CTRL_TYPE_CTRL_CLASS)
        return 0;
    if (ctrl->value < qctrl->minimum || ctrl->value > qctrl->maximum)
        return -ERANGE;
    if (qctrl->type == V4L2_CTRL_TYPE_MENU && menu_items != NULL) {
        if (menu_items[ctrl->value] == NULL ||
            menu_items[ctrl->value][0] == '\0')
            return -EINVAL;
    }
    return 0;
}
EXPORT_SYMBOL(v4l2_ctrl_check);

/* Returns NULL or a character pointer array containing the menu for
   the given control ID. The pointer array ends with a NULL pointer.
   An empty string signifies a menu entry that is invalid. This allows
   drivers to disable certain options if it is not supported. */
const char **v4l2_ctrl_get_menu(u32 id)
{
    static const char *mpeg_audio_sampling_freq[] = {
        "44.1 kHz",
        "48 kHz",
        "32 kHz",
        NULL
    };
    static const char *mpeg_audio_encoding[] = {
        "Layer I",
        "Layer II",
        "Layer III",
        NULL
    };
    static const char *mpeg_audio_l1_bitrate[] = {
        "32 kbps",
        "64 kbps",
        "96 kbps",
        "128 kbps",
        "160 kbps",
        "192 kbps",
        "224 kbps",
        "256 kbps",
        "288 kbps",
        "320 kbps",
        "352 kbps",
        "384 kbps",
        "416 kbps",
        "448 kbps",
        NULL
    };
    static const char *mpeg_audio_l2_bitrate[] = {
        "32 kbps",
        "48 kbps",
        "56 kbps",
        "64 kbps",
        "80 kbps",
        "96 kbps",
        "112 kbps",
        "128 kbps",
        "160 kbps",
        "192 kbps",
        "224 kbps",
        "256 kbps",
        "320 kbps",
        "384 kbps",
        NULL
    };
    static const char *mpeg_audio_l3_bitrate[] = {
        "32 kbps",
        "40 kbps",
        "48 kbps",
        "56 kbps",
        "64 kbps",
        "80 kbps",
        "96 kbps",
        "112 kbps",
        "128 kbps",
        "160 kbps",
        "192 kbps",
        "224 kbps",
        "256 kbps",
        "320 kbps",
        NULL
    };
    static const char *mpeg_audio_mode[] = {
        "Stereo",
        "Joint Stereo",
        "Dual",
        "Mono",
        NULL
    };
    static const char *mpeg_audio_mode_extension[] = {
        "Bound 4",
        "Bound 8",
        "Bound 12",
        "Bound 16",
        NULL
    };
    static const char *mpeg_audio_emphasis[] = {
        "No Emphasis",
        "50/15 us",
        "CCITT J17",
        NULL
    };
    static const char *mpeg_audio_crc[] = {
        "No CRC",
        "16-bit CRC",
        NULL
    };
    static const char *mpeg_video_encoding[] = {
        "MPEG-1",
        "MPEG-2",
        NULL
    };
    static const char *mpeg_video_aspect[] = {
        "1x1",
        "4x3",
        "16x9",
        "2.21x1",
        NULL
    };
    static const char *mpeg_video_bitrate_mode[] = {
        "Variable Bitrate",
        "Constant Bitrate",
        NULL
    };
    static const char *mpeg_stream_type[] = {
        "MPEG-2 Program Stream",
        "MPEG-2 Transport Stream",
        "MPEG-1 System Stream",
        "MPEG-2 DVD-compatible Stream",
        "MPEG-1 VCD-compatible Stream",
        "MPEG-2 SVCD-compatible Stream",
        NULL
    };
    static const char *mpeg_stream_vbi_fmt[] = {
        "No VBI",
        "Private packet, IVTV format",
        NULL
    };

    switch (id) {
        case V4L2_CID_MPEG_AUDIO_SAMPLING_FREQ:
            return mpeg_audio_sampling_freq;
        case V4L2_CID_MPEG_AUDIO_ENCODING:
            return mpeg_audio_encoding;
        case V4L2_CID_MPEG_AUDIO_L1_BITRATE:
            return mpeg_audio_l1_bitrate;
        case V4L2_CID_MPEG_AUDIO_L2_BITRATE:
            return mpeg_audio_l2_bitrate;
        case V4L2_CID_MPEG_AUDIO_L3_BITRATE:
            return mpeg_audio_l3_bitrate;
        case V4L2_CID_MPEG_AUDIO_MODE:
            return mpeg_audio_mode;
        case V4L2_CID_MPEG_AUDIO_MODE_EXTENSION:
            return mpeg_audio_mode_extension;
        case V4L2_CID_MPEG_AUDIO_EMPHASIS:
            return mpeg_audio_emphasis;
        case V4L2_CID_MPEG_AUDIO_CRC:
            return mpeg_audio_crc;
        case V4L2_CID_MPEG_VIDEO_ENCODING:
            return mpeg_video_encoding;
        case V4L2_CID_MPEG_VIDEO_ASPECT:
            return mpeg_video_aspect;
        case V4L2_CID_MPEG_VIDEO_BITRATE_MODE:
            return mpeg_video_bitrate_mode;
        case V4L2_CID_MPEG_STREAM_TYPE:
            return mpeg_stream_type;
        case V4L2_CID_MPEG_STREAM_VBI_FMT:
            return mpeg_stream_vbi_fmt;
        default:
            return NULL;
    }
}
EXPORT_SYMBOL(v4l2_ctrl_get_menu);

/* Fill in a struct v4l2_queryctrl */
int v4l2_ctrl_query_fill(struct v4l2_queryctrl *qctrl, s32 min, s32 max, s32 step, s32 def)
{
    const char *name;

    qctrl->flags = 0;
    switch (qctrl->id) {
    /* USER controls */
    case V4L2_CID_USER_CLASS:     name = "User Controls"; break;
    case V4L2_CID_AUDIO_VOLUME:     name = "Volume"; break;
    case V4L2_CID_AUDIO_MUTE:     name = "Mute"; break;
    case V4L2_CID_AUDIO_BALANCE:     name = "Balance"; break;
    case V4L2_CID_AUDIO_BASS:     name = "Bass"; break;
    case V4L2_CID_AUDIO_TREBLE:     name = "Treble"; break;
    case V4L2_CID_AUDIO_LOUDNESS:     name = "Loudness"; break;
    case V4L2_CID_BRIGHTNESS:     name = "Brightness"; break;
    case V4L2_CID_CONTRAST:     name = "Contrast"; break;
    case V4L2_CID_SATURATION:     name = "Saturation"; break;
    case V4L2_CID_HUE:         name = "Hue"; break;

    /* MPEG controls */
    case V4L2_CID_MPEG_CLASS:         name = "MPEG Encoder Controls"; break;
    case V4L2_CID_MPEG_AUDIO_SAMPLING_FREQ: name = "Audio Sampling Frequency"; break;
    case V4L2_CID_MPEG_AUDIO_ENCODING:     name = "Audio Encoding Layer"; break;
    case V4L2_CID_MPEG_AUDIO_L1_BITRATE:     name = "Audio Layer I Bitrate"; break;
    case V4L2_CID_MPEG_AUDIO_L2_BITRATE:     name = "Audio Layer II Bitrate"; break;
    case V4L2_CID_MPEG_AUDIO_L3_BITRATE:     name = "Audio Layer III Bitrate"; break;
    case V4L2_CID_MPEG_AUDIO_MODE:         name = "Audio Stereo Mode"; break;
    case V4L2_CID_MPEG_AUDIO_MODE_EXTENSION: name = "Audio Stereo Mode Extension"; break;
    case V4L2_CID_MPEG_AUDIO_EMPHASIS:     name = "Audio Emphasis"; break;
    case V4L2_CID_MPEG_AUDIO_CRC:         name = "Audio CRC"; break;
    case V4L2_CID_MPEG_AUDIO_MUTE:         name = "Audio Mute"; break;
    case V4L2_CID_MPEG_VIDEO_ENCODING:     name = "Video Encoding"; break;
    case V4L2_CID_MPEG_VIDEO_ASPECT:     name = "Video Aspect"; break;
    case V4L2_CID_MPEG_VIDEO_B_FRAMES:     name = "Video B Frames"; break;
    case V4L2_CID_MPEG_VIDEO_GOP_SIZE:     name = "Video GOP Size"; break;
    case V4L2_CID_MPEG_VIDEO_GOP_CLOSURE:     name = "Video GOP Closure"; break;
    case V4L2_CID_MPEG_VIDEO_PULLDOWN:     name = "Video Pulldown"; break;
    case V4L2_CID_MPEG_VIDEO_BITRATE_MODE:     name = "Video Bitrate Mode"; break;
    case V4L2_CID_MPEG_VIDEO_BITRATE:     name = "Video Bitrate"; break;
    case V4L2_CID_MPEG_VIDEO_BITRATE_PEAK:     name = "Video Peak Bitrate"; break;
    case V4L2_CID_MPEG_VIDEO_TEMPORAL_DECIMATION: name = "Video Temporal Decimation"; break;
    case V4L2_CID_MPEG_VIDEO_MUTE:         name = "Video Mute"; break;
    case V4L2_CID_MPEG_VIDEO_MUTE_YUV:    name = "Video Mute YUV"; break;
    case V4L2_CID_MPEG_STREAM_TYPE:     name = "Stream Type"; break;
    case V4L2_CID_MPEG_STREAM_PID_PMT:     name = "Stream PMT Program ID"; break;
    case V4L2_CID_MPEG_STREAM_PID_AUDIO:     name = "Stream Audio Program ID"; break;
    case V4L2_CID_MPEG_STREAM_PID_VIDEO:     name = "Stream Video Program ID"; break;
    case V4L2_CID_MPEG_STREAM_PID_PCR:     name = "Stream PCR Program ID"; break;
    case V4L2_CID_MPEG_STREAM_PES_ID_AUDIO: name = "Stream PES Audio ID"; break;
    case V4L2_CID_MPEG_STREAM_PES_ID_VIDEO: name = "Stream PES Video ID"; break;
    case V4L2_CID_MPEG_STREAM_VBI_FMT:    name = "Stream VBI Format"; break;

    default:
        return -EINVAL;
    }
    switch (qctrl->id) {
    case V4L2_CID_AUDIO_MUTE:
    case V4L2_CID_AUDIO_LOUDNESS:
    case V4L2_CID_MPEG_AUDIO_MUTE:
    case V4L2_CID_MPEG_VIDEO_MUTE:
    case V4L2_CID_MPEG_VIDEO_GOP_CLOSURE:
    case V4L2_CID_MPEG_VIDEO_PULLDOWN:
        qctrl->type = V4L2_CTRL_TYPE_BOOLEAN;
        min = 0;
        max = step = 1;
        break;
    case V4L2_CID_MPEG_AUDIO_SAMPLING_FREQ:
    case V4L2_CID_MPEG_AUDIO_ENCODING:
    case V4L2_CID_MPEG_AUDIO_L1_BITRATE:
    case V4L2_CID_MPEG_AUDIO_L2_BITRATE:
    case V4L2_CID_MPEG_AUDIO_L3_BITRATE:
    case V4L2_CID_MPEG_AUDIO_MODE:
    case V4L2_CID_MPEG_AUDIO_MODE_EXTENSION:
    case V4L2_CID_MPEG_AUDIO_EMPHASIS:
    case V4L2_CID_MPEG_AUDIO_CRC:
    case V4L2_CID_MPEG_VIDEO_ENCODING:
    case V4L2_CID_MPEG_VIDEO_ASPECT:
    case V4L2_CID_MPEG_VIDEO_BITRATE_MODE:
    case V4L2_CID_MPEG_STREAM_TYPE:
    case V4L2_CID_MPEG_STREAM_VBI_FMT:
        qctrl->type = V4L2_CTRL_TYPE_MENU;
        step = 1;
        break;
    case V4L2_CID_USER_CLASS:
    case V4L2_CID_MPEG_CLASS:
        qctrl->type = V4L2_CTRL_TYPE_CTRL_CLASS;
        qctrl->flags |= V4L2_CTRL_FLAG_READ_ONLY;
        min = max = step = def = 0;
        break;
    default:
        qctrl->type = V4L2_CTRL_TYPE_INTEGER;
        break;
    }
    switch (qctrl->id) {
    case V4L2_CID_MPEG_AUDIO_ENCODING:
    case V4L2_CID_MPEG_AUDIO_MODE:
    case V4L2_CID_MPEG_VIDEO_BITRATE_MODE:
    case V4L2_CID_MPEG_VIDEO_B_FRAMES:
    case V4L2_CID_MPEG_STREAM_TYPE:
        qctrl->flags |= V4L2_CTRL_FLAG_UPDATE;
        break;
    case V4L2_CID_AUDIO_VOLUME:
    case V4L2_CID_AUDIO_BALANCE:
    case V4L2_CID_AUDIO_BASS:
    case V4L2_CID_AUDIO_TREBLE:
    case V4L2_CID_BRIGHTNESS:
    case V4L2_CID_CONTRAST:
    case V4L2_CID_SATURATION:
    case V4L2_CID_HUE:
        qctrl->flags |= V4L2_CTRL_FLAG_SLIDER;
        break;
    }
    qctrl->minimum = min;
    qctrl->maximum = max;
    qctrl->step = step;
    qctrl->default_value = def;
    qctrl->reserved[0] = qctrl->reserved[1] = 0;
    snprintf(qctrl->name, sizeof(qctrl->name), name);
    return 0;
}
EXPORT_SYMBOL(v4l2_ctrl_query_fill);

/* Fill in a struct v4l2_queryctrl with standard values based on
   the control ID. */
int v4l2_ctrl_query_fill_std(struct v4l2_queryctrl *qctrl)
{
    switch (qctrl->id) {
    /* USER controls */
    case V4L2_CID_USER_CLASS:
    case V4L2_CID_MPEG_CLASS:
        return v4l2_ctrl_query_fill(qctrl, 0, 0, 0, 0);
    case V4L2_CID_AUDIO_VOLUME:
        return v4l2_ctrl_query_fill(qctrl, 0, 65535, 65535 / 100, 58880);
    case V4L2_CID_AUDIO_MUTE:
    case V4L2_CID_AUDIO_LOUDNESS:
        return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 0);
    case V4L2_CID_AUDIO_BALANCE:
    case V4L2_CID_AUDIO_BASS:
    case V4L2_CID_AUDIO_TREBLE:
        return v4l2_ctrl_query_fill(qctrl, 0, 65535, 65535 / 100, 32768);
    case V4L2_CID_BRIGHTNESS:
        return v4l2_ctrl_query_fill(qctrl, 0, 255, 1, 128);
    case V4L2_CID_CONTRAST:
    case V4L2_CID_SATURATION:
        return v4l2_ctrl_query_fill(qctrl, 0, 127, 1, 64);
    case V4L2_CID_HUE:
        return v4l2_ctrl_query_fill(qctrl, -128, 127, 1, 0);

    /* MPEG controls */
    case V4L2_CID_MPEG_AUDIO_SAMPLING_FREQ:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_SAMPLING_FREQ_44100,
                V4L2_MPEG_AUDIO_SAMPLING_FREQ_32000, 1,
                V4L2_MPEG_AUDIO_SAMPLING_FREQ_48000);
    case V4L2_CID_MPEG_AUDIO_ENCODING:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_ENCODING_LAYER_1,
                V4L2_MPEG_AUDIO_ENCODING_LAYER_3, 1,
                V4L2_MPEG_AUDIO_ENCODING_LAYER_2);
    case V4L2_CID_MPEG_AUDIO_L1_BITRATE:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_L1_BITRATE_32K,
                V4L2_MPEG_AUDIO_L1_BITRATE_448K, 1,
                V4L2_MPEG_AUDIO_L1_BITRATE_256K);
    case V4L2_CID_MPEG_AUDIO_L2_BITRATE:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_L2_BITRATE_32K,
                V4L2_MPEG_AUDIO_L2_BITRATE_384K, 1,
                V4L2_MPEG_AUDIO_L2_BITRATE_224K);
    case V4L2_CID_MPEG_AUDIO_L3_BITRATE:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_L3_BITRATE_32K,
                V4L2_MPEG_AUDIO_L3_BITRATE_320K, 1,
                V4L2_MPEG_AUDIO_L3_BITRATE_192K);
    case V4L2_CID_MPEG_AUDIO_MODE:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_MODE_STEREO,
                V4L2_MPEG_AUDIO_MODE_MONO, 1,
                V4L2_MPEG_AUDIO_MODE_STEREO);
    case V4L2_CID_MPEG_AUDIO_MODE_EXTENSION:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_4,
                V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_16, 1,
                V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_4);
    case V4L2_CID_MPEG_AUDIO_EMPHASIS:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_EMPHASIS_NONE,
                V4L2_MPEG_AUDIO_EMPHASIS_CCITT_J17, 1,
                V4L2_MPEG_AUDIO_EMPHASIS_NONE);
    case V4L2_CID_MPEG_AUDIO_CRC:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_AUDIO_CRC_NONE,
                V4L2_MPEG_AUDIO_CRC_CRC16, 1,
                V4L2_MPEG_AUDIO_CRC_NONE);
    case V4L2_CID_MPEG_AUDIO_MUTE:
        return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 0);
    case V4L2_CID_MPEG_VIDEO_ENCODING:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_VIDEO_ENCODING_MPEG_1,
                V4L2_MPEG_VIDEO_ENCODING_MPEG_2, 1,
                V4L2_MPEG_VIDEO_ENCODING_MPEG_2);
    case V4L2_CID_MPEG_VIDEO_ASPECT:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_VIDEO_ASPECT_1x1,
                V4L2_MPEG_VIDEO_ASPECT_221x100, 1,
                V4L2_MPEG_VIDEO_ASPECT_4x3);
    case V4L2_CID_MPEG_VIDEO_B_FRAMES:
        return v4l2_ctrl_query_fill(qctrl, 0, 33, 1, 2);
    case V4L2_CID_MPEG_VIDEO_GOP_SIZE:
        return v4l2_ctrl_query_fill(qctrl, 1, 34, 1, 12);
    case V4L2_CID_MPEG_VIDEO_GOP_CLOSURE:
        return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 1);
    case V4L2_CID_MPEG_VIDEO_PULLDOWN:
        return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 0);
    case V4L2_CID_MPEG_VIDEO_BITRATE_MODE:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_VIDEO_BITRATE_MODE_VBR,
                V4L2_MPEG_VIDEO_BITRATE_MODE_CBR, 1,
                V4L2_MPEG_VIDEO_BITRATE_MODE_VBR);
    case V4L2_CID_MPEG_VIDEO_BITRATE:
        return v4l2_ctrl_query_fill(qctrl, 0, 27000000, 1, 6000000);
    case V4L2_CID_MPEG_VIDEO_BITRATE_PEAK:
        return v4l2_ctrl_query_fill(qctrl, 0, 27000000, 1, 8000000);
    case V4L2_CID_MPEG_VIDEO_TEMPORAL_DECIMATION:
        return v4l2_ctrl_query_fill(qctrl, 0, 255, 1, 0);
    case V4L2_CID_MPEG_VIDEO_MUTE:
        return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 0);
    case V4L2_CID_MPEG_VIDEO_MUTE_YUV:  /* Init YUV (really YCbCr) to black */
        return v4l2_ctrl_query_fill(qctrl, 0, 0xffffff, 1, 0x008080);
    case V4L2_CID_MPEG_STREAM_TYPE:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_STREAM_TYPE_MPEG2_PS,
                V4L2_MPEG_STREAM_TYPE_MPEG2_SVCD, 1,
                V4L2_MPEG_STREAM_TYPE_MPEG2_PS);
    case V4L2_CID_MPEG_STREAM_PID_PMT:
        return v4l2_ctrl_query_fill(qctrl, 0, (1 << 14) - 1, 1, 16);
    case V4L2_CID_MPEG_STREAM_PID_AUDIO:
        return v4l2_ctrl_query_fill(qctrl, 0, (1 << 14) - 1, 1, 260);
    case V4L2_CID_MPEG_STREAM_PID_VIDEO:
        return v4l2_ctrl_query_fill(qctrl, 0, (1 << 14) - 1, 1, 256);
    case V4L2_CID_MPEG_STREAM_PID_PCR:
        return v4l2_ctrl_query_fill(qctrl, 0, (1 << 14) - 1, 1, 259);
    case V4L2_CID_MPEG_STREAM_PES_ID_AUDIO:
        return v4l2_ctrl_query_fill(qctrl, 0, 255, 1, 0);
    case V4L2_CID_MPEG_STREAM_PES_ID_VIDEO:
        return v4l2_ctrl_query_fill(qctrl, 0, 255, 1, 0);
    case V4L2_CID_MPEG_STREAM_VBI_FMT:
        return v4l2_ctrl_query_fill(qctrl,
                V4L2_MPEG_STREAM_VBI_FMT_NONE,
                V4L2_MPEG_STREAM_VBI_FMT_IVTV, 1,
                V4L2_MPEG_STREAM_VBI_FMT_NONE);
    default:
        return -EINVAL;
    }
}
EXPORT_SYMBOL(v4l2_ctrl_query_fill_std);

/* Fill in a struct v4l2_querymenu based on the struct v4l2_queryctrl and
   the menu. The qctrl pointer may be NULL, in which case it is ignored. */
int v4l2_ctrl_query_menu(struct v4l2_querymenu *qmenu, struct v4l2_queryctrl *qctrl,
           const char **menu_items)
{
    int i;

    if (menu_items == NULL ||
        (qctrl && (qmenu->index < qctrl->minimum || qmenu->index > qctrl->maximum)))
        return -EINVAL;
    for (i = 0; i < qmenu->index && menu_items[i]; i++) ;
    if (menu_items[i] == NULL || menu_items[i][0] == '\0')
        return -EINVAL;
    snprintf(qmenu->name, sizeof(qmenu->name), menu_items[qmenu->index]);
    qmenu->reserved = 0;
    return 0;
}
EXPORT_SYMBOL(v4l2_ctrl_query_menu);

/* ctrl_classes points to an array of u32 pointers, the last element is
   a NULL pointer. Each u32 array is a 0-terminated array of control IDs.
   Each array must be sorted low to high and belong to the same control
   class. The array of u32 pointer must also be sorted, from low class IDs
   to high class IDs.

   This function returns the first ID that follows after the given ID.
   When no more controls are available 0 is returned. */
u32 v4l2_ctrl_next(const u32 * const * ctrl_classes, u32 id)
{
    u32 ctrl_class = V4L2_CTRL_ID2CLASS(id);
    const u32 *pctrl;

    if (ctrl_classes == NULL)
        return 0;

    /* if no query is desired, then check if the ID is part of ctrl_classes */
    if ((id & V4L2_CTRL_FLAG_NEXT_CTRL) == 0) {
        /* find class */
        while (*ctrl_classes && V4L2_CTRL_ID2CLASS(**ctrl_classes) != ctrl_class)
            ctrl_classes++;
        if (*ctrl_classes == NULL)
            return 0;
        pctrl = *ctrl_classes;
        /* find control ID */
        while (*pctrl && *pctrl != id) pctrl++;
        return *pctrl ? id : 0;
    }
    id &= V4L2_CTRL_ID_MASK;
    id++;    /* select next control */
    /* find first class that matches (or is greater than) the class of
       the ID */
    while (*ctrl_classes && V4L2_CTRL_ID2CLASS(**ctrl_classes) < ctrl_class)
        ctrl_classes++;
    /* no more classes */
    if (*ctrl_classes == NULL)
        return 0;
    pctrl = *ctrl_classes;
    /* find first ctrl within the class that is >= ID */
    while (*pctrl && *pctrl < id) pctrl++;
    if (*pctrl)
        return *pctrl;
    /* we are at the end of the controls of the current class. */
    /* continue with next class if available */
    ctrl_classes++;
    if (*ctrl_classes == NULL)
        return 0;
    return **ctrl_classes;
}
EXPORT_SYMBOL(v4l2_ctrl_next);

int v4l2_chip_match_host(u32 match_type, u32 match_chip)
{
    switch (match_type) {
    case V4L2_CHIP_MATCH_HOST:
        return match_chip == 0;
    default:
        return 0;
    }
}
EXPORT_SYMBOL(v4l2_chip_match_host);

#if defined(CONFIG_I2C) || (defined(CONFIG_I2C_MODULE) && defined(MODULE))
int v4l2_chip_match_i2c_client(struct i2c_client *c, u32 match_type, u32 match_chip)
{
    switch (match_type) {
    case V4L2_CHIP_MATCH_I2C_DRIVER:
        return (c != NULL && c->driver != NULL && c->driver->id == match_chip);
    case V4L2_CHIP_MATCH_I2C_ADDR:
        return (c != NULL && c->addr == match_chip);
    default:
        return 0;
    }
}
EXPORT_SYMBOL(v4l2_chip_match_i2c_client);

int v4l2_chip_ident_i2c_client(struct i2c_client *c, struct v4l2_chip_ident *chip,
        u32 ident, u32 revision)
{
    if (!v4l2_chip_match_i2c_client(c, chip->match_type, chip->match_chip))
        return 0;
    if (chip->ident == V4L2_IDENT_NONE) {
        chip->ident = ident;
        chip->revision = revision;
    }
    else {
        chip->ident = V4L2_IDENT_AMBIGUOUS;
        chip->revision = 0;
    }
    return 0;
}
EXPORT_SYMBOL(v4l2_chip_ident_i2c_client);

/* ----------------------------------------------------------------- */

/* Helper function for I2C legacy drivers */

int v4l2_i2c_attach(struct i2c_adapter *adapter, int address, struct i2c_driver *driver,
        const char *name,
        int (*probe)(struct i2c_client *, const struct i2c_device_id *))
{
    struct i2c_client *client;
    int err;

    client = kzalloc(sizeof(struct i2c_client), GFP_KERNEL);
    if (!client)
        return -ENOMEM;

    client->addr = address;
    client->adapter = adapter;
    client->driver = driver;
    strlcpy(client->name, name, sizeof(client->name));

    err = probe(client, NULL);
    if (err == 0) {
        i2c_attach_client(client);
    } else {
        kfree(client);
    }
    return err != -ENOMEM ? 0 : err;
}
EXPORT_SYMBOL(v4l2_i2c_attach);
#endif
