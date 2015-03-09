#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QTranslator trans;
    if (trans.load(QLocale::system(), "winabw", "_", app.applicationDirPath())) {
        app.installTranslator(&trans);
    } else {
        qDebug("Failed to load translation");
    }

    MainWindow w;
    w.show();
    
    return app.exec();
}
