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
using namespace std;

namespace inf{
struct Job{
    string name;
    double release;
    double deadline;
    double execution_time;
    Job(){
    }
    Job(string NAME, double RELEASE, double DEADLINE, double EX_TIME){
        this->name=NAME;
        this->execution_time=EX_TIME;
        this->deadline=DEADLINE;
        this->release=RELEASE;
    }
    QString* toString(){
        QString* toReturn=new QString(this->name.c_str());
        toReturn->append(" ("+QString::number(this->release)+", "+QString::number(this->execution_time)+", "+QString::number(this->deadline)+");");
        return toReturn;
    }
    static Job* compile(QString literal){
        Job* job=new Job();
        int endName=literal.indexOf('(')-1;
        job->name=literal.mid(0,endName).toUtf8().constData();
        int endR=literal.indexOf(',', endName+2)-1;
        job->release=literal.mid(endName+2, endR-endName-1).toDouble();
        int endEx=literal.indexOf(',',endR+2);
        job->execution_time=literal.mid(endR+3,endEx-endR-3).toDouble();
        int endD=literal.indexOf(')')-1;
        job->deadline=literal.mid(endEx+2, endD-endEx-1).toDouble();
        return job;
    }
};

struct VertexLabel{
    /** flow indication
         * @brief flow
         */
    int flow;
    /** from which vertex the flow come from
         * @brief from
         */
    int from;
};
struct EdgeLabel{
    /** maximum flow through this edge
         * @brief capacity
         */
    int capacity;
    /** flow which is currently passing through this edge
         * @brief tmpFlow
         */
    int tmpFlow;
    EdgeLabel(const int CAPACITY){this->capacity=CAPACITY;}
    void setFlow(int new_flow){
        this->tmpFlow=new_flow;
    }
};
struct Vertex{
    int index;
    vector<Vertex*> toVertexes;
    vector<Vertex*> fromVertexes;
    VertexLabel* tmpLabel;
    Vertex(int index){
        this->index=index;
    }

    bool equalsTo( Vertex* otherV){
        return this->index==otherV->index;
    }
};

struct Edge{
    int index;
    int capacity;
    Vertex* from;
    Vertex* to;
    EdgeLabel* tmpLabel;
    Edge(const int INDEX, const int CAPACITY,  Vertex* FROM,  Vertex* TO){
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
    QString* name;
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
    Graph(const string NAME, const vector<Edge*> EDGES, const vector< Vertex*> VERTEXES){
        this->name=new QString(NAME.c_str());
        this->edges=EDGES;
        this->vertexes=VERTEXES;
    }
    Graph(const string NAME){
        this->name=new QString(NAME.c_str());
    }
    int size(){
        return this->vertexes.size();
    }

    QString* toString() const{
        QString* toReturn=new QString();
        toReturn->append("digraph "+*this->name+" {\n");
        Vertex* tmpV;
        for(unsigned int i=0; i<this->vertexes.size(); i++){
            tmpV=this->vertexes.at(i);
            toReturn->append(QString::number(tmpV->index));
            toReturn->append(";\n");
        }
        delete tmpV;
        Edge* tmpE;
        cout<<this->edges.size();
        for(unsigned int i=0; i<this->edges.size(); i++){
            tmpE=this->edges.at(i);
            toReturn->append(QString::number(tmpE->from->index)+" -> " +QString::number(tmpE->to->index)+";\n");
        }
        delete tmpE;
        toReturn->append("}\n");
        return toReturn;
    }
    void addVertex( Vertex* newVertex){
        if(newVertex->index<this->size()){
            cout<<"Vertice già inserito, non verrà inserito nuovamente"<<endl<<flush;
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
                std::cout<<"Errore nell'inserimento di un arco!"<<endl<<flush;
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
    }
};
}
#endif // STRUCTS_H
