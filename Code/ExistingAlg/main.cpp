#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "UH/maxUtility.h"
#include "Median_Hull/medianhull.h"
#include "Preference_Learning/preferLearn.h"
#include "UtilityApprox/UtilityApprox.h"
#include "Active_Ranking/active_ranking.h"
#include "PointPrune/PointPrune.h"
#include "Qhull/qhull_build.h"
#include "rev_HDPI/rev_HDPI.h"
#include <vector>
#include <ctime>
#include <sys/time.h>
#include "exp_stats.h"
#include <string>
#include <fstream>


int main(int argc, char *argv[])
{
    int num_repeat;
    string input_file, alg_name;
    if(argc == 1){
        input_file = "HTRU.txt";
        alg_name = "util";
        num_repeat = 1000;
    } 
    else if(argc != 4){
        cout << "usage: ./prog NUM_REPEAT ALG_NAME INPUT theta" << endl;
        cout << "ALG_NAME: exact | sampling | optimal" << endl;
        exit(-1);
    }
    else{
        num_repeat = atoi(argv[1]);
        alg_name = argv[2];
        input_file = argv[3];
    }

    string ofile_name = string("./results/") + "_" + alg_name + "_" + input_file + "_" + to_string(num_repeat) + ".txt";
    ofstream ofile;
    ofile.open(ofile_name);

    // parameters
    int w = 5;
    double theta = 0.05;

    //reload point
    srand(time(NULL));
    point_set_t *P0 = read_points((char*) input_file.c_str());
    cout << alg_name << endl;
    int dim = P0->points[0]->dim; //obtain the dimension of the point
    int k = 1;
    std::vector<point_t *> p_set, p0;
    skyband(P0, p_set, k);
    // cout << "Skyline size: " << p_set.size() << endl;
    point_set_t *P = point_reload(p_set);


    for(int i = 0; i < num_repeat; i++){
        cout << "round " << i << endl;
        ofile << "round " << i << endl;
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

        if(alg_name.compare("hdpi")==0){
            // the HDPI Algorithm
            rev_HDPI(p_set, u, 1, w, theta);
        }
        if(alg_name.compare("pointprune")==0){
            // PointPrune Algorithm
            PointPrune_v2(p_set, u, 3, w, theta);
        }
        if(alg_name.compare("persist")==0){
            // PointPrune_Persist Algorithm
            PointPrune_Persist(p_set, u, 3, w, theta);
        }
        if(alg_name.compare("combine")==0){
            // PointPrune_Persist Algorithm
            PointPrune_Combine(p_set, u, 3, w, theta);
        }
        if(alg_name.compare("util")==0){
            // the UtilityApprox Algorithm
            utilityapprox(P, u, 2, epsilon, maxRound, w, theta);
        }
        if(alg_name.compare("active")==0){
            // Algorithm: Active Ranking
            Active_Ranking(p_set, u, k, w, theta);
        }
        if(alg_name.compare("pref")==0){
            // Algorithm: Preference Learning
            Preference_Learning_accuracy(p_set, u, k, w, theta);
        }
        if(alg_name.compare("pref_persist")==0){
            // Algorithm: Preference Learning persist
            Preference_Learning_persist(p_set, u, k, w, theta);
        }
        if(alg_name.compare("uh")==0){
            // the UH-Simplex algorithm
            max_utility(P, u, 2, epsilon, maxRound, cmp_option, stop_option, prune_option, dom_option, w, theta);
        }
    }
    std::cout << "correct count: " << correct_count << std::endl;
    std::cout << "avg question num: "<< question_num/num_repeat << std::endl;
    std::cout << "avg return size: "<< return_size/num_repeat << std::endl;
    std::cout << "avg time: "<< avg_time() << std::endl;

    ofile << "correct count: " << correct_count << endl;
    ofile << "avg question num: "<< question_num/num_repeat << endl;
    ofile << "avg return size: "<< return_size/num_repeat << endl;
    ofile << "avg time: "<< avg_time() << endl;
    ofile.close();

    release_point_set(P, true);
    return 0;
}
