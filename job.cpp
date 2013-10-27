#include "job.h"

int Job::index_counter=0;
Job::Job(QObject *parent, int index) :
    QObject(parent)
{
    if(index==index_counter){
        this->index=index_counter++;
    }
    else {
        this->index=index;
    }
}
Job::Job(int index) :
    QObject(0){
    this->index=index;
}

int Job::getIndex(){
    return this->index;
}
