#include "../Others/used_function.h"
#include "../exp_stats.h"
#include <map>
#include <math.h>
#include <algorithm>
#include <random>
#include "../user_study_func.h"

#ifndef EXACT_REVISED
#define EXACT_REVISED

#define SCORE_SELECT 1
#define RAND_SELECT 2

struct conf_region
{
    std::set<point_t *> points;
    std::vector<halfspace_set_t *> partitions;
};


namespace exact_rev{
    int Exact_revised(std::vector<point_t *> p_set, point_set_t *P, int k, int w, int select_opt, int alg_id);
}




#endif
