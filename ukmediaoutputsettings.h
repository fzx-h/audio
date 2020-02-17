#ifndef UKMEDIAOUTPUTSETTINGS_H
#define UKMEDIAOUTPUTSETTINGS_H

#include <libmatemixer/matemixer.h>

class UkmediaOutputSettings
{
public:
    UkmediaOutputSettings();
private:
    MateMixerStream *stream;
};

#endif // UKMEDIAOUTPUTSETTINGS_H
