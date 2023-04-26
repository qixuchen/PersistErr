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