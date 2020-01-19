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
#include "ukmedia_control_center_widget.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTranslator>

#define GETTEXT_PACKAGE "ukui-media"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //加载qm翻译文件o
    QString locale = QLocale::system().name();
    QTranslator translator;

    if (locale == "zh_CN") {
        if (translator.load("/home/kylin/workspace/audio/translations/audio-zh_CN.qm")) {
            a.installTranslator(&translator);
        }
        else {
            qDebug() << "Load translations file" << locale << "failed!";
        }
    }
    QFile qss(":/data/qss/audio.qss");
    bool ok = qss.open(QFile::ReadOnly);
    if (!ok)
        qDebug() << "加载失败";
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    UkmediaControlCenterWidget w;
    w.show();

    return a.exec();
}
