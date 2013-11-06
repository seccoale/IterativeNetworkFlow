#include "graphview.h"
#include "ui_graphview.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <QDir>
#include <unistd.h>
#include <infalgoithms.h>

#define RESOURCES_FOLDER QCoreApplication::applicationDirPath().toStdString()+QDir::separator().toLatin1()+"resources"+QDir::separator().toLatin1()
#define FIRST_IMAGE RESOURCES_FOLDER+"first_image.png"
#define TMP_GRAPH_INPUT_FILE "digraph.tmp"
#define TMP_GRAPH_FILE_LOC RESOURCES_FOLDER+TMP_GRAPH_INPUT_FILE
#define TMP_GRAPH_OUTPUT_FILE "digraph.tmp.png"
#define TMP_GRAPH_OUTPUT RESOURCES_FOLDER+TMP_GRAPH_OUTPUT_FILE
#define DESCR_AND_AUTHOR "RTS, INF. Prvided by Alessandro Secco (1041258) seccoale@dei.unipd.it"
#define TIME_TO_SLEEP 1
using namespace std;
using namespace inf;
GraphView::GraphView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphView)
{
    ui->setupUi(this);
    this->tasks_inserted=0;
    ui->centralWidget->setWindowTitle(DESCR_AND_AUTHOR);
    ui->graphicsView_graph->setSceneRect(0,0,1400,600);
    this->currentScene=new QGraphicsScene();
    this->currentScene->setSceneRect(0,0,1400,600);
    string first_img_loc=FIRST_IMAGE;
    QPixmap first_img(first_img_loc.c_str());
    this->currentScene->addPixmap(first_img);
    ui->graphicsView_graph->setScene(this->currentScene);
}

GraphView::~GraphView()
{
    delete ui;
}


void GraphView::setNewImage(string loc){
    QPixmap img(loc.c_str());
    this->currentPixmap=this->currentScene->addPixmap(img);
    this->currentScene->addPixmap(img);
    //ui->graphicsView_graph->setScene(this->currentScene);
    //ui->graphicsView_graph->update();
}

void GraphView::replaceImg(string loc){
    QGraphicsPixmapItem prev_img(this->currentPixmap);
    this->currentScene->removeItem(this->currentPixmap);
    QPixmap new_img(loc.c_str());
    this->currentPixmap=this->currentScene->addPixmap(new_img);
    //this->currentPixmap->setPixmap(new_img);
    this->history.push_front(&prev_img);
   /* QPixmap img(loc.c_str());
    this->currentScene->clear();
    this->history.push_back(this->currentPixmap);
    this->currentPixmap=this->currentScene->addPixmap(img);
    this->currentScene->addPixmap(img);
    ui->graphicsView_graph->setScene(this->currentScene);
    ui->graphicsView_graph->update();*/
}

void GraphView::on_add_task_button_clicked()
{

    TaskDeclarationWindow* taskDecl=new TaskDeclarationWindow(this);
    taskDecl->setModal(true);
    connect(taskDecl, SIGNAL(addTask(Task*)), this, SLOT(task_received(Task*)));
    connect(taskDecl, SIGNAL(discardTask()), this, SLOT(task_discarded()));
    taskDecl->show();
}
void GraphView::task_received(Task * task){
    this->ui->add_task_button->setEnabled(true);

    this->ui->listTask->insertItem(++tasks_inserted, task->toString().c_str());
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
    task=task->compile(&selectedTask);
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
    this->ui->listTask->insertItem(rowOldTask, newTask->toString().c_str());
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

void GraphView::on_import_graph_button_clicked()
{
    INFGraph* g=new INFGraph(ui->graphNameLE->text().toStdString());
    TaskSet* set=new TaskSet();
    for(int i=0; i<ui->listTask->count(); i++){
        Task* newTask=new Task();
        QString literal=ui->listTask->item(i)->text();
        newTask=newTask->compile(&literal);
        set->addTask(newTask);
    }
    this->frame_sizes=INFAlgoithms::detectFrameSizes(set);
    this->current_frame=0;
    g->importTaskSet(set, this->frame_sizes.at(this->current_frame), INFAlgoithms::findHyperperiod(set));
    drawGraph(g->toStringComplete());
}

void GraphView::drawGraph(string literal){
    string input_file=TMP_GRAPH_FILE_LOC;
    std::ofstream inputGraphFile (input_file.c_str());
    inputGraphFile<<literal.c_str()<<endl;
    inputGraphFile.close();
    string resources_foder_with_spaces=RESOURCES_FOLDER;
    string new_image_spaces=resources_foder_with_spaces+TMP_GRAPH_OUTPUT_FILE;
    QString* resources_folder=new QString(resources_foder_with_spaces.c_str());
    *resources_folder=resources_folder->replace(*new QString(" "), *new QString("\\ "));
    string new_image_no_sapces=resources_folder->toStdString()+TMP_GRAPH_OUTPUT_FILE;
    string command="dot -Tpng "+resources_folder->toStdString()+TMP_GRAPH_INPUT_FILE+" -O "+new_image_no_sapces.c_str();
    system(command.c_str());
    this->setNewImage(new_image_spaces);

}
