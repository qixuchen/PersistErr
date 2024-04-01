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
#include "../user_study_func.h"
#include <map>
#include <math.h>
#include <algorithm>
#include <random>

#ifndef SAMPLING
#define SAMPLING

#define SCORE_SELECT 1
#define RAND_SELECT 2

#define LABELED_PRICE 1
#define FINAL_PRICE 2

namespace sampling{

    struct sample_set
    {
        std::set<point_t *> data;
        // key:sample point, value: data point
        std::set<point_t *> sample;
    };

    int sampling(std::vector<point_t *> p_set, point_set_t *P0, int k, int w, int select_opt, int display_mode, int alg_id);
}

#endif