#include "infalgoithms.h"
#include <math.h>
#include <QDebug>
#include <iostream>
#include <stdio.h>

INFAlgorithms::INFAlgorithms()
{

}
vector<double> INFAlgorithms::detectFrameSizes(TaskSet* set){
    double hyperperiod;
    vector<int> integerPeriods=vector<int>();
    for(int i=0; i<set->size(); i++){
        double dp=(set->at(i)->period*100.);
        integerPeriods.push_back((int)dp);
    }
    hyperperiod=findHyperperiod(integerPeriods);
    vector<double> toReturn;
    double min_f=0;
    for(int f=(int)100*hyperperiod; f>(int)100*min_f; f--){
        if((100*((int)hyperperiod))%f==0 && f%100==0){
            bool third_constraint_satisfied=true;
            for(int j=0; third_constraint_satisfied && j<set->size(); j++){
                third_constraint_satisfied=2*f-gcd_r((int)(set->at(j)->period*100), f)<=(int)(100*(set->at(j)->deadline));
            }
            if(third_constraint_satisfied){
                toReturn.push_back((double)f/100.0);
            }
        }
    }
return toReturn;
}
/** Receives the periods *100
 * @brief INFAlgoithms::findHyperperiod
 * @param periods the 100*periods of a TaskSet
 * @return the hyperperiod (double, it divides for 100)
 */
double INFAlgorithms::findHyperperiod(vector<int> periods){
    cout<<"find hyperperiod"<<endl;
    double hyperperiod=(double)lcm(periods);
    hyperperiod=hyperperiod/100.;
    while(hyperperiod<1){
        hyperperiod+=hyperperiod;
    }
    return hyperperiod;
}

double INFAlgorithms::findHyperperiod(TaskSet* set){
    vector<int> periods=*new vector<int>();
    for(int i=0; i<set->size(); i++){
        double dp=set->at(i)->period*100.;
        periods.push_back((int)dp);
    }
    return findHyperperiod(periods);
}

/*
     * This uses the euclid's algortithm
     */
int INFAlgorithms::gcd(vector<int> values){
    if(values.size()<2){
        QDebug(new QString("not enough values"));
        throw new exception();
    }
    //BASE
    if(values.size()==2){
        return gcd_r(values.at(0), values.at(1));
    }
    //RECURSION: gcd(a,b,c)=gcd(a, gcd(b, c))
    else {
        int tmpGCD=gcd_r(values.back(), values.size()-2);//gcd of last 2 elements
        values.pop_back();
        values.pop_back();
        values.push_back(tmpGCD);
        return gcd(values);
    }
}
int INFAlgorithms::lcm(vector<int> values){
    cout<<"FIN QUA C'È"<<endl;
    for(unsigned int i=0; i<values.size(); i++){
        cout<<values.at(i)<<" | ";
    }
    cout<<"ENDED"<<endl;
    if(values.size()<2){
        QDebug(new QString("not enough values"));
        throw new exception();
    }
    int m=values.front();
    for(unsigned int i=1; i<values.size(); i++){
        m=lcm(m, values.at(i));
    }
    return m;
}

/*
 * Euclid's recursive algorithm
 */
int INFAlgorithms::gcd_r(int a, int b){
    if(a==0){
        return b;
    }
    else if(b==0){
        return a;
    }
    else {
        return gcd_r(b, a%b);
    }
}
int INFAlgorithms::lcm(int a, int b){
    return a*b/gcd_r(a,b);
}
