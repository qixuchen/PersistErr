#include "../Others/used_function.h"
#include "../exp_stats.h"

/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
int Active_Ranking(std::vector<point_t*> p_set, point_t* u, int k, int w, double theta);


/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
int Active_Ranking_persist(std::vector<point_t *> p_set, point_t *u, int k, int w, double theta);