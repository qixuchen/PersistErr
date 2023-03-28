#include "../Others/data_struct.h"
#include "../Others/data_utility.h"
#include "../Others/operation.h"
#include <ctime>
#include <cstdlib>

#ifndef USER_MODE_H
#define USER_MODE_H

int user_rand_err(const std::vector<point_t *> &p_set, point_t* u, int p1, int p2, double err_rate, int &round);

point_t* user_rand_err(point_t* u, point_t* p1, point_t* p2, double err_rate, int &round);

int checking(const std::vector<point_t *> &p_set, point_t* u, int p1, int p2, double err_rate, int k, int &round);

point_t* checking(point_t* u, point_t* p1, point_t* p2, double err_rate, int k, int &round);

point_t* checking_varyk(point_t* u, point_t* p1, point_t* p2, double err_rate, int k, int &round);

point_t* checking_varyk_persist(point_t* u, point_t* p1, point_t* p2, double err_rate, int k, int &round);

#endif