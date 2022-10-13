#include "exact.h"

#ifndef OPTIMAL_ALG 
#define OPTIMAL_ALG


struct Check_set
{
    int size(){
        return PS.size();
    }
    Check_set &push_back(point_t *p){
        PS.push_back(p);
        return *this;
    }

    double prod(int i, point_t *p2);
    std::vector<point_t *> PS;
    int lower = 0, upper = 0, cur = 0;
};



int Optimal(std::vector<point_t *> p_set, point_t *u, int k);

#endif