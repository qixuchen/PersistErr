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
#include <vector>
#include <ctime>
#include <sys/time.h>


int main(int argc, char *argv[])
{
    //reload point
    point_set_t *P0 = read_points((char*)"2d.txt");
    int dim = P0->points[0]->dim; //obtain the dimension of the point
    int k = 1;
    std::vector<point_t *> p_set, p0;
    skyband(P0, p_set, k);
    point_set_t *P = point_reload(p_set);


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
    find_top_k(u, P, top_current, k);
    printf("---------------------------------------------------------\n");
    printf("|%30s |%10s |%10s |\n", "Ground Truth", "--", "--");
    for (int i = 0; i < k; i++)
        printf("|%30s |%8s%2d |%10d |\n", "Point", "top -", i + 1, top_current[i]->id);
    printf("---------------------------------------------------------\n");

    //Algorithm: Active Ranking
    //Active_Ranking(p_set, u, k);
    //Algorithm: Preference Learning
    //Preference_Learning_accuracy(p_set, u, k);

    // initial parameters
    int s = 2, maxRound = 1000;
    double v1 = dot_prod(u, top_current[0]);
    double vk = dot_prod(u, top_current[k - 1]);
    double epsilon = (v1 - vk) / v1;
    point_t *p;
    int prune_option = RTREE, dom_option = HYPER_PLANE, stop_option = EXACT_BOUND, cmp_option;

    // the 2-dimensional algorithms
    if (dim == 2)
    {
        // the Median algorithm
        Median(P, u, maxRound);
        // the Median_Adapt algorithm
        Median_Adapt(p_set, u, maxRound, k);
        // the Hull algorithm
        Hull(P, u, s, maxRound);
        // the Hull_Adapt algorithm
        Hull_Adapt(p_set, u, maxRound, k);
    }

    cmp_option = SIMPLEX;
    // the UH-Simplex algorithm
    max_utility(P, u, s, epsilon, maxRound, cmp_option, stop_option, prune_option, dom_option);
    // the UH-Simplex-Adapt algorithm
    //Simplex_Adapt(p_set, u, epsilon, k);

    cmp_option = RANDOM;
    // the UH-Random algorithm
    max_utility(P, u, s, epsilon, maxRound, cmp_option, stop_option, prune_option, dom_option);
    // the UH-Random-Adapt algorithm
    Random_Adapt(p_set, u, epsilon, k);

    // the UtilityApprox Algorithm
    utilityapprox(P, u, s, epsilon, maxRound);

    release_point_set(P, true);
    return 0;
}
