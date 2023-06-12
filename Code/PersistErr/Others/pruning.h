#ifndef PRUNING_H
#define PRUNING_H

#include "../Others/data_struct.h"
#include "../Others/data_utility.h"

#include "operation.h"
#include "../lp.h"
#include "../Others/frame.h"
#include "read_write.h"
#include "../Qhull/io.h"
#include <queue>

// the domination options
#define HYPER_PLANE 1
#define CONICAL_HULL 2

// the skyline options
#define SQL 1
#define RTREE 2

//  the stopping options
#define NO_BOUND 1
#define EXACT_BOUND 2
#define APPROX_BOUND 3

using namespace std;

/**
 * @brief Conduct halfspace intersection by invoking Qhull
 * @param rPtr   Contains the data of all the halfspace
 * @param wPtr   The extreme points of the intersection and the necessary halfspaces are shown in wPtr
 */
int halfspace(FILE* rPtr, FILE* wPtr);


/**
 * @brief Get the set of extreme points of R (bounded by the extreme vectors)
 * @param ext_vec    The hyperplanes(in the form of point) bounding the utility range R
 * @return  All the extreme points of R
 */
vector<point_t*> get_extreme_pts(vector<point_t*>& ext_vec);

/**
 * @brief Used to check whether the hyperplane is on one side of the bounding sphere of the halfspace set
 *        Not the relation between the hyperplanes and the halfspace set
 * @param hyper     The hyperplane
 * @param half_set  The halfspace set
 * @return The situation of the hyperplane and the bounding shpere
 *      -1 The hyperplane on the positive side of the bounding shpere
 *      -0 The hyperplane intersects with the bounding shpere
 *     --1 The hyperplane on the negative side of the bounding sphere
 */
int check_situation_bounding_sphere(hyperplane_t* hyper, halfspace_set_t* half_set);

/**
 * @brief Used to check whether the hyperplane is on one side of the bounding rectangle of the halfspace set
 *        Not the relation between the hyperplanes and the halfspace set
 * @param hyper     The hyperplane
 * @param half_set  The halfspace set
 * @return The situation of the hyperplane and the bounding rectangle
 *      -1 The hyperplane on the positive side of the bounding rectangle
 *      -0 The hyperplane intersects with the bounding rectangle
 *     --1 The hyperplane on the negative side of the bounding rectangle
 */
int check_situation_bounding_rec(hyperplane_t* hyper, halfspace_set_t* half_set);

/**
 * @brief Get an "exact" upper bound bound in O(|ext_pts|^2) time based on R
 * @param ext_pts       All the hyperplanes(in the form of points) bounding R
 * @return The regret ratio
 */
double get_rrbound_exact(vector<point_t*> ext_pts);

/**
 * @brief Check whether the intersection of the halfspaces exists
 *        Set the extreme points of the halfspace set. Set the average point of ext_pts as a ext_pt
 *        Refine the bounded hyperplanes
 * @param half_set The halfspace set. All the halfspaces are in the form of <=0
 * @return  Whether the intersection of the halfspaces exists
 *   -true  The intersection exist
 *   -false The intersection does not exist
 */
bool get_extreme_pts_refine_halfspaces_alg1(halfspace_set_t* half_set);

/**
 * @brief Check whether the intersection of the halfspaces exists
 *        Set the extreme points of the halfspace
 *        Refine the bounded hyperplanes
 *        Set the bounding sphere
 * @param half_set The halfspace set. All the halfspaces are in the form of >=0
 * @return  Whether the intersection of the halfspaces exists
 *   -true  The intersection exist
 *   -false The intersection does not exist
 */
bool get_extreme_pts_refine_from_halfspaces(halfspace_set_t* half_set);

#endif