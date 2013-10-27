#ifndef JOBDECLARATIONWINDOW_H
#define JOBDECLARATIONWINDOW_H

#include <QDialog>
#include "graphview.h"
#include "Structs.h"
namespace Ui {
class JobDeclarationWindow;
}
using namespace inf;
class JobDeclarationWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit JobDeclarationWindow(QWidget *parent = 0);
    ~JobDeclarationWindow();

signals:
    void addJob(Job*);
    void discardJob();
    
private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();
    void editJob(QString* jobLiteral, int row);

private:
    Ui::JobDeclarationWindow *ui;
};

#endif // JOBDECLARATIONWINDOW_H
