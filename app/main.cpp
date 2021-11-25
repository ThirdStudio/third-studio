#include <QApplication>
#include <QCoreApplication>
#include <QStyleFactory>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("SimplestStudio");
    QCoreApplication::setApplicationName("SimplestStudio");
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    app.setStyle(QStyleFactory::create("Fusion"));
    QFont font = app.font();
    font.setPointSize(8);
    app.setFont(font);

    MainWindow w;
    w.show();
    return app.exec();
}


