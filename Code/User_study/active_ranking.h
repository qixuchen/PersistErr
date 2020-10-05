#include "Others/used_function.h"


/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
point_t* Active_Ranking(FILE *wPtr, std::vector<point_t *> p_set, point_set_t* P0, int k, int &questions);