#include <QtGui/QApplication>
#include "LAWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LAWindow w;
    w.show();

    return a.exec();
}
