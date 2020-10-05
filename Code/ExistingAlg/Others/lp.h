#ifndef LP_H
#define LP_H

#include <glpk.h>
#include "data_struct.h"
#include "data_utility.h"
#include "operation.h"
#include <vector>

// solve LP using GLPK

// Use LP to check whehter a point pt is a conical combination of the vectors in ExRays
bool insideCone(std::vector<point_t*> ExRays, point_t* pt);

// Use LP to find a feasible point of the half sapce intersection (used later in Qhull for half space intersection)
point_t* find_feasible(std::vector<hyperplane_t*> hyperplane);

// solve the LP in frame computation
void solveLP(std::vector<point_t*> B, point_t* b, double& theta, point_t* & pi);

// Check whether the hyperplane is the boundary of the range R(the intersection of the halfspace)
bool check_hyperplane_on_boundary(hyperplane_t* hyperplane, halfspace_set_t* half_set);

// Use LP to check whether the hyperplane intersects with the range R(the intersection of the halfspace)
bool check_hyperplane_intersect(hyperplane_t* hyperplane, halfspace_set_t* half_set, int direction);

// Use LP to check whether the the intersection of the halfspaces is feasible
// If it is feasible, set the check_point as the solution of the LP
bool check_halfspace_intersect(halfspace_set_t* half_set);

//Find the least number of halfspaces which construct a polytope
//Prune the halfspaces which are unnecessary
void prune_halfspace(halfspace_set_t* half_set);

#endif
