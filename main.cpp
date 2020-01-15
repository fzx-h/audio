#include "widget.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTranslator>
//#include <glib/gi18n.h>
//#include <libmatemixer/matemixer.h>
#define GETTEXT_PACKAGE "mate-media"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //加载qm翻译文件o
    QString locale = QLocale::system().name();
    QTranslator translator;

    if (locale == "zh_CN") {
        if (translator.load("/home/fzx/fzx/audio/audio-zh_CN.qm")) {
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

    Widget w;
    w.show();

    return a.exec();
}
