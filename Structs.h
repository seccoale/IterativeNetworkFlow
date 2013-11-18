#ifndef STRUCTS_H
#define STRUCTS_H
#define MAX_FLOW 9999
#define NO_VERTEX -1
#include <vector>
#include <string>
#include <algorithm>
#include <QString>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <cmath>
#include <pthread.h>
using namespace std;

namespace inf{
static int level_labeling=0;
struct GraphElement{
    int step=-1;//Nothing in the first graph has to be marked as changed!
    void setLevelLabel(int newLevel){
        if(newLevel<level_labeling){
            cerr<<"This labeling level is not possible: NewLevel="<<newLevel<<" < "<<level_labeling<<"=CurrentLevel"<<endl;
            throw new exception();
        }
        this->step=newLevel;
        if(this->step>=level_labeling){
            level_labeling=this->step;
        }
    }
};

struct Task{
    string name;
    double period;
    double deadline;
    double execution_time;
    Task(){
        this->deadline=0;
        this->name="NONAME";
    }
    Task(string NAME, double PERIOD, double DEADLINE, double EX_TIME){
        this->name=NAME;
        this->period=PERIOD;
        this->execution_time=EX_TIME;
        this->deadline=DEADLINE;
    }
    string toString(){
        string toReturn=this->name+" ("+QString::number(this->period).toStdString()+", "+QString::number(this->execution_time).toStdString()+", "+QString::number(this->deadline).toStdString()+");";
        return toReturn;
    }
    static Task* compile(QString* literal){
        Task* job=new Task();
        int endName=literal->indexOf('(')-1;
        job->name=literal->mid(0,endName).toStdString();
        int endP=literal->indexOf(',', endName+2)-1;
        job->period=literal->mid(endName+2, endP-endName-1).toDouble();
        int endEx=literal->indexOf(',',endP+2);
        job->execution_time=literal->mid(endP+3,endEx-endP-3).toDouble();
        int endD=literal->indexOf(')')-1;
        job->deadline=literal->mid(endEx+2, endD-endEx-1).toDouble();
        return job;
    }
};

struct TaskSet{
    vector<Task*> tasks;
    void addTask(Task* task){
        this->tasks.push_back(task);
    }
    int size(){
        return this->tasks.size();
    }
    Task* at(int i){
        return this->tasks.at(i);
    }
};

struct VertexLabel:GraphElement{
    /** flow indication
         * @brief flow
         */
    int flow;
    /** from which vertex the flow come from
         * @brief from
         */
    int from;
};
struct EdgeLabel:GraphElement{
    /** maximum flow through this edge
         * @brief capacity
         */
    double capacity;
    /** flow which is currently passing through this edge
         * @brief tmpFlow
         */
    double tmpFlow=0;
    EdgeLabel(const double CAPACITY){this->capacity=CAPACITY;}
    void setFlow(int new_flow){
        this->tmpFlow=new_flow;
    }
};
struct Vertex:GraphElement{
    string name;
    int index;
    vector<Vertex*> toVertexes;
    vector<Vertex*> fromVertexes;
    VertexLabel* tmpLabel;
    Vertex(int index, string name){
        this->index=index;
        this->name=name;
    }

    bool equalsTo( Vertex* otherV){
        return this->index==otherV->index;
    }
};

struct JobVertex:Vertex{
    JobVertex(int index, string name): Vertex(index, name){
    }

};

struct FrameVertex:Vertex{
    FrameVertex(int index, string name): Vertex(index, name){

    }
};

struct SinkVertex:Vertex{
    SinkVertex(int index):Vertex(index, "SINK"){

    }
};

struct SourceVertex:Vertex{
    SourceVertex(int index):Vertex(index, "SOURCE"){

    }
};

struct Edge:GraphElement{
    int index;
    double capacity;
    Vertex* from;
    Vertex* to;
    EdgeLabel* tmpLabel;
    Edge(const int INDEX, const double CAPACITY,  Vertex* FROM,  Vertex* TO){
        this->index=INDEX;
        this->from=FROM;
        this->to=TO;
        this->tmpLabel=new EdgeLabel(CAPACITY);
    };
    bool compare( Edge &first,  Edge &second){
        return first.from->index<second.from->index;
    };
    bool equalsTo(Edge* otherEdge){
        return this->index==otherEdge->index;
    }

};


struct Graph{
    string name;
    /** Vertexes are represented as progessive integers
     * @brief vertexes vector of integers that represent vertexes
     */
    vector< Vertex*> vertexes;
    /** Vector of edges. edges<i>=j means the existence of an arc (i,j)
     * @brief edges
     */
    vector<Edge*> edges;
    /** contains the description of a vertex, containing tmp indication of flow & from which vertex
     * @brief vertexLabels
     */
    vector<VertexLabel*> vertexLabels;
    /** contains information about
     * @brief edgeLabels
     */
    vector<EdgeLabel*> edgeLabels;
    Graph(string NAME, vector<Edge*> EDGES, vector< Vertex*> VERTEXES){
        this->name=NAME.c_str();
        this->edges=EDGES;
        this->vertexes=VERTEXES;
    }
    Graph(const string NAME){
        this->name=NAME;
    }
    int size(){
        return this->vertexes.size();
    }

