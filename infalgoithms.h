#ifndef INFALGOITHM_H
#define INFALGOITHM_H
#include <vector>
#include "Structs.h"

using namespace inf;
class INFAlgoithms
{
public:
    INFAlgoithms();
    static int gcd(vector<int> values);
    static int lcm(vector<int> values);
    static double findHyperperiod(vector<int> periods);

private:
    vector<double> detectFrameSizes(bool first_constraint_considered, TaskSet* set);
    static int gcd_r(int a,int b);
    static int lcm(int a, int b);
};

#endif // INFALGOITHM_H
