#ifndef FORDFULKERSONRUNNER_H
#define FORDFULKERSONRUNNER_H
#include "Structs.h"
#include "infalgoithms.h"
#include <semaphore.h>
#include <pthread.h>
#include "graphview.h"
#include <unistd.h>
class FordFulkersonRunner:public QObject
{
    Q_OBJECT
public:
    FordFulkersonRunner(GraphView* view, INFGraph* graph);
    void setFrame(double newFrame);
    void run(bool whole);
    void nextStep();
    void setGraph(INFGraph* newGraph);
signals:
    void ended(double maxFlow);
private:
    //vector<int> pred;
    //vector<double> epsilon;
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
