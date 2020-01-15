#include "appvolume.h"

AppVolume::AppVolume()
{
    //应用音量显示标签
    app_display_label = new QLabel();
    app_display_label->setText(tr("No application is currently playing or recording audio"));

    appLabel = new QLabel();
    appVolumeLabel = new QLabel();
    appSlider = new QSlider(Qt::Horizontal);
//    standItemModel->removeRows(0,1);
    appSlider->setMaximum(100);
    appSlider->setFixedSize(178,20);
//    appVolumeTableView->setIndexWidget(standItemModel->index(appnum-1,0),w->appLabel);
//    appVolumeTableView->setIndexWidget(standItemModel->index(appnum-1,1),w->appSlider);
//    appVolumeTableView->setIndexWidget(standItemModel->index(appnum-1,2),w->appVolumeLabel);
}
