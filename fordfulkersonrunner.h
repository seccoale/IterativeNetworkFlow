#ifndef FORDFULKERSONRUNNER_H
#define FORDFULKERSONRUNNER_H
#include "Structs.h"
#include "infalgoithms.h"
#include <semaphore.h>
#include "graphview.h"
#include <unistd.h>
#include <QThread>
class FordFulkersonRunner:public QThread
{
    Q_OBJECT
public:
    explicit FordFulkersonRunner(GraphView* view, INFGraph* graph);
    void setFrame(double newFrame);
    void nextStep();
    void setGraph(INFGraph* newGraph);
    void run();
    void setRunWhole(bool whole);
    static void* pthreadRunAlgorithm(void* param);
    bool isRunning();
    void suspend();
public slots:
signals:
    void ended(double maxFlow);
    void repaint();
private:
    void *runAlgorithm();
    //vector<int> pred;
    //vector<double> epsilon;
    //double TIME_TO_SLEEP;
    bool running;
    sem_t* semaphore;
    bool runWhole;
    GraphView* view;
    double currentFrame;
    vector<double> frames;
    double hyperPeriod;
    INFGraph* graph;
    void increaseLabels(GraphElement* elem);
    /**
     * @brief queue Set of labeled but not processed vertexes in labeling phase
     */
    QSet<Vertex*>* queue;
    void step();

};

#endif // FORDFULKERSONRUNNER_H
