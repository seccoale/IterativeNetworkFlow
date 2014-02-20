#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QMainWindow>
#include <string>
#include <QGraphicsPixmapItem>
#include <QStack>
#include <QFrame>
#include <QGraphicsScene>
#include "taskdeclarationwindow.h"
#include "Structs.h"
#include <QListWidgetItem>
#include <graphviz/gvc.h>
#include <fstream>
#include <iostream>
//#include "fordfulkersonrunner.h"

namespace Ui {
class GraphView;
}

using namespace inf;
class GraphView : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GraphView(QWidget *parent = 0);
    void drawGraph(string literal);
    bool fastPlay();
    ~GraphView();
public slots:
    void endedFlowComputation(double maxFlow);
    void repaintRequested();
private slots:
    void on_add_task_button_clicked();
    void task_received(Task* task);
    void task_discarded();
    void task_edited(Task* newTask, int rowOldTask);
    void on_listTask_itemClicked(QListWidgetItem *item);

    void on_editTaskBtn_clicked();

    void on_removeBtn_clicked();


    void on_listTask_itemDoubleClicked(QListWidgetItem *item);

    void on_import_graph_button_clicked();

    void on_play_stop_button_clicked();

    void on_frameSizes_activated(const QString &arg1);

   // void on_sleepMS_valueChanged(int ms);
    void on_nextButton_clicked();

    void on_prevButton_clicked();

    void on_saveGraphBTN_clicked();

    void on_saveSchedBTN_clicked();

signals:
    void sleepMSChanged(int ms);

private:
    void resizeSchedule();
    void schedResize();
    //void resizeEvent(QResizeEvent *event);
    QPixmap schedToImage();
    void drawSchedule();
    void addToSchedule(JobVertex* jv, FrameVertex* fv, QPen *pen);
    bool play=false;
    vector<double> frame_sizes;
    double frame;
    //FordFulkersonRunner* runner;
    INFGraph* graph;
    TaskSet* set;
    int tasks_inserted;
    Ui::GraphView *ui;
    QGraphicsScene* currentScene;
    QGraphicsScene* schedScene;
    QGraphicsPixmapItem* currentPixmap;
    vector<QGraphicsItem*>* scheduleItems;
    QStack<QPixmap>* historyGraph;
    QStack<QPixmap>* forwardGraph;
    QStack<QPixmap*>* historySched;
    QStack<QPixmap*>*forwardSched;
    void setNewImage(string loc);
    void replaceImg(string loc);
    void initGantt();
};

#endif // GRAPHVIEW_H


