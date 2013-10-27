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
    this->jobs_inserted=0;
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

void GraphView::on_add_job_button_clicked()
{
    ui->add_job_button->setEnabled(false);
    JobDeclarationWindow* jobDecl=new JobDeclarationWindow(this);
    connect(jobDecl, SIGNAL(addJob(Job*)), this, SLOT(job_received(Job*)));
    connect(jobDecl, SIGNAL(discardJob()), this, SLOT(job_discarded()));
    jobDecl->show();
}
void GraphView::job_received(Job * job){
    ui->add_job_button->setEnabled(true);
    cout<<job->toString()->toUtf8().constData()<<endl;
    this->ui->listJob->insertItem(++jobs_inserted, job->toString()->toUtf8().constData());
}
void GraphView::job_discarded(){
    ui->add_job_button->setEnabled(true);
}

void GraphView::on_listJob_itemClicked(QListWidgetItem *item)
{
   ui->editJobBtn->setEnabled(true);
   ui->removeBtn->setEnabled(true);
}
void GraphView::on_editJobBtn_clicked()
{
    QString selectedJob=this->ui->listJob->selectedItems().front()->text();
    Job* job=new Job();
    job=job->compile(selectedJob);
    JobDeclarationWindow* jobEditWindow=new JobDeclarationWindow(this);
}
void GraphView::on_removeBtn_clicked()
{
    QListWidgetItem* selected=this->ui->listJob->currentItem();
    this->ui->listJob->removeItemWidget(selected);
    delete selected;
    if(this->ui->listJob->selectedItems().isEmpty()){
        this->ui->editJobBtn->setEnabled(false);
        this->ui->removeBtn->setEnabled(false);
    }
}
