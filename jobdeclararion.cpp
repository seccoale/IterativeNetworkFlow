#include "jobdeclaration.h"
#include "ui_jobdeclaration.h"

JobDeclaration::JobDeclaration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JobDeclaration)
{
    ui->setupUi(this);
}

JobDeclaration::~JobDeclaration()
{
    delete ui;
}