    string toString() const{
        string toReturn="digraph "+this->name+" {\n";
        //    Vertex* tmpV;
        //    for(unsigned int i=0; i<this->vertexes.size(); i++){
        //        tmpV=this->vertexes.at(i);
        //        toReturn=toReturn+tmpV->name+";\n";
        //    }
        Edge* tmpE;
        for(unsigned int i=0; i<this->edges.size(); i++){
            tmpE=this->edges.at(i);
            string tmp=tmpE->from->name+" -> "+tmpE->to->name+";\n";
            //toReturn->append(tmp.c_str());
            toReturn=toReturn+tmpE->from->name+" -> "+tmpE->to->name+";\n";
            // toReturn->append(tmpE->from->name);
            // toReturn->append(" -> ");
            // toReturn->append(tmpE->to->name);
            // toReturn->append(";\n");
        }
        delete tmpE;
        toReturn=toReturn+"}";
        return toReturn;
    }
    void addVertex( Vertex* newVertex){
        if(newVertex->index<this->size()){
            return;
        }
        if(newVertex->index!=this->size()){
            throw new exception();
        }
        this->vertexes.push_back(newVertex);
    }
    void addEdge(Edge* newEdge){
        //check if this edge exists in the graph
        for(unsigned int i=0; i<this->edges.size(); i++){
            if(newEdge->equalsTo(this->edges.at(i))){
                throw new exception();
            }
        }
        this->edges.push_back(newEdge);
        //check for new vertexes introduced adding this edge
        Vertex* from=newEdge->from;
        Vertex* to=newEdge->to;
        bool foundFrom=false;
        bool foundTo=false;
        Vertex* tmpVertex;
        for(unsigned int i=0;(!foundFrom && !foundTo) && i<this->vertexes.size(); i++){
            tmpVertex=this->vertexes.at(i);
            if(!foundFrom){
                foundFrom=tmpVertex->equalsTo(from);
                if(foundFrom){
                    tmpVertex->toVertexes.push_back(to);
                }
            }
            if(! foundTo){
                foundTo=tmpVertex->equalsTo(to);
                if(foundTo){
                    tmpVertex->fromVertexes.push_back(from);
                }
            }
        }
        if(!foundFrom){
            this->addVertex(from);
        }
        if(!foundTo){
            this->addVertex(to);
        }
        foundTo=false;
        for(unsigned int i=0;!foundTo && i<from->toVertexes.size(); i++){
            foundTo=from->toVertexes.at(i)->index==to->index;
        }
        if(!foundTo){
            from->toVertexes.push_back(to);
        }
        foundFrom=false;
        for(unsigned int i=0; !foundFrom && i<to->fromVertexes.size(); i++){
            foundFrom=to->fromVertexes.at(i)->index==from->index;
        }
        if(!foundFrom){
            to->fromVertexes.push_back(from);
        }
    }
    int edgeIndex(Vertex* from, Vertex* to){
        int index=-1;
        Edge* edge;
        for(unsigned int i=0; i<this->edges.size(); i++){
            edge=this->edges.at(i);
            if(edge->from->index==from->index && edge->to->index==to->index){
                return i;
            }
        }
        return index;
    }
};

struct INFGraph:Graph{
    SinkVertex* sink;
    SourceVertex* source;
    vector<JobVertex*> jobVertexes;
    vector<FrameVertex*> frameVertexes;
    INFGraph(string name):Graph(name,vector<Edge*>(),vector<Vertex*>()){

    }

