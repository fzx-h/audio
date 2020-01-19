/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */
#ifndef UTILS_H
#define UTILS_H

#include <libmatemixer/matemixer.h>

class utils
{
public:
    utils();
    const gchar *gvc_channel_position_to_pulse_string (MateMixerChannelPosition position);
    const gchar *gvc_channel_position_to_pretty_string (MateMixerChannelPosition position);
    static const gchar *gvc_channel_map_to_pretty_string (MateMixerStreamControl *control);
};

#endif // UTILS_H
