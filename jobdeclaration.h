#ifndef JOBDECLARATION_H
#define JOBDECLARATION_H

#include <QDialog>

namespace Ui {
class JobDeclaration;
}

class JobDeclaration : public QDialog
{
    Q_OBJECT
    
public:
    explicit JobDeclaration(QWidget *parent = 0);
    ~JobDeclaration();
    
private:
    Ui::JobDeclaration *ui;
};

#endif // JOBDECLARATION_H
