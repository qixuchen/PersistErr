#include "user_study.h"


std::vector<point_t *> random_choose(point_set_t *p_set)
{
    std::vector<point_t *> p;
    int count = 0, index = 0, dim = p_set->points[0]->dim;
    while(count < 1000)
    {
        index = rand() % p_set->numberOfPoints;
        if(1000 < p_set->points[index]->coord[0] && p_set->points[index]->coord[0] < 100000
            && 2001 < p_set->points[index]->coord[1] && p_set->points[index]->coord[1]< 2017
            && 50 < p_set->points[index]->coord[2] && p_set->points[index]->coord[2]< 1000
            && 5000 < p_set->points[index]->coord[3] && p_set->points[index]->coord[3]< 150000
        )
        {
            bool is_same = false;
            for (int j = 0; j < p.size(); j++)
            {
                if (is_same_point(p_set->points[index], p[j]))
                {
                    is_same = true;
                    break;
                }
            }
            if (!is_same)
            {
                point_t *pp = alloc_point(dim);
                for (int i = 0; i < dim; i++)
                {
                    pp->coord[i] = p_set->points[index]->coord[i];
                }
                pp->dim = p_set->points[index]->dim;
                pp->id = p_set->points[index]->id;
                p.push_back(pp);
                count++;
            }
        }
    }
    return p;
}


std::vector<point_t *> choose_1k(point_set_t *p_set)
{
    std::vector<point_t *> p;
    int count = 0, index = 0, dim = p_set->points[0]->dim;
    while(count < 1000)
    {
        // if(1000 < p_set->points[index]->coord[0] && p_set->points[index]->coord[0] < 100000
        //     && 2001 < p_set->points[index]->coord[1] && p_set->points[index]->coord[1]< 2017
        //     && 50 < p_set->points[index]->coord[2] && p_set->points[index]->coord[2]< 1000
        //     && 5000 < p_set->points[index]->coord[3] && p_set->points[index]->coord[3]< 150000
        // )
        // {
        bool is_same = false;
        for (int j = 0; j < p.size(); j++)
        {
            if (is_same_point(p_set->points[index], p[j]))
            {
                is_same = true;
                break;
            }
        }
        if (!is_same)
        {
            point_t *pp = alloc_point(dim);
            for (int i = 0; i < dim; i++)
            {
                pp->coord[i] = p_set->points[index]->coord[i];
            }
            pp->dim = p_set->points[index]->dim;
            pp->id = p_set->points[index]->id;
            p.push_back(pp);
            count++;
        }
        // }
        ++index;
    }
    return p;
}


void normalize_set(std::vector<point_t *> p_set, double *max, double *min)
{
    int dim = p_set[0]->dim;
    for (int j = 0; j < dim; j++)
    {
        max[j] = -1;
        min[j] = 999999;
    }
    for (int i = 0; i < p_set.size(); i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (max[j] < p_set[i]->coord[j])
            {
                max[j] = p_set[i]->coord[j];
            }
            if (min[j] > p_set[i]->coord[j])
            {
                min[j] = p_set[i]->coord[j];
            }
        }
    }
    for (int i = 0; i < p_set.size(); i++)
    {
        for (int j = 0; j < dim; j++)
        {
            // if (j == 0 || j == 3)
            if(j == 0)
            {
                p_set[i]->coord[j] = 1 - (p_set[i]->coord[j] - min[j]) / (max[j] - min[j]);
            }
            else
            {
                p_set[i]->coord[j] = (p_set[i]->coord[j] - min[j]) / (max[j] - min[j]);
            }
        }
    }
}


std::vector<point_t *> scale_up(std::vector<point_t *> p_set)
{
    int dim = p_set[0]->dim;
    std::vector<point_t *> P;
    for (int i = 0; i < p_set.size(); i++)
    {
        point_t *p = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            p->coord[j] = p_set[i]->coord[j] * 1000;
        }
        p->id = p_set[i]->id;
        p->dim = p_set[i]->dim;
        P.push_back(p);
    }
    return P;
}


std::vector<int> load_final_cand(){
    std::set<int> final_set;
    for(int i=0; i < TOT_ALG_COUNT; i++){
        final_set.insert(best_pid_list[i]);
    }

    std::vector<int> final_list;
    for(auto pid: final_set){
        final_list.push_back(pid);
    }
    return final_list;
}


