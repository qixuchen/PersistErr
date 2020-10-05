#ifndef MEDIANHULL_H
#define MEDIANHULL_H

#include "../Others/data_struct.h"
#include "../Others/data_utility.h"
#include "../Qhull/qhull_a.h"
#include "../Qhull/libqhull.h"
#include "../Qhull/geom.h"
#include "../Qhull/qset.h"
#include "../Qhull/mem.h"
#include "../Others/operation.h"
#include <algorithm>


// get the set of vertices of Conv(P)
vector<point_t*> getVertices(point_set_t* P);

// Algorithm Median
int Median(point_set_t *P, point_t *u, int maxRound);

// Algorithm Hull
int Hull(point_set_t *P, point_t *u, int s, int maxRound);

// Algorithm Median-Adapt
int Median_Adapt(std::vector<point_t *> vertices, point_t *u, int maxRound, int k);

// Algorithm Hull-Adapt
int Hull_Adapt(std::vector<point_t *> vertices, point_t *u, int maxRound, int k);

#endif