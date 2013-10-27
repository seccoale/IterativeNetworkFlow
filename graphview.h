#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QMainWindow>
#include <string>
#include <QGraphicsPixmapItem>
#include <QStack>
#include <QFrame>
#include <QGraphicsScene>
#include "jobdeclarationwindow.h"
#include "Structs.h"
#include <QListWidgetItem>

namespace Ui {
class GraphView;
}

using namespace inf;
class GraphView : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GraphView(QWidget *parent = 0);
    ~GraphView();
private slots:
    void on_add_job_button_clicked();
    void job_received(Job* job);
    void job_discarded();

    void on_listJob_itemClicked(QListWidgetItem *item);

    void on_editJobBtn_clicked();

    void on_removeBtn_clicked();

private:
    int jobs_inserted;
    Ui::GraphView *ui;
    QGraphicsScene* currentScene;
    QGraphicsPixmapItem* currentPixmap;
};

#endif // GRAPHVIEW_H

