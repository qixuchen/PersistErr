#include "../Others/used_function.h"
#include "user_mode.h"
#include "partition_func.h"
#include <ctime>
#include <cmath>
#include <chrono>
#include <sys/time.h>


#ifndef POINTPRUNE
#define POINTPRUNE

/**
 * @brief           PointPrune, with some modifications (e.g., end prematurely) 
 */
int PointPrune_v2(std::vector<point_t *> p_set, point_t *u, int checknum, int w, double theta);

/**
 * @brief           PointPrune assuming user makes persistent errors
 */
int PointPrune_Persist(std::vector<point_t *> p_set, point_t *u, int checknum, int w, double theta);
#endif