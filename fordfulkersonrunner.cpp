#include "fordfulkersonrunner.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#define TIME_TO_SLEEP 1
#define INFINITE 99999
FordFulkersonRunner::FordFulkersonRunner(GraphView *view, INFGraph *graph) : QObject()
{
    this->semaphore=new sem_t();
    this->queue=new QSet<Vertex*>();
    this->view=view;
    this->graph=graph;
    sem_init(semaphore, 1, 0);
    this->running=false;
    QObject::connect(this, SIGNAL(ended(double)), this->view, SLOT(endedFlowComputation(double)));
}
void FordFulkersonRunner::step(){
    cout<<"STEP\n\n\n\n\n\n\n\n\n\n"<<endl;
    view->drawGraph(this->graph->toStringComplete());
    if(runWhole){
        sleep(TIME_TO_SLEEP);
    }
    else {
        sem_wait(this->semaphore);
    }
}
void FordFulkersonRunner::nextStep(){
    sem_post(this->semaphore);
}
void FordFulkersonRunner::increaseLabels(GraphElement *elem){
    elem->setLevelLabel(++level_labeling);

}

void FordFulkersonRunner::run(bool whole){
    if(FordFulkersonRunner::running){
        cerr<<"AN INSTANCE OF FORD FULKERSON IS ALEADY RUNNING! MULTIPLE INSTANCES ARE NOT ALLOWED!"<<endl;
        throw new exception();
    }
    else {
        //int levelLabel=0;
        running=true; //do not accept other instances
        cout<<this->graph->toStringComplete()<<endl;
        cout<<this->graph->vertexes.size()<<endl;
        int pred[this->graph->vertexes.size()];
        double epsilon[this->graph->vertexes.size()];
        double delta=0;
        double flow=0;
        Edge* edge;
        cout<<"initial flow is 0"<<endl;
        for(unsigned int i=0; i< graph->edges.size(); i++){//initial flow is 0
            edge=graph->edges.at(i);
            edge->tmpLabel->tmpFlow=0;
        }
        cout<<"Main cycle Starts"<<endl;
        do{//main cycle
            cout<<"pred is source or each vertex!"<<endl;
            for(int i=0; i<graph->vertexes.size(); i++){
                if(i!=graph->source->index){
                    pred[i]=this->graph->source->index;
                }
            }
            epsilon[this->graph->source->index]=INFINITE;
            pred[this->graph->source->index]=this->graph->source->index;
            cout<<" pred["<<this->graph->source->index<<"="<<this->graph->source->index;
            this->queue->insert(this->graph->source);
            cout<<"cycle inside the queue"<<endl;
            while(!this->queue->isEmpty()){
                int selectedIndex=-1;
                cout<<"selects first vertex in the queue"<<endl;
                for(int i=0; i<this->graph->vertexes.size(); i++){
                    if(this->queue->contains(this->graph->vertexes.at(i))){
                        selectedIndex=i;
                    }
                }// selected index is the least index of the vertexes in the queue
                cout<<"SELECTED VERTEX INDEX "<<selectedIndex<<endl;
                Vertex* selectedVertex=this->graph->vertexes.at(selectedIndex);
                increaseLabels(selectedVertex);
                this->queue->remove(selectedVertex);
                step();
                Edge* edge;
                cout<<"searching for increasing path & labeling"<<endl;
                cout<<selectedVertex->toVertexes.size()<<endl;
                for(int i=0; i<selectedVertex->toVertexes.size(); i++){
                    cout<<"directed from "<<selectedVertex->toVertexes.at(i)<<endl;
                    int toIndex=selectedVertex->toVertexes.at(i)->index;
                    edge=this->graph->edges.at(this->graph->edgeIndex(selectedVertex, selectedVertex->toVertexes.at(i)));
                    if(edge->tmpLabel->tmpFlow<edge->tmpLabel->capacity){
                        cout<<"edge->tmpLabel->tmpFlow<edge->tmpLabel->capacity"<<endl;
                        if(pred[toIndex]==0){
                              cout<<"pred[toIndex]==0"<<endl;
                            epsilon[toIndex]=min(epsilon[selectedIndex], edge->tmpLabel->capacity-edge->tmpLabel->tmpFlow);
                            pred[toIndex]=selectedIndex;
                            this->queue->insert(selectedVertex->toVertexes.at(i));
                            selectedVertex->toVertexes.at(i)->setLevelLabel(level_labeling);
                            increaseLabels(edge);
                            edge->from->setLevelLabel(level_labeling);
                            edge->to->setLevelLabel(level_labeling);
                            step();
                        }
                    }
                }
                cout<<selectedVertex->fromVertexes.size()<<endl;
                for(int i=0; i<selectedVertex->fromVertexes.size(); i++){
                    cout<<"inverse to "<<selectedVertex->fromVertexes.at(i)<<endl;
                    int fromIndex=selectedVertex->fromVertexes.at(i)->index;
                    edge=this->graph->edges.at(this->graph->edgeIndex(selectedVertex->fromVertexes.at(i), selectedVertex));
                    if(edge->tmpLabel->tmpFlow>0){
                        cout<<"edge->tmpLabel->tmpFlow>0"<<endl;
                        if(pred[fromIndex]==0){
                            cout<<"pred[fromIndex]==0"<<endl;
                            epsilon[fromIndex]=min(epsilon[selectedIndex], edge->tmpLabel->tmpFlow);
                            pred[fromIndex]=-selectedIndex;
                            this->queue->insert(selectedVertex->fromVertexes.at(i));
                            increaseLabels(edge);
                            edge->from->setLevelLabel(level_labeling);
                            edge->to->setLevelLabel(level_labeling);
                            step();
                        }
                    }
                }
            }
            if(pred[this->graph->sink->index]!=0){// found growing path
                cout<<"reconstructing growing path"<<endl;
                delta=epsilon[this->graph->sink->index];
                flow+=delta;
                int predPath=this->graph->sink->index;
                increaseLabels(this->graph->sink);
                while(predPath!=this->graph->source->index){//updating labels
                    cout<<"UPDATE"<<endl<<flush;
                    int tmp=pred[predPath];
                    if(tmp>=0){
                        Edge* edge=this->graph->edges.at(this->graph->edgeIndex(this->graph->vertexes.at(tmp), this->graph->vertexes.at(predPath)));
                        edge->tmpLabel->tmpFlow+=delta;
                        edge->setLevelLabel(level_labeling);
                        edge->from->setLevelLabel(level_labeling);
                        edge->to->setLevelLabel(level_labeling);
                    }
                    else {
                        Edge* edge=this->graph->edges.at(this->graph->edgeIndex(this->graph->vertexes.at(predPath), this->graph->vertexes.at(-tmp)));
                        edge->tmpLabel->tmpFlow-=delta;
                        edge->setLevelLabel(level_labeling);
                        edge->from->setLevelLabel(level_labeling);
                        edge->to->setLevelLabel(level_labeling);
                    }
                    predPath=abs(tmp);
                }
                step();
            }
        }
        while(pred[graph->sink->index]!=0);
        step();
        cout<<"ENDED ALGORITHM"<<endl;
        this->running=false;
        level_labeling=0;
        emit ended(flow);
    }
}
