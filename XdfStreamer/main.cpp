#include "xdfstreamer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XdfStreamer w;
    w.show();

    return a.exec();
}
