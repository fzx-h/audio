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
