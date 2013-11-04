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

namespace Ui {
class GraphView;
}

using namespace inf;
class GraphView : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GraphView(QWidget *parent = 0);
    ~GraphView();
private slots:
    void on_add_task_button_clicked();
    void task_received(Task* task);
    void task_discarded();
    void task_edited(Task* newTask, int rowOldTask);
    void on_listTask_itemClicked(QListWidgetItem *item);

    void on_editTaskBtn_clicked();

    void on_removeBtn_clicked();


    void on_listTask_itemDoubleClicked(QListWidgetItem *item);

private:
    int tasks_inserted;
    Ui::GraphView *ui;
    QGraphicsScene* currentScene;
    QGraphicsPixmapItem* currentPixmap;
};

#endif // GRAPHVIEW_H



