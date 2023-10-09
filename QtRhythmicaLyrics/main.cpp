#include "QtRhythmicaLyrics.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtRhythmicaLyrics w;
    w.show();
    return a.exec();
}
