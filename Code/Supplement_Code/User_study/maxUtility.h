#ifndef MAXUTILITY_H
#define MAXUTILITY_H

#include "Others/data_struct.h"
#include "Others/data_utility.h"
#include "Others/read_write.h"
#include "Qhull/qhull_a.h"
#include "Others/used_function.h"
#include <vector>
#include <algorithm> 

#include "Others/rtree.h"
#include "lp.h"
#include "Others/pruning.h"
#include <queue>

#define RANDOM 1
#define SIMPLEX 2
#define SIMPLEX_FLY 3

using namespace std;

// get the index of the "current best" point
int get_current_best_pt(point_set_t* P, vector<int>& C_idx, vector<point_t*>& ext_vec);

// generate s cars for selection in a round
void update_ext_vec(point_set_t* P, vector<int>& C_idx, point_t* u, int s, vector<point_t*>& ext_vec, int& current_best_idx, int& last_best, vector<int>& frame, int cmp_option);

// generate the options for user selection and update the extreme vecotrs based on the user feedback
vector<int> generate_S(point_set_t* P, vector<int>& C_idx, int s, int current_best_idx, int& last_best, vector<int>& frame, int cmp_option);

/*
 * @brief The interactive algorithm Random/Simplex. Find best points/return a point which satisfy the regret ratio
 * @param original_P     the original dataset
 * @param u              the unkonwn utility vector
 * @param s              the question size
 * @param epsilon        the required regret ratio
 * @param maxRound       the maximum number of rounds of interactions
 * @param cmp_option     the car selection mode
 *                       -SIMPLEX UH-Simplex method
 *                       -RANDOM UH-Simplex method
 * @param stop_option    the stopping condition
 *                       -NO_BOUND
 *                       -EXACT_BOUND
 *                       -APRROX_BOUND
 * @param prune_option   the skyline algorithm
 *                       -SQL
 *                       -RTREE
 * @param dom_option     the domination checking mode
 *                       -HYPER_PLANE
 *                       -CONICAL_HULL
 */
point_t* max_utility(FILE *wPtr, point_set_t *P0, point_set_t* original_set, int cmp_option, int &questions);

#endif