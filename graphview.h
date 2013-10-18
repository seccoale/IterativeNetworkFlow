#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include <QMainWindow>

namespace Ui {
class GraphView;
}

class GraphView : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GraphView(QWidget *parent = 0);
    ~GraphView();
    
private:
    Ui::GraphView *ui;
};

#endif // GRAPHVIEW_H
