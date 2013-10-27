#include "graphview.h"
#include <QApplication>
#include "stdio.h"
#include <iostream>
#include "Structs.h"
using namespace std;
using namespace inf;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphView w;
    w.show();
    return a.exec();
}
