#ifndef FAKEPOINTS
#define FAKEPOINTS

#include "../Others/data_struct.h"
#include "../Others/data_utility.h"
#include "../Others/used_function.h"

#include "../Others/operation.h"

/**
 * @brief Danupon's UtilityApprox algorithm
 * @param P the dataset
 * @param u the real utility vector
 * @param s the number of points shown in each question
 * @param epsilon the threshold of regret ratio
 * @param maxRound the upper bound of number of questions
 */
int utilityapprox(point_set_t *P, point_t *u, int s, double epsilon, int maxRound);


#endif