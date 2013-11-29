#include "graphview.h"
#include "ui_graphview.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <QDir>
#include <unistd.h>
#include <infalgoithms.h>
#include <QFileDialog>
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
#define PEN_BRUSH_WIDTH 20

using namespace std;
using namespace inf;

static FordFulkersonRunner* runner;
static int schedHeight;
static int schedWidth;
static int schedStartX;
static int schedStartY;
static int schedEndX;
static int schedEndY;
static int frameWidth;
static int* framePad;

GraphView::GraphView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphView)
{
    this->history=new QStack<QGraphicsPixmapItem*>();
    this->forward=new QStack<QGraphicsPixmapItem*>();
    this->scheduleItems=new vector<QGraphicsItem*>();
    ui->setupUi(this);
    this->tasks_inserted=0;
    ui->centralWidget->setWindowTitle(DESCR_AND_AUTHOR);
    ui->graphicsView_graph->setSceneRect(-50000,-50000,100000,100000);
    ui->graphicsView_schedule->setSceneRect(-5000, -5000, 10000, 10000);
    this->currentScene=new QGraphicsScene();
    this->schedScene=new QGraphicsScene();
    this->currentScene->setSceneRect(-10000,-100000,10000,10000);
    this->schedScene->setSceneRect(-10000,-100000,10000,10000);
    string first_img_loc=FIRST_IMAGE;
    setNewImage(first_img_loc);
    ui->graphicsView_graph->setScene(this->currentScene);
    ui->graphicsView_schedule->setScene(this->schedScene);
}

GraphView::~GraphView()
{
    delete ui;
}


void GraphView::setNewImage(string loc){
    QPixmap img(loc.c_str());
    this->currentPixmap=this->currentScene->addPixmap(img);
    this->currentScene->addPixmap(img);
    this->ui->graphicsView_graph->fitInView(this->currentPixmap, Qt::KeepAspectRatio);
    cout<<"history: "<<this->history->size()<<"; forward: "<<this->forward->size()<<endl;
    //ui->graphicsView_graph->setScene(this->currentScene);
    //ui->graphicsView_graph->update();
}

