#include "../Others/used_function.h"
#include "user_mode.h"
#include "partition_func.h"
#include <ctime>
#include <cmath>
#include <chrono>
#include <sys/time.h>
#include "../user_study_func.h"


#ifndef POINTPRUNE
#define POINTPRUNE

/**
 * @brief           PointPrune, with some modifications (e.g., end prematurely) 
 */
int PointPrune_v2(std::vector<point_t *> p_set, point_set_t *P0, int checknum, int w, double *max, double *min);

#endif