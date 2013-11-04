#include "taskdeclarationwindow.h"
#include "ui_taskdeclarationwindow.h"

TaskDeclarationWindow::TaskDeclarationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskDeclarationWindow)
{
    ui->setupUi(this);
    this->edit=false;
}

TaskDeclarationWindow::TaskDeclarationWindow(QWidget *parent, Task *TaskToEdit, int row):
    QDialog(parent),
    ui(new Ui::TaskDeclarationWindow){
    ui->setupUi(this);
    this->edit=true;
    this->row_edited=row;
    this->ui->task_name->setText(TaskToEdit->name.c_str());
    this->ui->deadlineSB->setValue(TaskToEdit->deadline);
    this->ui->exTimeSB->setValue(TaskToEdit->execution_time);
    this->ui->periodSB->setValue(TaskToEdit->period);
}

TaskDeclarationWindow::~TaskDeclarationWindow()
{
    delete ui;
}

void TaskDeclarationWindow::on_buttonBox_accepted()
{
    Task* task=new Task(ui->task_name->text().toUtf8().constData(),ui->periodSB->value(), ui->deadlineSB->value(), ui->exTimeSB->value());
    if(edit){
        emit replaceTask(task, this->row_edited);
    }
    else {
        emit addTask(task);
    }
}

void TaskDeclarationWindow::on_buttonBox_rejected()
{
    emit discardTask();
}
void TaskDeclarationWindow::editTask(QString* taskLiteral){
    Task* task=new Task();
    task=task->compile(*taskLiteral);
    QString* taskName=new QString(task->name.c_str());
    this->ui->task_name->setText(*taskName);
    delete taskName;
    this->ui->deadlineSB->setValue(task->deadline);
    this->ui->exTimeSB->setValue(task->execution_time);
    this->ui->periodSB->setValue(task->period);
    delete task;
    this->show();
}

void TaskDeclarationWindow::on_periodSB_valueChanged(double arg1)
{
    this->ui->deadlineSB->setMinimum(arg1);
}
