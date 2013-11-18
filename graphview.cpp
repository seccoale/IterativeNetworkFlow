#include "graphview.h"
#include "ui_graphview.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <QDir>
#include <unistd.h>
#include <infalgoithms.h>
#include "fordfulkersonrunner.h"

#define RESOURCES_FOLDER QCoreApplication::applicationDirPath().toStdString()+QDir::separator().toLatin1()+"resources"+QDir::separator().toLatin1()
#define FIRST_IMAGE RESOURCES_FOLDER+"first_image.png"
#define TMP_GRAPH_INPUT_FILE "digraph.tmp"
#define TMP_GRAPH_FILE_LOC RESOURCES_FOLDER+TMP_GRAPH_INPUT_FILE
#define TMP_GRAPH_OUTPUT_FILE "digraph.tmp.png"
#define TMP_GRAPH_OUTPUT RESOURCES_FOLDER+TMP_GRAPH_OUTPUT_FILE
#define DESCR_AND_AUTHOR "RTS, INF. Prvided by Alessandro Secco (1041258) seccoale@dei.unipd.it"
#define TAB_INDEX_TASK 0
#define TAB_INDEX_GRAPH 1
#define TAB_INDEX_SCHEDULE 2
#define PLAY_TEXT "play"
#define STOP_TEXT "stop"

using namespace std;
using namespace inf;

static FordFulkersonRunner* runner;

GraphView::GraphView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphView)
{
    ui->setupUi(this);
    this->tasks_inserted=0;
    ui->centralWidget->setWindowTitle(DESCR_AND_AUTHOR);
    ui->graphicsView_graph->setSceneRect(-5000,-5000,10000,10000);
    this->currentScene=new QGraphicsScene();
    this->currentScene->setSceneRect(-10000,-100000,10000,10000);
    string first_img_loc=FIRST_IMAGE;
    setNewImage(first_img_loc);
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
    this->ui->graphicsView_graph->fitInView(this->currentPixmap);
    //ui->graphicsView_graph->setScene(this->currentScene);
    //ui->graphicsView_graph->update();
}

void GraphView::replaceImg(string loc){
    QGraphicsPixmapItem prev_img(this->currentPixmap);
    this->currentScene->setSceneRect(-10000, -10000, 10000, 10000);
    this->currentScene->removeItem(this->currentPixmap);
    QPixmap new_img(loc.c_str());
    this->currentPixmap=this->currentScene->addPixmap(new_img);
    this->currentPixmap->update();
    this->ui->graphicsView_graph->fitInView(this->currentPixmap, Qt::KeepAspectRatio);
    this->ui->graphicsView_graph->repaint();
    this->ui->graphicsView_graph->update();
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
    cout<<this->ui->listTask->count()<<endl;
    if(this->ui->listTask->count()>1){
        this->ui->import_graph_button->setEnabled(true);
    }
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
    if(this->ui->listTask->count()<2){
        this->ui->import_graph_button->setEnabled(false);
    }
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
    graph=new INFGraph(ui->graphNameLE->text().toStdString());
    this->set=new TaskSet();
    for(int i=0; i<ui->listTask->count(); i++){
        Task* newTask=new Task();
        QString literal=ui->listTask->item(i)->text();
        newTask=newTask->compile(&literal);
        set->addTask(newTask);
    }
    this->ui->frameSizes->clear();
    this->frame_sizes=INFAlgoithms::detectFrameSizes(set);
    for(unsigned int i=0; i<frame_sizes.size(); i++){
        this->ui->frameSizes->addItem(QString::number(frame_sizes.at(i)));
    }
    this->ui->hyperperiod->display(INFAlgoithms::findHyperperiod(set));
    this->current_frame=0;
    graph->importTaskSet(set, this->frame_sizes.at(this->current_frame), INFAlgoithms::findHyperperiod(set));
    drawGraph(graph->toStringComplete());
    this->ui->play_stop_button->setText(PLAY_TEXT);
    free(runner);
    ui->play_stop_button->setEnabled(true);
    ui->nextButton->setEnabled(true);
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
    this->replaceImg(new_image_spaces);
    //switches to the correct tab
    if(this->ui->tabWidget->currentIndex()!=TAB_INDEX_GRAPH){
        this->ui->tabWidget->setCurrentIndex(TAB_INDEX_GRAPH);
    }
}