    void addEdge(JobVertex* job, double cap){
        Edge* sjedge=new Edge(this->edges.size(), cap, this->source, job);
        Graph::addEdge(sjedge);
    }
    void addEdge(JobVertex* job, FrameVertex* frame, double cap){
        Edge* jfedge=new Edge(this->edges.size(),cap,job,frame);
        Graph::addEdge(jfedge);
    }
    void addEdge(FrameVertex* frame, double cap){
        Edge* fsedge=new Edge(this->edges.size(), cap, frame, sink);
        Graph::addEdge((fsedge));
    }
    void importTaskSet(TaskSet* set, double frame, double hyperperiod){
        level_labeling=-1;
        this->vertexes.clear();
        this->jobVertexes.clear();;
        this->frameVertexes.clear();
        this->edges.clear();
        this->vertexLabels.clear();
        this->edgeLabels.clear();
        cout<<"==>"<<vertexes.size()<<endl;
        this->source=new SourceVertex(0);
        this->source->step=-1;
        this->sink=new SinkVertex(1);
        this->sink->step=-1;
        this->addVertex(source);
        this->addVertex(sink);
        int framesInH=floor(hyperperiod/frame);// must be integer!
        cout<<"==>"<<" frame vertexes in H=="<<framesInH<<endl;
        for(int i=0; i<framesInH; i++){
            string frameName="F_"+QString::number(i+1).toStdString();
            FrameVertex* newFrame=new FrameVertex(vertexes.size(), frameName);
            addVertex(newFrame);
            frameVertexes.push_back(newFrame);
            addEdge(newFrame, frame);
        }
        Task* tmp;
        JobVertex* newJob;
        for(int i=0; i<set->size(); i++){
            int jobsInH=floor(hyperperiod/set->at(i)->period);// must be integer!
            for(int j=1; j<=jobsInH; j++){//adds all jobs for a specific task
                tmp=set->at(i);
                string jobName="J_"+tmp->name+"_"+QString::number(j).toStdString();
                newJob=new JobVertex(vertexes.size(),jobName.c_str());
                this->addVertex(newJob);
                this->jobVertexes.push_back(newJob);
                this->addEdge(newJob, tmp->execution_time);
                double jobStart=tmp->period*(j-1);
                double jobEnd=tmp->deadline+jobStart;
                for(int f=0; f<framesInH; f++){
                    cout<<"JOB: "<<newJob->name<<": ["<<jobStart<<", "<<jobEnd<<"]. FRAMEs: ";
                    if((f+1)*frame<=jobEnd){//frame ends no later than job deadline
                        if(f*frame>=jobStart){//frame starts no sooner than job release
                            cout<<frameVertexes.at(f)->name<<", ";
                            addEdge(newJob, frameVertexes.at(f), frame);
                        }
                    }
                    cout<<endl;
                }
            }
        }
    }
    string toStringComplete() const{
        string toReturn="digraph "+this->name+" {\n";
        // ALL edges with label & color if changed
        Vertex* from;
        Vertex* to;
        Edge* edge;
        for(unsigned int i=0; i<edges.size(); i++){
            edge=edges.at(i);
            from=edge->from;
            to=edge->to;
            toReturn+=from->name;
            toReturn+=" -> ";
            toReturn+=to->name;
            toReturn+=" [label=\"[";
            toReturn+=QString::number(edge->tmpLabel->capacity).toStdString()+", "+QString::number(edge->tmpLabel->tmpFlow).toStdString()+"]";
            toReturn+="\"";
            if(edge->step>=level_labeling){
                level_labeling=edge->step;
                toReturn+=",color=\"red\"";
            }
            toReturn+="];\n";
        }
        // ALL JOB VERTEXES
        toReturn+="subgraph cluster_JOBS{\nlabel=\"Job Vertexes\";\ncolor=lightgrey;\nstyle=filled\n";
        JobVertex* jv;
        for(unsigned int i=0; i<this->jobVertexes.size(); i++){
            jv=jobVertexes.at(i);
            toReturn+=jv->name;
            if(jv->step>=level_labeling){
                level_labeling=jv->step;
                toReturn+=" [color=\"red\"]";
            }
            toReturn+=" ;\n";
        }
        toReturn+="}\n";
        // ALL FRAME VERTEXES
        toReturn+="subgraph cluster_FRAMES{\nlabel=\"Frame Vertexes\";\ncolor=\"lightblue\";\nstyle=filled\n";
        FrameVertex* fv;
        for(unsigned int i=0; i<this->frameVertexes.size();i++){
            fv=this->frameVertexes.at(i);
            toReturn+=fv->name;
            if(fv->step>=level_labeling){
                level_labeling=fv->step;
                toReturn+=" [color=\"red\"]";
            }
            toReturn+=" ;\n";
        }
        toReturn+="}\n";
        // SOURCE
        toReturn+=source->name+" [shape=Mdiamond";
        if(source->step>=level_labeling){
            level_labeling=source->step;
            toReturn+=",color=\"red\"";
        }
        toReturn+="];\n";
        //SINK
        toReturn+=sink->name+" [shape=Mdiamond";
        if(sink->step>=level_labeling){
            level_labeling=sink->step;
            toReturn+=",color=\"red\"";
        }
        toReturn+="];\n}";
        return toReturn;
    }
};
}

#endif // STRUCTS_H
