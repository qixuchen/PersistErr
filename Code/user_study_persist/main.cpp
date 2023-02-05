#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "rev_HDPI/rev_HDPI.h"
#include "UH/maxUtility.h"
#include "Preference_Learning/preferLearn.h"
#include "UtilityApprox/UtilityApprox.h"
#include "Active_Ranking/active_ranking.h"
#include "PointPrune/PointPrune.h"
#include "exact/exact.h"
#include "exact/Exact_revised.h"
#include "exact/optimal.h"
#include "sampling/sampling.h"
#include "optimal/optimal.h"
#include "approx/approx.h"
#include "Others/qhull_build.h"
#include <vector>
#include <ctime>
#include <iostream>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    // parameters
    int k = 2;
    int num_repeat = 3;
    int w = 4;
    double theta = 0.05;

    srand(time(NULL));
    point_set_t *P0 = read_points((char*)"4d1k.txt");
    int dim = P0->points[0]->dim; //obtain the dimension of the point
    std::vector<point_t *> p_set, p0;
    skyband(P0, p_set, 1);
    cout << "skyline size: " << p_set.size() << endl;
    point_set_t *P = point_reload(p_set);

    for(int i = 0; i < num_repeat; i++){
        cout << "round " << i << endl;
        // generate the utility vector
        point_t *u = alloc_point(dim);
        double sum = 0;
        for (int i = 0; i < dim; i++)
        {
            u->coord[i] = ((double) rand()) / RAND_MAX;
            sum += u->coord[i];
        }
        for (int i = 0; i < dim; i++)
            u->coord[i] = u->coord[i]/sum;
        // look for the ground truth top-k point
        std::vector<point_t *> top_current;
        find_top_k(u, P, top_current, 1);
        best_score = dot_prod(u, top_current[0]);

        // initial parameters
        int s = 2, maxRound = 1000;
        double v1 = dot_prod(u, top_current[0]);
        double v2 = dot_prod(u, top_current[1]);
        double epsilon = (v1 - v2) / v1;
        int prune_option = RTREE, dom_option = HYPER_PLANE, stop_option = EXACT_BOUND, cmp_option = SIMPLEX;

        // the exact Algorithm
        // exact_rev::Exact_revised(p_set, u, k, w, SCORE_SELECT, theta);

        // the sampling Algorithm
        // sampling::sampling(p_set, u, k, w, RAND_SELECT, theta);

        // the optimal Algorithm
        // optimal::optimal(p_set, u, k, w, SCORE_SELECT, theta);

        // the HDPI Algorithm
        // rev_HDPI(p_set, u, 1, w, theta);

        // PointPrune Algorithm
        // PointPrune_v2(p_set, u, 3, w, theta);

        // the UtilityApprox Algorithm
        // utilityapprox(P, u, 2, epsilon, maxRound, w, theta);

        // Algorithm: Active Ranking
        // Active_Ranking(p_set, u, 1, w, theta);
        
        // Algorithm: Preference Learning
        // Preference_Learning_accuracy(p_set, u, 1, w, theta);

        // the UH-Simplex algorithm
        max_utility(P, u, 2, epsilon, maxRound, cmp_option, stop_option, prune_option, dom_option, w, theta);
    }
    std::cout << "correct count: " << correct_count << std::endl;
    std::cout << "avg question num: "<< question_num/num_repeat << std::endl;
    std::cout << "avg return size: "<< return_size/num_repeat << std::endl;
    std::cout << "avg time: "<< avg_time() << std::endl;
    release_point_set(P, true);
    return 0;
}
