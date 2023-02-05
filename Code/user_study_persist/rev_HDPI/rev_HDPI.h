#include "../Others/used_function.h"
#include "../exp_stats.h"
#include "../user_study_func.h"

#ifndef REV_HDPI
#define REV_HDPI

/**
 * @brief Asking user question and return one of the top-k point
 *        Find the top-1 point accurately convex Hull
 * @param p_set 		 The dataset
 * @param u 			 The linear function
 * @param k 			 The parameter
 */
int rev_HDPI(std::vector<point_t *> p_set, point_set_t *P0, int w);

#endif