void GraphView::replaceImg(string loc){
    QGraphicsPixmapItem* prev_img=new QGraphicsPixmapItem(this->currentPixmap);
    this->currentScene->setSceneRect(-10000, -10000, 10000, 10000);
    this->currentScene->removeItem(this->currentPixmap);
    QPixmap* new_img=new QPixmap(loc.c_str());
    this->currentPixmap=this->currentScene->addPixmap(*new_img);
    this->currentPixmap->update();
    this->ui->graphicsView_graph->fitInView(this->currentPixmap, Qt::KeepAspectRatio);
    this->ui->graphicsView_graph->repaint();
    this->ui->graphicsView_graph->update();
    this->history->push_back(new QGraphicsPixmapItem(prev_img));
    cout<<"history: "<<this->history->size()<<"; forward: "<<this->forward->size()<<endl;
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
    this->tasks_inserted=this->set->size();
    this->ui->frameSizes->clear();
    this->frame_sizes=INFAlgoithms::detectFrameSizes(set);
    for(unsigned int i=0; i<frame_sizes.size(); i++){
        this->ui->frameSizes->addItem(QString::number(frame_sizes.at(i)));
    }
    this->ui->hyperperiod->display(INFAlgoithms::findHyperperiod(set));
    this->frame=this->ui->frameSizes->currentText().toDouble();
    graph->importTaskSet(set, frame, INFAlgoithms::findHyperperiod(set));
    drawGraph(graph->toStringComplete());
    this->ui->play_stop_button->setText(PLAY_TEXT);
    free(runner);
    ui->play_stop_button->setEnabled(true);
    ui->nextButton->setEnabled(true);
    this->initGantt();
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
    this->drawSchedule();
    //switches to the correct tab
    if(this->ui->tabWidget->currentIndex()==TAB_INDEX_TASK){
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
            //connect(this, SIGNAL(sleepMSChanged(int)), runner, SLOT(setTimeToSleep(int)));
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
    this->ui->prevButton->setEnabled(true);
}

void GraphView::on_frameSizes_activated(const QString &arg1)
{
    this->frame=arg1.toDouble();
    this->graph->importTaskSet(this->set, frame, INFAlgoithms::findHyperperiod(this->set));
    this->ui->play_stop_button->setText(PLAY_TEXT);
    this->play=false;
    this->ui->play_stop_button->setEnabled(true);
    this->ui->nextButton->setEnabled(true);
    this->ui->prevButton->setEnabled(false);
    drawGraph(this->graph->toStringComplete());
    initGantt();
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
    else {
        notification->append("\nSchedulable with Frame Size = ");
        notification->append(QString::number(frame));
        notification->append("!!!");
    }
    this->currentScene->addText(*notification);
    this->schedScene->addText(*notification)->setPos(schedStartX-50, schedStartY-50);
    this->ui->play_stop_button->setEnabled(false);
    this->ui->nextButton->setEnabled(false);
}
//void GraphView::on_sleepMS_valueChanged(int ms)
//{
//    emit sleepMSChanged(ms);
//}

void GraphView::on_nextButton_clicked()
{
    cout<<this->graph->toStringComplete()<<endl;
    //this->ui->play_stop_button->setEnabled(false);
    if(this->forward->isEmpty()){
        if(runner==NULL){
            runner=new FordFulkersonRunner(this, this->graph);
            //   connect(this, SIGNAL(sleepMSChanged(int)), runner, SLOT(setTimeToSleep(int)));
        }
        runner->setRunWhole(false);
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
    else{
        this->history->push_back(this->currentPixmap);
        this->currentScene->removeItem(this->currentPixmap);
        this->currentPixmap=this->forward->pop();
        this->currentScene->addPixmap(this->currentPixmap->pixmap());
        this->ui->graphicsView_graph->repaint();
    }
    this->ui->prevButton->setEnabled(true);
}
void GraphView::repaintRequested(){
    this->drawGraph(this->graph->toStringComplete());
}
bool GraphView::fastPlay(){
    return this->ui->fastPlayCB->isChecked();
}
void GraphView::on_prevButton_clicked()
{
    cout<<"history: "<<this->history->size()<<"; forward: "<<this->forward->size()<<endl;
    this->forward->push_back(this->currentPixmap);
    this->currentScene->removeItem(this->currentPixmap);
    this->currentPixmap=this->history->pop();
    this->currentScene->addPixmap(this->currentPixmap->pixmap());
    this->ui->graphicsView_graph->repaint();
    if(this->history->isEmpty()){
        this->ui->prevButton->setEnabled(false);
    }
    else {
        this->ui->prevButton->setEnabled(true);
    }
}
void GraphView::on_saveGraphBTN_clicked()
{
    if(this->play){
        on_play_stop_button_clicked();
    }
    QFileDialog* fileDialog=new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setModal(true);
    string location=RESOURCES_FOLDER+TMP_GRAPH_OUTPUT_FILE;
    QString* loc_no_spaces=new QString(location.c_str());
    *loc_no_spaces=loc_no_spaces->replace(*new QString(" "),*new QString("\\ "));
    cout<<loc_no_spaces->toStdString()<<endl;
    string command="cp "+loc_no_spaces->toStdString()+" "+fileDialog->getSaveFileName().toStdString()+".png";
    cout<<command<<endl;
    system(command.c_str());
}
void GraphView::drawSchedule(){
    cout<<"DRAW SCHEDULE"<<endl;
    for(unsigned int i=0; i<this->scheduleItems->size(); i++){
        this->schedScene->removeItem(this->scheduleItems->at(i));
    }
    this->ui->graphicsView_schedule->repaint();
    int numFrame=(int)this->ui->hyperperiod->value()/frame;
    framePad=new int[numFrame];
    for(int i=0; i<numFrame; i++){
        framePad[i]=0;
    }
    FrameVertex* fv;
    QPen* pen=new QPen(*new QBrush(Qt::Dense1Pattern), PEN_BRUSH_WIDTH);
    for(unsigned int i=0; i<this->graph->frameVertexes.size(); i++){
        fv=this->graph->frameVertexes.at(i);
        for(unsigned int jvi=0; jvi<fv->fromVertexes.size(); jvi++){
            addToSchedule((JobVertex*)fv->fromVertexes.at(jvi),fv, pen);
        }
    }
}
void GraphView::addToSchedule(JobVertex *jv, FrameVertex *fv, QPen* pen){
    int edgIdx=this->graph->edgeIndex(jv,fv);
    Edge* e=this->graph->edges.at(edgIdx);
    if(e->tmpLabel->tmpFlow>0){
        QString* frameName;
        QString* jobName;
        frameName=new QString(fv->name.c_str());
        *frameName=frameName->mid(frameName->lastIndexOf("_")+1);
        jobName=new QString(jv->name.c_str());
        *jobName=jobName->mid(jobName->indexOf("_")+1);
        *jobName=jobName->mid(0,jobName->lastIndexOf("_"));
        int taskIdx;
        for(unsigned int i=0; i<this->set->size();i++){
            if(this->set->at(i)->name.compare(jobName->toStdString())==0){
                cout<<"TASKIDX IS "<<i<<": "<<this->set->at(i)->name<<"=="<<jobName->toStdString()<<endl;
                taskIdx=i;
                break;
            }
        }
        int frameIdx=frameName->toInt()-1;
        cout<<fv->name<<" : "<<frameIdx<<endl;
        cout<<jv->name<<" : "<<taskIdx<<endl;
        int r,g,b;
        r=255*taskIdx/this->set->size();
        g=255*(taskIdx+this->set->size()/2)/this->set->size();
        b=255*(taskIdx+this->set->size()/3)/this->set->size();
        cout<<"COLOR: "<<r<<","<<g<<","<<b<<endl;
        pen->setColor(*new QColor(r, g,b));
        int length=(e->tmpLabel->tmpFlow/frame)*frameWidth;
        int y=schedStartY+schedHeight*(taskIdx+1);
        int x=schedStartX+frameWidth*frameIdx/*+span[frameIdx]*/;
        cout<<"THE X IS: "<<x;
        if(x+framePad[frameIdx]>frameIdx*frameWidth){
            cout<<x<<"+"<<framePad[frameIdx]<<endl;
            cerr<<"ERROR IN SCHEDULE!"<<endl<<flush;
            //throw new exception();
        }
        cout<<x<<"+"<<framePad[frameIdx]<<endl;
        x+=framePad[frameIdx];
        framePad[frameIdx]+=length;
        this->scheduleItems->push_back(this->schedScene->addLine(x+pen->width()/2,y-pen->width(),x+length-pen->width()/2,y-pen->width(), *pen));
        this->scheduleItems->push_back(this->schedScene->addLine(x+length, y, x+length, y-schedHeight/3));
        int timeFrom=round(100*this->frame/frameWidth*(abs(schedStartX-x)));
        int timeTo=timeFrom+e->tmpLabel->tmpFlow*100;
        this->scheduleItems->push_back(this->schedScene->addText(*new QString(QString::number(timeTo/100.))));
        this->scheduleItems->back()->setPos(x+length-30,y-schedHeight*2/7);
        this->scheduleItems->push_back(this->schedScene->addLine(x, y, x, y-schedHeight/3));
        this->scheduleItems->push_back(this->schedScene->addText(*new QString(QString::number((timeFrom/100.)))));
        this->scheduleItems->back()->setPos(x,y-schedHeight*1/5);
        cout<<"Add to schedule edge "<<jv->name<<"->"<<fv->name<<endl;
    }
}

void GraphView::initGantt(){
    cout<<"Gantt Inits"<<endl;
    const int numFrames=this->ui->hyperperiod->value()/this->frame;
    delete framePad;
    framePad=new int[numFrames];
    for(int i=0; i<numFrames; i++){
        framePad[i]=0;
    }
    this->scheduleItems->clear();
    this->schedScene->clear();
    schedWidth=this->width()-200;
    schedStartY=-this->height()/3+50;
    schedHeight=(this->height()-schedStartY)/(2*tasks_inserted);
    if(schedHeight<20){//needs rolls
        schedHeight=20;
    }
    schedStartX=-this->width()/2+100;
    schedEndX=this->width()/2-100;
    schedEndY=schedStartY+schedHeight*tasks_inserted;
    QString graphName=this->ui->graphNameLE->text();
    this->schedScene->addText(graphName)->setY(schedStartY-50);
    for(int i=1; i<=this->tasks_inserted; i++){
        this->schedScene->addLine(schedStartX ,schedStartY+schedHeight*i, schedEndX, schedStartY+schedHeight*i);
        this->schedScene->addText(this->ui->listTask->item(i-1)->text())->setPos(schedStartX,schedStartY+schedHeight*i-schedHeight/2);
    }
    int width=schedEndX-schedStartX;
    frameWidth=width*frame/this->ui->hyperperiod->value();
    int i=0;
    for(double f=0; f<this->ui->hyperperiod->value()+0.01; f+=frame){
        this->schedScene->addLine(schedStartX+frameWidth*i,schedStartY,schedStartX+frameWidth*i, schedEndY);
        this->schedScene->addText(QString::number(f))->setPos(schedStartX+frameWidth*(i++),schedStartY-20);
    }
}