int user_study_main_body(){

    // parameters
    int k = 2;
    int w = 5;
    double theta = 0.05;

    srand(time(NULL));
    point_set_t *P0 = read_points((char*)"airbnb.txt");
    //point_set_t *P0 = read_points((char*)"car.txt");

    std::vector<point_t *> p_set, p_1, p_2;
    p_1 = choose_1k(P0);
    int dim = p_1[0]->dim;
    double max[dim], min[dim];
    normalize_set(p_1, max, min);
    skyband(p_1, p_set, 1, P0->numberOfPoints);
    // p_2 = scale_up(p_set);
    point_set_t *P = point_reload(p_set);

    intro();

    if(jump <= 1){
        part1_info();
    }
    if(jump <= 1){
        // the exact Algorithm
        print_alg_start(0);
        exact_rev::Exact_revised(p_set, P0, k, w, SCORE_SELECT, 0);
        print_alg_end(0);
    }
    if(jump <= 2){
        // the sampling Algorithm
        print_alg_start(1);
        sampling::sampling(p_set, P0, k, w, SCORE_SELECT, 1);
        print_alg_end(1);
    }
    if(jump <= 3){
        // the HDPI Algorithm
        print_alg_start(2);
        rev_HDPI(p_set, P0, w, 2);
        print_alg_end(2);
    }
    if(jump <= 4){
        // PointPrune Algorithm
        print_alg_start(3);
        PointPrune_v2(p_set, P0, 3, w, max, min, 3);
        print_alg_end(3);
    }
    if(jump <= 5){
        // Active Ranking Algorithm
        print_alg_start(4);
        Active_Ranking(p_set, P0, w, 4);
        print_alg_end(4);
    }
    if(jump <= 6){
        // Preference Learning Algorithm
        print_alg_start(5);
        Preference_Learning(p_set, P0, w, 5);
        print_alg_end(5);
    }

    part1_end();

    int best_idx = -1, possible_best_idx = -1;
    std::vector<int> final_cand = load_final_cand(); 
    int cand_num = final_cand.size();
    if(cand_num > 1){
        part2_info();
        display_final_list(P0, final_cand);
        possible_best_idx = ask_favorite_item(cand_num);
        best_idx = confirm_favorite_item(P0, final_cand, possible_best_idx);
        dissat_info_best();
        dissat_score_best = ask_dissat_score_best(P0, final_cand[best_idx]);
        std::set<int> dissat_lists = find_dissatisfactory_lists(P0, final_cand[best_idx]);
        for(int i = 0; i < TOT_ALG_COUNT; i++){
            if(dissat_lists.find(i) == dissat_lists.end()) hit_list[i] = 1;
        }
        if(dissat_lists.size() > 0 && dissat_score_best < 10){
            dissat_info(dissat_score_best);
        }
        if(dissat_score_best == 10){
            for(int i = 0; i < TOT_ALG_COUNT; i++){
                dissat_score_list[i] = dissat_score_best;
            }
        }
        else{
            for(int i = 0; i < TOT_ALG_COUNT; i++){
                if(dissat_lists.find(i) == dissat_lists.end()){
                    dissat_score_list[i] = dissat_score_best;
                }
                else{
                    dissat_score_list[i] = ask_dissat_score(P0, recommendation_list[i], dissat_score_best);
                }
            }
        }
        part2_end();
    }
    else{   // only 1 final answer
        best_idx = 0;
        for(int i = 0; i < TOT_ALG_COUNT; i++){
            hit_list[i] = 1;
        }
        cout << "===============================================================================================" << endl << endl;
        cout << "                                     Beginning of Part 2" << endl << endl;
        cout << "===============================================================================================" << endl << endl;
        dissat_info_best();
        dissat_score_best = ask_dissat_score_best(P0, final_cand[0]);
        for(int i = 0; i < TOT_ALG_COUNT; i++){
            dissat_score_list[i] = dissat_score_best;
        }
        part2_end();
    }
   
    write_summary(final_cand[best_idx]);

    end();
    
    release_point_set(P, true);
    release_point_set(P0, true);
    return 0;

}