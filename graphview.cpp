#include "graphview.h"
#include "ui_graphview.h"
#include <string>
#include <stdio.h>
#include <iostream>

#define FIRST_IMAGE "/media/data/Universita/Sistemi_In_Tempo_Reale/Progetto/IterativeNetworkFlow/resources/first_image.png"
using namespace std;
using namespace inf;
GraphView::GraphView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphView)
{
    ui->setupUi(this);
    this->tasks_inserted=0;
    ui->centralWidget->setWindowTitle("RTS, INF. Prvided by Alessandro Secco (1041258) seccoale@dei.unipd.it");
    ui->graphicsView_graph->setSceneRect(0,0,300,450);
    this->currentScene=new QGraphicsScene();
    this->currentScene->setSceneRect(0,0,300,450);
    QPixmap firstImage(FIRST_IMAGE);
    this->currentPixmap=this->currentScene->addPixmap(firstImage);
    this->currentScene->addPixmap(firstImage);
    ui->graphicsView_graph->setScene(this->currentScene);
}

GraphView::~GraphView()
{
    delete ui;
}

void GraphView::on_add_task_button_clicked()
{
    this->ui->add_task_button->setEnabled(false);
    this->ui->removeBtn->setEnabled(false);
    this->ui->editTaskBtn->setEnabled(false);
   // this->ui->listTask->setEnabled(false);

    TaskDeclarationWindow* taskDecl=new TaskDeclarationWindow(this);
    taskDecl->setModal(true);
    connect(taskDecl, SIGNAL(addTask(Task*)), this, SLOT(task_received(Task*)));
    connect(taskDecl, SIGNAL(discardTask()), this, SLOT(task_discarded()));
    taskDecl->show();
}
void GraphView::task_received(Task * task){
    this->ui->add_task_button->setEnabled(true);

    cout<<task->toString()->toUtf8().constData()<<endl;
    this->ui->listTask->insertItem(++tasks_inserted, task->toString()->toUtf8().constData());
}
void GraphView::task_discarded(){
    //this->ui->add_task_button->setEnabled(true);
    //this->ui->editTaskBtn->setEnabled(true);
    //this->ui->removeBtn->setEnabled(true);
}

void GraphView::on_listTask_itemClicked(QListWidgetItem *item)
{
    this->ui->editTaskBtn->setEnabled(true);
    this->ui->removeBtn->setEnabled(true);
}
void GraphView::on_editTaskBtn_clicked()
{
 //  this->ui->add_task_button->setEnabled(false);
   // this->ui->editTaskBtn->setEnabled(false);
//    this->ui->removeBtn->setEnabled(false);

    QString selectedTask=this->ui->listTask->selectedItems().front()->text();
    Task* task=new Task();
    task=task->compile(selectedTask);
    TaskDeclarationWindow* taskEditWindow=new TaskDeclarationWindow(this, task, this->ui->listTask->currentIndex().row());
    taskEditWindow->setModal(true);
    taskEditWindow->show();
    connect(taskEditWindow, SIGNAL(replaceTask(Task*,int)),this,SLOT(task_edited(Task*,int)));
    connect(taskEditWindow, SIGNAL(rejected()),this,SLOT(task_discarded()));
}
void GraphView::on_removeBtn_clicked()
{
    QListWidgetItem* selected=this->ui->listTask->currentItem();
    this->ui->listTask->removeItemWidget(selected);
    delete selected;
    if(this->ui->listTask->selectedItems().isEmpty()){
        this->ui->editTaskBtn->setEnabled(false);
        this->ui->removeBtn->setEnabled(false);
    }
}
void GraphView::task_edited(Task *newTask, int rowOldTask){
    delete this->ui->listTask->item(rowOldTask);
    this->ui->listTask->insertItem(rowOldTask, newTask->toString()->toUtf8().constData());
    this->ui->listTask->setItemSelected(this->ui->listTask->item(rowOldTask), true);
    this->ui->add_task_button->setEnabled(true);
    this->ui->editTaskBtn->setEnabled(true);
    this->ui->removeBtn->setEnabled(true);
}
void GraphView::on_listTask_itemDoubleClicked(QListWidgetItem *item)
{
    on_editTaskBtn_clicked();
    this->ui->editTaskBtn->setEnabled(false);
    this->ui->removeBtn->setEnabled(false);
}
