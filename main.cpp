#include "stdafx.h"
#include "graphicsdoc.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphicsDoc w;
    w.show();

    return a.exec();
}
