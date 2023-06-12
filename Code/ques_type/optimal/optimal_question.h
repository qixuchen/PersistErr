#include "Eigen/Eigen"
//#define VOLESTI_DEBUG
#include <fstream>
#include "boost/random.hpp"
#include "boost/random/uniform_int.hpp"
#include "boost/random/normal_distribution.hpp"
#include "boost/random/uniform_real_distribution.hpp"
#include "random_walks/random_walks.hpp"
#include "volume/volume_sequence_of_balls.hpp"
#include "volume/volume_cooling_gaussians.hpp"
#include "volume/volume_cooling_balls.hpp"
#include "exact_vols.h"
#include "generators/known_polytope_generators.h"
#include "sampling/sampling.hpp"

#include "../Others/used_function.h"
#include "../exp_stats.h"
#include <map>
#include <math.h>
#include <algorithm>
#include <random>
#include <unordered_set>


#ifndef OPT_QUESTION
#define OPT_QUESTION

#define SCORE_SELECT 1
#define RAND_SELECT 2
#define PURE_RANDOM 3

namespace optimal_question{

    struct sample_set
    {
        std::set<point_t *> data;
        // key:sample point, value: data point
        std::set<point_t *> sample;
    };

    int optimal_listwise(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta, int s);

    int optimal_sup_inf(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta, int s);
}

#endif