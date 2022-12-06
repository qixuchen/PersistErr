#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "UH/maxUtility.h"
#include "Median_Hull/medianhull.h"
#include "Preference_Learning/preferLearn.h"
#include "UtilityApprox/UtilityApprox.h"
#include "Active_Ranking/active_ranking.h"
#include "Qhull/qhull_build.h"
#include "rev_HDPI/rev_HDPI.h"
#include <vector>
#include <ctime>
#include <sys/time.h>
#include "exp_stats.h"


int main(int argc, char *argv[])
{
    int num_repeat = 20;
    int w = 4;
    double theta = 0.05;

    //reload point
    srand(time(NULL));
    point_set_t *P0 = read_points((char*)"4d1k.txt");
    int dim = P0->points[0]->dim; //obtain the dimension of the point
    int k = 1;
    std::vector<point_t *> p_set, p0;
    skyband(P0, p_set, k);
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
        find_top_k(u, P, top_current, 2);
        best_score = dot_prod(u, top_current[0]);

        // initial parameters
        int s = 2, maxRound = 1000;
        double v1 = dot_prod(u, top_current[0]);
        double v2 = dot_prod(u, top_current[1]);
        double epsilon = (v1 - v2) / v1;
        int prune_option = RTREE, dom_option = HYPER_PLANE, stop_option = EXACT_BOUND, cmp_option = SIMPLEX;

        // Algorithm: Active Ranking
        // Active_Ranking(p_set, u, k, theta);
        
        // Algorithm: Preference Learning
        // Preference_Learning_accuracy(p_set, u, k, theta);

        // the UH-Simplex algorithm
        // max_utility(P, u, 2, epsilon, maxRound, cmp_option, stop_option, prune_option, dom_option, w, theta);

        // the UtilityApprox Algorithm
        // utilityapprox(P, u, 2, epsilon, maxRound, theta);

        // the HDPI Algorithm
        // rev_HDPI(p_set, u, 1, w, theta);

    }
    std::cout << "correct count: " << correct_count << std::endl;
    std::cout << "avg question num: "<< question_num/num_repeat << std::endl;
    std::cout << "avg time: "<< avg_time() << std::endl;
    release_point_set(P, true);
    return 0;
}
