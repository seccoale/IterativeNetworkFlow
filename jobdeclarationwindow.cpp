#include "jobdeclarationwindow.h"
#include "ui_jobdeclarationwindow.h"

JobDeclarationWindow::JobDeclarationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JobDeclarationWindow)
{
    ui->setupUi(this);
}

JobDeclarationWindow::~JobDeclarationWindow()
{
    delete ui;
}

void JobDeclarationWindow::on_buttonBox_accepted()
{
    Job* job=new Job(ui->job_name->text().toUtf8().constData(),ui->releaseTimeSB->value(), ui->deadlineSB->value(), ui->exTimeSB->value());
    emit addJob(job);
}

void JobDeclarationWindow::on_buttonBox_rejected()
{
    emit discardJob();
}
void JobDeclarationWindow::editJob(QString* jobLiteral, int row){
    Job* job=new Job();
    job=job->compile(*jobLiteral);
    QString* jobName=new QString(job->name.c_str());
    this->ui->job_name->setText(*jobName);
    delete jobName;
    this->ui->deadlineSB->setValue(job->deadline);
    this->ui->exTimeSB->setValue(job->execution_time);
    this->ui->releaseTimeSB->setValue(job->release);
    delete job;
    this->show();
}
