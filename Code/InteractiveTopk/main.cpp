#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "HDPI/HDPI.h"
#include "rev_HDPI/rev_HDPI.h"
#include "exact/exact.h"
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
    srand(time(NULL));
    point_set_t *P0 = read_points((char*)"4d1k.txt");
    int dim = P0->points[0]->dim; //obtain the dimension of the point
    int k = 1, num_repeat = 10;
    std::vector<point_t *> p_set, p0;
    skyband(P0, p_set, 1);
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
        find_top_k(u, P, top_current, k);
        best_score = dot_prod(u, top_current[0]);

        // rev_HDPI(p_set, u, k);
        // Exact(p_set, u, 2);
        // compute_convh_hyperplanes(p_set);
        Approx(p_set, u, 2);
    }
    

    std::cout << "correct count: " << correct_count << std::endl;
    std::cout << "avg question num: "<< question_num/num_repeat << std::endl;
    std::cout << "avg return size: "<< return_size/num_repeat << std::endl;
    release_point_set(P, true);
    return 0;
}
