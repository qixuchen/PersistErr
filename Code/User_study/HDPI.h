#include "Others/used_function.h"

/**
 * @brief Asking user question and return one of the top-k point
 *        For find the point which is the top-k point w.r.t any u in R, we search the whole set
 *        Find the top-1 point by sampling
 * @param p_set 		 The original dataset
 * @param u 			 The linear function
 * @param k 			 The threshold top-k
 */
point_t* HDPI_sampling(FILE *wPtr, std::vector<point_t *> p_set, point_set_t *P0, int k, int &questions);

/**
 * @brief Asking user question and return one of the top-k point
 *        For find the point which is the top-k point w.r.t any u in R, we search the whole set
 *        Find the top-1 point accurately
 * @param p_set 		 The original dataset
 * @param u 			 The linear function
 * @param k 			 The threshold top-k
 */
point_t* HDPI_accurate(FILE *wPtr, std::vector<point_t *> p_set, point_set_t *P0, int k, int &questions);
