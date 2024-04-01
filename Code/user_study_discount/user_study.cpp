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

void final_price_util_form(std::vector<point_t *> &p_set){
    int num_point = p_set.size();
    int dim = p_set[0]->dim - 1;
    for(int i =0; i < num_point; i++){
        point_t *old_p = p_set[i];
        point_t *p = alloc_point(dim);
        for (int j = 0; j < dim; j++){
            p->coord[j] = old_p->coord[j];
        }
        p->dim = dim;
        p->id = old_p->id;
        p->coord[1] = old_p->coord[1] * old_p->coord[4];
        p->value = old_p->coord[4];
        p_set[i] = p;
        release_point(old_p);
    }

}

int user_study_main_body(){

    // parameters
    int k = 2;
    int w = 5;
    double theta = 0.05;

    srand(time(NULL));
    point_set_t *P0 = read_points((char*)"airbnb_discount.txt");
    //point_set_t *P0 = read_points((char*)"car.txt");

    std::vector<point_t *> p_set, p_1, p_2;
    p_1 = choose_1k(P0);
    int dim = p_1[0]->dim;
    double max[dim], min[dim];
    normalize_set(p_1, max, min);
    skyband(p_1, p_set, 1, P0->numberOfPoints);

    std::vector<point_t *> p_set_final_price, p_1_final_price;
    p_1_final_price = choose_1k(P0);
    final_price_util_form(p_1_final_price);
    int final_price_dim = p_1[0]->dim;
    double final_price_max[final_price_dim], final_price_min[final_price_dim];
    normalize_set(p_1_final_price, final_price_max, final_price_min);
    skyband(p_1_final_price, p_set_final_price, 1, P0->numberOfPoints);

    intro();

    part1_info();
    

    // Setting 1
    print_alg_start(0);
    sampling::sampling(p_set, P0, k, w, SCORE_SELECT, LABELED_PRICE, 0);
    print_alg_end(0);
    
    // Setting 2
    print_alg_start(1);
    sampling::sampling(p_set_final_price, P0, k, w, SCORE_SELECT, LABELED_PRICE, 1);
    print_alg_end(1);
    
    // Setting 3
    print_alg_start(2);
    alg3_info();
    sampling::sampling(p_set_final_price, P0, k, w, SCORE_SELECT, FINAL_PRICE, 2);
    print_alg_end(2);

    part1_end();

    part2_begin();

    int best_idx = -1, possible_best_idx = -1, best_pid = -1;
    std::vector<int> final_cand = load_final_cand(); 
    int cand_num = final_cand.size();
    if(cand_num > 1){
        part2_info_v1_p1();
        display_final_list(P0, final_cand);
        best_idx = ask_favorite_item(cand_num);
        best_pid = final_cand[best_idx];
        for(int i = 0; i < TOT_ALG_COUNT; i++){
            if(std::find(recommendation_list[i].begin(), recommendation_list[i].end(), final_cand[best_idx]) != recommendation_list[i].end()) hit_list[i] = 1;
        }
        part2_info_v1_p2();
        // best_idx = confirm_favorite_item(P0, final_cand, possible_best_idx);
        // dissat_info_best();
        // dissat_score_best = ask_dissat_score_best(P0, final_cand[best_idx]);
        // std::set<int> dissat_lists = find_dissatisfactory_lists(P0, final_cand[best_idx]);
    }
    else{   // only 1 final answer
        best_idx = 0;
        for(int i = 0; i < TOT_ALG_COUNT; i++){
            hit_list[i] = 1;
        }
        best_pid = final_cand[0];
        part2_info_v2();
    }

    part2_second_task();
    higher = higher_than_expect(P0, (best_pid == best_pid_list[0])? best_pid_list[0] : best_pid_list[1]);

    part2_end();
   
    write_summary(final_cand[best_idx]);
    // log_lock();
    end();

    release_point_set(P0, true);
    return 0;

}