void GraphView::on_play_stop_button_clicked()
{
    this->play=!this->play;
    if(this->play){
        cout<<"playing"<<endl;
        this->ui->play_stop_button->setText(STOP_TEXT);
        if(runner==NULL){
            runner=new FordFulkersonRunner(this, this->graph);
            connect(this, SIGNAL(sleepMSChanged(int)), runner, SLOT(setTimeToSleep(int)));
        }
        runner->nextStep();
        runner->setRunWhole(true);
        //this->ui->play_stop_button->setEnabled(false);
        if(!runner->isRunning()){
            runner->start();
        }
    }
    else {
        cout<<"stopping"<<endl;
        this->ui->play_stop_button->setText(PLAY_TEXT);
        //runner had already been created!
        runner->setRunWhole(false);
        //runner->suspend();
    }
}

void GraphView::on_frameSizes_activated(const QString &arg1)
{
    double frame=arg1.toDouble();
    this->graph->importTaskSet(this->set, frame, INFAlgoithms::findHyperperiod(this->set));
    this->ui->play_stop_button->setText(PLAY_TEXT);
    this->play=false;
    this->ui->play_stop_button->setEnabled(true);
    this->ui->nextButton->setEnabled(true);
    this->ui->prevButton->setEnabled(false);
    drawGraph(this->graph->toStringComplete());
}

void GraphView::endedFlowComputation(double maxFlow){
    cout<<"The computation ended with flow="<<maxFlow<<endl;
    this->ui->play_stop_button->setEnabled(false);
    QString* notification=new QString("frame size ");
    notification->append(this->ui->frameSizes->currentText());
    notification->append(" has a maximum flow of ");
    notification->append(QString::number(maxFlow));
    double totalExecutionTime=0;
    for(unsigned int i=0; i<this->graph->edges.size(); i++){
        if(this->graph->edges.at(i)->from->index==this->graph->source->index){
            totalExecutionTime+=this->graph->edges.at(i)->tmpLabel->capacity;
        }
    }
    if(maxFlow<totalExecutionTime){
        notification->append("<");
    }
    else {
        notification->append(">=");
    }
    notification->append(QString::number(totalExecutionTime));
    if(maxFlow<totalExecutionTime){
        int i=this->ui->frameSizes->currentIndex();
        if(++i<this->ui->frameSizes->count()){
            notification->append(" \nTry with frame size ");
            notification->append(this->ui->frameSizes->itemText(i));
            notification->append("!");
        }
    }
    this->currentScene->addText(*notification);
    this->ui->play_stop_button->setEnabled(false);
    this->ui->nextButton->setEnabled(false);
}
void GraphView::on_sleepMS_valueChanged(int ms)
{
    emit sleepMSChanged(ms);
}

void GraphView::on_nextButton_clicked()
{
    cout<<this->graph->toStringComplete()<<endl;
    if(runner==NULL){
        runner=new FordFulkersonRunner(this, this->graph);
        connect(this, SIGNAL(sleepMSChanged(int)), runner, SLOT(setTimeToSleep(int)));
    }
    runner->setRunWhole(false);
    this->ui->play_stop_button->setEnabled(false);
    if(!runner->isRunning()){
        runner->start();
    }
    else {
        runner->nextStep();
        //while(runner->isRunning()){
        //   QCoreApplication::processEvents();
        //}
    }
}
void GraphView::repaintRequested(){
    this->drawGraph(this->graph->toStringComplete());
}
bool GraphView::fastPlay(){
    return this->ui->fastPlayCB->isChecked();
}
