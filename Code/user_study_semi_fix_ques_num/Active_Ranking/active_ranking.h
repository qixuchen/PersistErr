#include "../Others/used_function.h"
#include "../exp_stats.h"
#include "../user_study_func.h"

/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
int Active_Ranking(std::vector<point_t*> p_set, point_set_t *P0, int w, int alg_id, int max_round);