#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "HDPI/HDPI.h"
#include "rev_HDPI/rev_HDPI.h"
#include "exact/exact.h"
#include "exact/Exact_revised.h"
#include "exact/optimal.h"
#include "sampling/sampling.h"
#include "optimal/optimal.h"
#include "approx/approx.h"
#include "RH/alg_one.h"
#include "2DPI/2dPI.h"
#include "Others/qhull_build.h"
#include <vector>
#include <ctime>
#include <iostream>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    // parameters
    int k = 2;
    int num_repeat = 1;
    int w = 4;
    double theta = 0.05;

    srand(time(NULL));
    point_set_t *P0 = read_points((char*)"5d100k.txt");
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

        // rev_HDPI(p_set, u, k);
        // Exact(p_set, u, 2);
        // compute_convh_hyperplanes(p_set);
        // Approx(p_set, u, k);
        // Exact(p_set, u, k);
        // Optimal(p_set, u, k);
        // exact_rev::Exact_revised(p_set, u, k, w, theta);
        sampling::sampling(p_set, u, k, w, theta);
        // optimal::optimal(p_set, u, k, w, theta);
    }
    

    std::cout << "correct count: " << correct_count << std::endl;
    std::cout << "avg question num: "<< question_num/num_repeat << std::endl;
    std::cout << "avg return size: "<< return_size/num_repeat << std::endl;
    std::cout << "avg time: "<< avg_time() << std::endl;
    release_point_set(P, true);
    return 0;
}
