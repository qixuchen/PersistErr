#ifndef READ_WRITE_H
#define READ_WRITE_H

#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"


// read points from the input file
point_set_t* read_points(char* input);

// check domination
int dominates(point_t* p1, point_t* p2);

int dominates_same(point_t* p1, point_t* p2);

// compute the skyline set
point_set_t* skyline_point(point_set_t *p);

// prepare the file for computing the convex hull (the candidate utility range R) via half space interaction
void write_hyperplanes(vector<hyperplane_t*> utility_hyperplane, point_t* feasible_pt, char* filename);


#endif