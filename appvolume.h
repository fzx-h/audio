#ifndef APPVOLUME_H
#define APPVOLUME_H
#include <QLabel>
#include <QSlider>

class AppVolume
{
    Q_OBJECT
public:
    AppVolume();

public:
    QLabel *appLabel;
    QLabel *appVolumeLabel;
    QSlider *appSlider;

    QLabel *app_display_label;


};

#endif // APPVOLUME_H
