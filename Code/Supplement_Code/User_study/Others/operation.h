#ifndef OPERATION_H
#define OPERATION_H

#include "data_struct.h"
#include "data_utility.h"
#include <cmath>
#include <vector>

using namespace std;

#define EQN_EPS 1e-9

// Basic operaitons
int isZero(double x);
DIST_TYPE calc_l1_dist( point_t* point_v1, point_t* point_v2);
DIST_TYPE calc_len(point_t* point_v);
point_t* copy(point_t * point_v2);
double dot_prod(point_t* point_v1, point_t* point_v2);
double dot_prod(point_t* point_v1, double* v);
double dot_prod(double* v1, double* v2, int dim);
point_t* sub(point_t* point_v1, point_t* point_v2);
point_t* add(point_t* point_v1, point_t* point_v2);
point_t* scale(double c, point_t* point_v);
double compute_intersection_len(hyperplane_t *hp, point_t* s);
int guassRank(vector< point_t* > P);
bool linearInd(point_t* p, point_t* q);
point_t* getPoint(point_set_t* p, double* point);

#endif