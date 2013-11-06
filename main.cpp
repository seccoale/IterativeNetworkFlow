#include "graphview.h"
#include <QApplication>
#include "stdio.h"
#include <iostream>
#include "Structs.h"
#include "infalgoithms.h"
using namespace std;
using namespace inf;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GraphView w;
    TaskSet* set=new TaskSet();
    set->addTask(new Task("T1",4,4,1));
    set->addTask(new Task("T2",5,7,2));
    set->addTask(new Task("T3",20,20,5));
    vector<int> integerPeriods=vector<int>();
    for(int i=0; i<set->size(); i++){
        integerPeriods.push_back(((int)set->at(i)->period*100));
    }
    INFGraph* g=new INFGraph("TEST");
    g->importTaskSet(set, INFAlgoithms::detectFrameSizes(set).at(0), INFAlgoithms::findHyperperiod(integerPeriods));
    w.show();
    return a.exec();
}
