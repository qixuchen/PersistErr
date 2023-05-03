#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "HDPI/HDPI.h"
#include "rev_HDPI/rev_HDPI.h"
#include "exact/Exact_revised.h"
#include "sampling/sampling.h"
#include "sampling/sampling_listwise.h"
#include "optimal/optimal.h"
#include "optimal/optimal_question.h"
#include "RH/alg_one.h"
#include "2DPI/2dPI.h"
#include "Others/qhull_build.h"
#include <vector>
#include <ctime>
#include <iostream>
#include <sys/time.h>
#include <string>
#include <fstream>

int main(int argc, char *argv[])
{
    int num_repeat;
    string input_file, alg_name;
    if(argc == 1){
        input_file = "4d100k.txt";
        alg_name = "optimal_sup_inf";
        num_repeat = 10;
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
    
    string ofile_name = string("./results/") + alg_name + "_" + input_file + "_" + to_string(num_repeat) + ".txt";
    ofstream ofile;
    ofile.open(ofile_name);

    // parameters
    int k = 2;
    int w = 5;
    double theta = 0.05;
    int s = 10;

    srand(time(NULL));
    point_set_t *P0 = read_points((char*) input_file.c_str());
    cout << alg_name << endl;
    int dim = P0->points[0]->dim; //obtain the dimension of the point
    std::vector<point_t *> p_set, p0;
    skyband(P0, p_set, 1);
    // cout << "skyline size: " << p_set.size() << endl;
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

        if(alg_name.compare("exact") == 0){
            exact_rev::Exact_revised(p_set, u, k, w, SCORE_SELECT, theta);
        }
        if(alg_name.compare("sampling") == 0){
            sampling::sampling(p_set, u, k, w, RAND_SELECT, theta);
        }
        if(alg_name.compare("sampling_listwise") == 0){
            sampling_listwise::sampling_listwise(p_set, u, k, w, SCORE_SELECT, theta, s);
        }
        if(alg_name.compare("sampling_sup_inf") == 0){
            sampling_listwise::sampling_sup_inf(p_set, u, k, w, SCORE_SELECT, theta, s);
        }
        if(alg_name.compare("optimal") == 0){
            optimal::optimal(p_set, u, k, w, SCORE_SELECT, theta);
        }
        if(alg_name.compare("optimal_listwise") == 0){
            optimal_question::optimal_listwise(p_set, u, k, w, SCORE_SELECT, theta, s);
        }
        if(alg_name.compare("optimal_sup_inf") == 0){
            optimal_question::optimal_sup_inf(p_set, u, k, w, SCORE_SELECT, theta, s);
        }
    }
    

    cout << "correct count: " << correct_count << endl;
    cout << "avg question num: "<< question_num/num_repeat << endl;
    cout << "avg return size: "<< return_size/num_repeat << endl;
    cout << "avg time: "<< avg_time() << endl;
    // cout << "preprocessing time: " << total_time()/num_repeat << endl;

    ofile << "correct count: " << correct_count << endl;
    ofile << "avg question num: "<< question_num/num_repeat << endl;
    ofile << "avg return size: "<< return_size/num_repeat << endl;
    ofile << "avg time: "<< avg_time() << endl;
    // ofile << "preprocessing time: " << total_time()/num_repeat << endl;
    ofile.close();
    release_point_set(P, true);
    return 0;
}
