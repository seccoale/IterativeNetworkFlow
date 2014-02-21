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
#define TMP_SCHED_OUTPUT_FILE "sched.tmp.png"
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
static QPoint* schedTopLeft=0;
static int schedStartX=0;
static int schedStartY=0;
static int schedEndX=0;
static int schedEndY=0;
static int frameWidth=0;
static int* framePad;
static int eggcounter=0;
static double maxFlowReached=0;
GraphView::GraphView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphView)
{
    this->historyGraph=new QStack<QPixmap>();
    this->forwardGraph=new QStack<QPixmap>();
    this->scheduleItems=new vector<QGraphicsItem*>();
    this->historySched=new QStack<QPixmap*>();
    this->forwardSched=new QStack<QPixmap*>();
    //this->historySched=new QStack<vector<QGraphicsItem*> >();
    //this->forwardSched=new QStack<vector<QGraphicsItem*> >();
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
    this->setMinimumSize(this->size());
    this->setMaximumSize(this->size());
    ui->graphicsView_graph->setScene(this->currentScene);
    ui->graphicsView_schedule->setScene(this->schedScene);
    ui->tabWidget->setCurrentIndex(TAB_INDEX_SCHEDULE);
    ui->tabWidget->setCurrentIndex(TAB_INDEX_GRAPH);
}

GraphView::~GraphView()
{
    delete ui;
}

QPixmap GraphView::schedToImage(){
   //QPixmap pixmap=this->ui->graphicsView_schedule->grab(this->ui->graphicsView_schedule->contentsRect());
     QPixmap pixmap=QPixmap::grabWidget(this->ui->graphicsView_schedule, this->ui->graphicsView_schedule->contentsRect());//this->schedScene);
     //string resources_foder_with_spaces=RESOURCES_FOLDER;
     //QString* resources_folder=new QString(resources_foder_with_spaces.c_str());
     //*resources_folder=resources_folder->replace(*new QString(" "), *new QString("\\ "));
     //string new_image_no_sapces=resources_folder->toStdString()+TMP_SCHED_OUTPUT_FILE;
     //string pathname=new_image_no_sapces;
     //cout<<"============>"<<pathname<<endl;
     //pixmap.save("/");
    return pixmap;
}

void GraphView::setNewImage(string loc){
    QPixmap img(loc.c_str());
    this->currentPixmap=this->currentScene->addPixmap(img);
    this->currentScene->addPixmap(img);
    this->ui->graphicsView_graph->fitInView(this->currentPixmap, Qt::KeepAspectRatio);
    cout<<"history: "<<this->historyGraph->size()<<"; forward: "<<this->forwardGraph->size()<<endl;
    //ui->graphicsView_graph->setScene(this->currentScene);
    //ui->graphicsView_graph->update();
}

void GraphView::replaceImg(string loc){
    this->currentScene->setSceneRect(-10000, -10000, 10000, 10000);
    //QGraphicsPixmapItem* prev_img=new QGraphicsPixmapItem(this->currentPixmap);
    if(this->currentPixmap!=NULL){
        this->currentScene->removeItem(this->currentPixmap);
    }
    QPixmap* new_img=new QPixmap(loc.c_str());
    this->currentPixmap=this->currentScene->addPixmap(*new_img);
    this->currentPixmap->update();
    this->ui->graphicsView_graph->fitInView(this->currentPixmap, Qt::KeepAspectRatio);
    this->ui->graphicsView_graph->repaint();
    this->ui->graphicsView_graph->update();
   // this->history->push_back(new QGraphicsPixmapItem(prev_img));
    cout<<"history: "<<this->historyGraph->size()<<"; forward: "<<this->forwardGraph->size()<<endl;
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
    maxFlowReached=0;
    this->historyGraph->clear();
    this->forwardGraph->clear();
    this->historySched->clear();
    this->forwardSched->clear();
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
    this->ui->hyperperiod->display(INFAlgorithms::findHyperperiod(set));
    if(this->ui->hyperperiod->value()-(int)this->ui->hyperperiod->value()>0){
        this->currentScene->clear();
        this->schedScene->clear();
        this->currentScene->addText("Hyperperiod is not integer! Fail!");
        this->schedScene->addText("Hyperperiod is not integer! Fail!");
        this->ui->graphicsView_graph->fitInView(0,0,200,200);
        this->ui->graphicsView_schedule->fitInView(0,0,200,200);
        return;
    }
    this->frame_sizes=INFAlgorithms::detectFrameSizes(set);
    for(unsigned int i=0; i<frame_sizes.size(); i++){
        this->ui->frameSizes->addItem(QString::number(frame_sizes.at(i)));
    }
    this->frame=this->ui->frameSizes->currentText().toDouble();
    graph->importTaskSet(set, frame, INFAlgorithms::findHyperperiod(set));
    drawGraph(graph->toStringComplete());
    this->ui->play_stop_button->setText(PLAY_TEXT);
    //free(runner);
    ui->play_stop_button->setEnabled(true);
    ui->nextButton->setEnabled(true);
    this->initGantt();
    free(runner);
    runner=new FordFulkersonRunner(this, this->graph);
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
        this->ui->tabWidget->setCurrentIndex(TAB_INDEX_SCHEDULE);//this step is a plus which initialize correctly the schedule tab
        this->ui->tabWidget->setCurrentIndex(TAB_INDEX_GRAPH);
    }
}

