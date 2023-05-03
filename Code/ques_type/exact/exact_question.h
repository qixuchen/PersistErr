#include "../Others/used_function.h"
#include "../exp_stats.h"
#include <map>
#include <math.h>
#include <algorithm>
#include <random>
#include <unordered_set>

#ifndef EXACT_QUESTION
#define EXACT_QUESTION

#define SCORE_SELECT 1
#define RAND_SELECT 2


namespace exact_question{

    struct conf_region
    {
        std::set<point_t *> points;
        std::vector<halfspace_set_t *> partitions;
    };

    int exact_listwise(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta, int s);

    int exact_sup_inf(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta, int s);
}




#endif
