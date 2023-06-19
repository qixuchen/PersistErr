#include "../Others/used_function.h"
#include "../exp_stats.h"
#include <map>
#include <math.h>
#include <algorithm>
#include <random>
#include <unordered_set>


#ifndef OPT_EXACT
#define OPT_EXACT

#define SCORE_SELECT 1
#define RAND_SELECT 2

namespace optimal_exact{

    struct conf_region
    {
        std::set<point_t *> points;
        std::vector<halfspace_set_t *> partitions;
    };

    int optimal_exact(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta);
}

#endif