void GraphView::on_play_stop_button_clicked()
{
    cerr<<"MUST CATCH:"<<eggcounter<<endl;
    if(eggcounter==8){
        cerr<<"EASTER EGG LAUNCHING!"<<endl;
        system("sensible-browser http://cristgaming.com/pirate.swf");
        eggcounter=0;
    }
    else{
        eggcounter=0;
        this->play=!this->play;
        if(this->play){
            cout<<"playing"<<endl;
            this->ui->play_stop_button->setText(STOP_TEXT);
            if(runner==NULL){
                runner=new FordFulkersonRunner(this, this->graph);
                //connect(this, SIGNAL(sleepMSChanged(int)), runner, SLOT(setTimeToSleep(int)));
            }/*
        else {
            free(runner);
            runner=new FordFulkersonRunner(this, this->graph);
            //runner->setGraph(graph);
        }*/
            runner->nextStep();
            //runner->step();
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
}

void GraphView::on_frameSizes_activated(const QString &arg1)
{
    this->frame=arg1.toDouble();
    this->graph->importTaskSet(this->set, frame, INFAlgorithms::findHyperperiod(this->set));
    this->ui->play_stop_button->setText(PLAY_TEXT);
    this->play=false;
    this->ui->play_stop_button->setEnabled(true);
    this->ui->nextButton->setEnabled(true);
    this->ui->prevButton->setEnabled(false);
    drawGraph(this->graph->toStringComplete());
    initGantt();
}

void GraphView::endedFlowComputation(double maxFlow){
    //this->history->push_back(this->currentPixmap->pixmap());
    cout<<"The computation ended with flow="<<maxFlow<<endl;
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
        else{
            notification->append(" \nNOT SCHEDULABLE!");
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
   // this->currentPixmap->setPixmap(this->forward->pop());
   // this->currentScene->addPixmap(this->currentPixmap->pixmap());
    //this->ui->graphicsView_graph->repaint();
}

void GraphView::on_nextButton_clicked()
{
    this->historyGraph->push_back(this->currentPixmap->pixmap());
    this->historySched->push_back(new QPixmap(this->schedToImage()));
    //this->historySched->push_back(*this->scheduleItems);
    cerr<<eggcounter<<endl;
    if(eggcounter<2 || (eggcounter>3 && eggcounter<6)){
        ++eggcounter;
    }
    else {
        if(eggcounter>2){
            eggcounter=0;
        }
    }
    cout<<this->graph->toStringComplete()<<endl;
    //this->ui->play_stop_button->setEnabled(false);
    if(this->forwardGraph->isEmpty()){
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
        }
    }
    else{
        this->currentPixmap->setPixmap(this->forwardGraph->pop());
        this->currentScene->addPixmap(this->currentPixmap->pixmap());
        //vector<QGraphicsItem*>* currentSched=new vector<QGraphicsItem*>();
        //for(unsigned int i=0; i<this->scheduleItems->size(); i++){
        //    currentSched->push_back(this->scheduleItems->at(i));
        //    this->schedScene->removeItem(this->scheduleItems->at(i));
        //}
        this->schedScene->clear();
        this->scheduleItems->clear();
        QPixmap* img=this->forwardSched->back();
        QGraphicsPixmapItem* tmpI=this->schedScene->addPixmap(*img);
        tmpI->setPos(*schedTopLeft);
        this->ui->graphicsView_schedule->fitInView(tmpI, Qt::KeepAspectRatio);
        this->forwardSched->pop_back();
        /*
        this->historySched->push_back(*currentSched);
        vector<QGraphicsItem*> nextSched=this->forwardSched->back();
        this->forwardSched->pop_back();
        for(unsigned int i=0; i<nextSched.size(); i++){
            this->schedScene->addItem(nextSched.at(i));
        }

  /*      cout<<this->historySched->back().size()<<endl;
        this->historySched->pop().swap(*this->scheduleItems);
        cout<<this->scheduleItems->size()<<endl;
        for(unsigned int i=0; i<this->scheduleItems->size(); i++){
            this->schedScene->addItem(this->scheduleItems->at(i));
        }
*/
        if(maxFlowReached>0){
            endedFlowComputation(maxFlowReached);
        }
        this->ui->graphicsView_graph->repaint();
        if(forwardGraph->empty() && !runner->isRunning()){
            this->ui->nextButton->setEnabled(false);
        }
    }
    //this->schedToImage()->save("/home/seccoale/Scrivania/tmpSched.png");
    this->ui->prevButton->setEnabled(true);
}
void GraphView::repaintRequested(){
    this->drawGraph(this->graph->toStringComplete());
    //this->historySched->push_back(new QPixmap(this->schedToImage()));
    this->drawSchedule();
}
bool GraphView::fastPlay(){
    return this->ui->fastPlayCB->isChecked();
}
void GraphView::on_prevButton_clicked()
{
    this->ui->nextButton->setEnabled(true);
    cerr<<eggcounter<<"|"<<(eggcounter>2 && eggcounter<5)<<endl;
    if((eggcounter>1 && eggcounter<4) || (eggcounter>5 && eggcounter<8)){
        ++eggcounter;
    }
    else{
        eggcounter=0;
    }
    cout<<"history: "<<this->historyGraph->size()<<"; forward: "<<this->forwardGraph->size()<<endl;

    this->forwardGraph->push_back(this->currentPixmap->pixmap());
    this->forwardSched->push_back(new QPixmap(this->schedToImage()));
    //this->forwardSched->push_back(*this->scheduleItems);
    this->currentPixmap->setPixmap(this->historyGraph->pop());
    this->currentScene->addPixmap(this->currentPixmap->pixmap());

    QPixmap* img=this->historySched->back();
    this->historySched->pop_back();
    this->schedScene->clear();
    this->scheduleItems->clear();
    QGraphicsPixmapItem* tmpI=this->schedScene->addPixmap(*img);
    tmpI->setPos(*schedTopLeft);
    this->ui->graphicsView_schedule->fitInView(tmpI, Qt::KeepAspectRatio);
    /*
    vector<QGraphicsItem*>* currentSched=new vector<QGraphicsItem*>();
    for(unsigned int i=0; i<this->scheduleItems->size(); i++){
        currentSched->push_back(this->scheduleItems->at(i));
    }
    for(;this->scheduleItems->size()>0;this->scheduleItems->pop_back()){
        this->schedScene->removeItem(this->scheduleItems->back());
        sleep(0.5);
    }
    this->forwardSched->push_back(*currentSched);
    vector<QGraphicsItem*> prevSched=this->historySched->back();
    this->historySched->pop_back();
    for(unsigned int i=0;i<prevSched.size(); i++){
        this->schedScene->addItem(prevSched.at(i));
    }

  /*  for(unsigned int i=0; i<this->scheduleItems->size(); i++){
        cout<<"----->removed item "<<i<<" from scene"<<endl;
        this->schedScene->removeItem(this->scheduleItems->at(i));
    }
    this->forwardSched->push_back(*this->scheduleItems);
    cout<<this->forwardSched->back().size()<<endl;
    this->historySched->pop().swap(*this->scheduleItems);
    cout<<this->scheduleItems->size()<<endl;
    for(unsigned int i=0; i<this->scheduleItems->size(); i++){
        cout<<"----->added item "<<i<<" from scene"<<endl;
        this->schedScene->addItem(this->scheduleItems->at(i));
    }*/

    if(maxFlowReached>0){
        endedFlowComputation(maxFlowReached);
    }
    this->ui->graphicsView_graph->repaint();
    if(this->historyGraph->isEmpty()){
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
void GraphView::on_saveSchedBTN_clicked()
{
    if(this->play){
        on_play_stop_button_clicked();
    }
    QFileDialog* fileDialog=new QFileDialog(this);
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setModal(true);
    QString* pathname=new QString(fileDialog->getSaveFileName());
    if(!pathname->endsWith(".png")){
        if(pathname->contains('.')){
            pathname->truncate(pathname->lastIndexOf("."));
        }
        pathname->append(".png");
    }
    this->schedToImage().save(*pathname);
}
int lastBusyTime=0;
void GraphView::drawSchedule(){
    lastBusyTime=0;
    this->scheduleItems->clear();
    this->schedScene->clear();//removeItem(this->schedScene->);
    /*for(unsigned int i=0; i<this->scheduleItems->size(); i++){
        this->schedScene->removeItem(this->scheduleItems->at(i));
    }*/
    cout<<"DRAW SCHEDULE: "<<this->scheduleItems->size()<<endl;
//    if(this->scheduleItems->empty()){
        this->initGantt();
  //  }
  //  if(this->historySched->empty() && this->forwardSched->empty()){
  //      cout<<"NOTHING TO DRAW! EXIT."<<endl;
  //      return;
  //  }
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
    QPixmap sched=this->schedToImage();
    this->schedScene->clear();
    QGraphicsPixmapItem* imgI=this->schedScene->addPixmap(sched);
    imgI->setPos(*schedTopLeft);
    this->ui->graphicsView_schedule->fitInView(imgI, Qt::KeepAspectRatio);
}
void GraphView::addToSchedule(JobVertex *jv, FrameVertex *fv, QPen* pen){
    QPen* textPen=new QPen();
    textPen->setWidth(15);
    textPen->setColor(*new QColor("white"));
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
        int r,g,b,s;
        s=253/(this->set->size()-1);
        r=(s*taskIdx)%153+102;
        g=(s*taskIdx*2)%153+102;
        b=(s*taskIdx*3)%153+102;
        cout<<"COLOR: "<<r<<","<<g<<","<<b<<endl;
        pen->setColor(*new QColor(r, g,b, 100));
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
        int timeFrom=round(100*this->frame/frameWidth*(abs(schedStartX-x)));
        if(timeFrom<lastBusyTime){
            timeFrom=lastBusyTime;
        }
        int timeTo=timeFrom+e->tmpLabel->tmpFlow*100;
        lastBusyTime=timeTo;
        QString timeToS=QString::number(timeTo/100);
        this->scheduleItems->push_back(this->schedScene->addLine(x+length+textPen->width()/2+2,y-textPen->width()/2-2,x+length+textPen->width()/2+timeToS.length()*2,y-textPen->width()/2-2,*textPen));
        this->scheduleItems->push_back(this->schedScene->addLine(x+pen->width()/2,y-pen->width(),x+length-pen->width()/2,y-pen->width(), *pen));
        this->scheduleItems->push_back(this->schedScene->addLine(x+length, y, x+length, y-schedHeight/3));

        this->scheduleItems->push_back(this->schedScene->addText(*new QString(QString::number(timeTo/100.))));
        this->scheduleItems->back()->setPos(x+length/*-30*/,y-pen->width()/*-2*pen->width()-25*/);
        this->scheduleItems->push_back(this->schedScene->addLine(x, y, x, y-schedHeight/3));
        //this->scheduleItems->push_back(this->schedScene->addText(*new QString(QString::number((timeFrom/100.)))));
        //this->scheduleItems->back()->setPos(x-this->scheduleItems->back()->boundingRect().width(),y-pen->width()/*-2*pen->width()-10/*1/5*/);
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
    delete schedTopLeft;
    //this->resizeSchedule();
    schedWidth=this->width()-200;
    schedStartY=-this->height()/3+50;
    schedHeight=(this->height()-schedStartY)/(2*tasks_inserted);
    if(schedHeight<20){//needs rolls
        schedHeight=20;
    }
    schedStartX=-this->width()/2+144;
    schedTopLeft=new QPoint(-this->width()/2+34,-this->height()/3-28);
    schedEndX=this->width()/2-56;
    schedEndY=schedStartY+schedHeight*tasks_inserted;
    QString graphName=this->ui->graphNameLE->text();
    this->schedScene->addText(graphName)->setY(schedStartY-50);
    for(int i=1; i<=this->tasks_inserted; i++){
        this->schedScene->addLine(schedStartX ,schedStartY+schedHeight*i, schedEndX, schedStartY+schedHeight*i);
        this->schedScene->addText(this->ui->listTask->item(i-1)->text())->setPos(schedStartX-this->ui->listTask->item(i-1)->text().length()*7,schedStartY+schedHeight*i-schedHeight/2);
    }
    int width=schedEndX-schedStartX;
    frameWidth=width*frame/this->ui->hyperperiod->value();
    int i=0;
    for(double f=0; f<this->ui->hyperperiod->value()+0.01; f+=frame){
        this->schedScene->addLine(schedStartX+frameWidth*i,schedStartY,schedStartX+frameWidth*i, schedEndY);
        this->schedScene->addText(QString::number(f))->setPos(schedStartX+frameWidth*(i++),schedStartY-20);
    }
}
void GraphView::resizeSchedule(){
    QRect* scheduleRect=new QRect(this->ui->graphicsView_schedule->contentsRect());
    cout<<scheduleRect->width()<<endl;
    delete schedTopLeft;
    schedWidth=scheduleRect->width()-200;
    schedStartY=-scheduleRect->height()/3+50;
    schedHeight=0;
    if(tasks_inserted>0){
        schedHeight=(scheduleRect->height()-schedStartY)/(2*tasks_inserted);
    }
    if(schedHeight<20){//needs rolls
        schedHeight=20;
    }
    schedStartX=scheduleRect->width()/2+144;
    cout<<schedStartX<<endl;
    schedTopLeft=new QPoint(-scheduleRect->width()/2+34,-scheduleRect->height()/3-28);
    schedEndX=scheduleRect->width()/2-56;
    schedEndY=schedStartY+schedHeight*tasks_inserted;
}
