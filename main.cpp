#include "graphview.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphView w;
    w.show();
    
    return a.exec();
}
