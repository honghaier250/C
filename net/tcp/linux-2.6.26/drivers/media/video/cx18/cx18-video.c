/*
 *  cx18 video interface functions
 *
 *  Copyright (C) 2007  Hans Verkuil <hverkuil@xs4all.nl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA
 */

#include "cx18-driver.h"
#include "cx18-video.h"
#include "cx18-av-core.h"
#include "cx18-cards.h"

void cx18_video_set_io(struct cx18 *cx)
{
    struct v4l2_routing route;
    int inp = cx->active_input;
    u32 type;

    route.input = cx->card->video_inputs[inp].video_input;
    route.output = 0;
    cx18_av_cmd(cx, VIDIOC_INT_S_VIDEO_ROUTING, &route);

    type = cx->card->video_inputs[inp].video_type;

    if (type == CX18_CARD_INPUT_VID_TUNER)
        route.input = 0;  /* Tuner */
    else if (type < CX18_CARD_INPUT_COMPOSITE1)
        route.input = 2;  /* S-Video */
    else
        route.input = 1;  /* Composite */
}
