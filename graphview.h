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
    ~GraphView();
public slots:
    void endedFlowComputation(double maxFlow);
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

private:
    vector<double> frame_sizes;
    int current_frame=0;
    INFGraph* graph;
    TaskSet* set;
    int tasks_inserted;
    Ui::GraphView *ui;
    QGraphicsScene* currentScene;
    QGraphicsPixmapItem* currentPixmap;
    QStack<QGraphicsPixmapItem*> history;
    QStack<QGraphicsPixmapItem*> forward;
    void setNewImage(string loc);
    void replaceImg(string loc);
   // FordFulkersonRunner* runner;
};

#endif // GRAPHVIEW_H
