#include "Others/used_function.h"

/**
 * @brief Ask user question and find one of the top-k point
 *        For find the point which is the top-k point w.r.t any u in R, we search the whole set
 * @param p_set 		The selected points
 * @param original_P    The set containing all the points
 * @param k 			The threshold top-k
 */
point_t* RH(FILE *wPtr, std::vector<point_t*> p_set, point_set_t* original_P, int k, int &questions);