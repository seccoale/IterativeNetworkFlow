#include "graphview.h"
#include "ui_graphview.h"

GraphView::GraphView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GraphView)
{
    ui->setupUi(this);
}

GraphView::~GraphView()
{
    delete ui;
}
