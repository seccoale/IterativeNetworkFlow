#ifndef TASKDECLARATIONWINDOW_H
#define TASKDECLARATIONWINDOW_H

#include <QDialog>
#include "graphview.h"
#include "Structs.h"
namespace Ui {
class TaskDeclarationWindow;
}
using namespace inf;
class TaskDeclarationWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit TaskDeclarationWindow(QWidget *parent = 0);
    TaskDeclarationWindow(QWidget *parent, Task* taskToEdit,int row);
    ~TaskDeclarationWindow();

signals:
    void addTask(Task*);
    void discardTask();
    void replaceTask(Task*, int);
    
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void editTask(QString* taskLiteral);

    void on_periodSB_valueChanged(double arg1);

private:
    Ui::TaskDeclarationWindow *ui;
    bool edit;
    int row_edited;
};

#endif // TASKDECLARATIONWINDOW_H
