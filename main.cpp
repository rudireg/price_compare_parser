#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // QTextCodec *codecUtf8 = QTextCodec::codecForName("Windows-1251");
    QTextCodec *codecUtf8 = QTextCodec::codecForName("utf8");
    QTextCodec::setCodecForLocale(codecUtf8);

    qRegisterMetaType<QList<Article>*>("QList<Article>*");
    qRegisterMetaType<TInfo>("TInfo");
    qRegisterMetaType<QList<Site>>("QList<Site>");

    MainWindow w;
    w.resize(900, 400);
//    w.setWindowState(Qt::WindowMaximized);
    w.show();

    return a.exec();
}
