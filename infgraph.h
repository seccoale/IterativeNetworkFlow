#ifndef INFGRAPH_H
#define INFGRAPH_H
#include <vector>
#include <cstddef>
#include <stdio.h>
#include <string>
namespace inf {
using namespace std;
struct Edge;

struct Vertex{
    vector<Edge*> inEdges;
    vector<Edge*> outEdges;
    Vertex* prev;
    int index;
    bool visited;
    virtual string toString() const { return ""; }
    Vertex():inEdges(),
        outEdges(),
        prev(NULL),
        visited(false),
        index(-1){}

};

class INFGraph
{
public:
    INFGraph();

private:
    static int vIndex;
};
}

#endif // INFGRAPH_H
