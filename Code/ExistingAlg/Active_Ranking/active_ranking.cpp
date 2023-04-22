#include "active_ranking.h"
#include <sys/time.h>

/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
int Active_Ranking(std::vector<point_t *> p_set, point_t *u, int k, int w, double theta)
{
    int dim = p_set[0]->dim;
    int round = 0;
    point_random(p_set);

    start_timer();
    //initial
    //add the basic halfspace into R_hyperplane
    halfspace_set_t *R_half_set = R_initial(dim);
    int M = p_set.size();
    std::vector<point_t *> current_use;
    current_use.push_back(p_set[0]); //store all the points in order
    for (int i = 1; i < M; i++) //compare: p_set contains all the points
    {
        bool same_exist = false;
        for (int j = 0; j < current_use.size(); j++)
        {
            if (is_same_point(p_set[i], current_use[j]))//if there exist a same point in the current_use, insert p_set[i]
            {
                same_exist = true;
                current_use.insert(current_use.begin() + j, p_set[i]);
                break;
            }
        }
        if (!same_exist)
        {
            int num_point = current_use.size();
            int place = 0; //the place of the point inserted into the current_use
            //find the question asked user
            for (int j = 0; j < num_point; j++)
            {
                hyperplane_t *h = alloc_hyperplane(p_set[i], current_use[j], 0);
                int relation = check_situation_accelerate(h, R_half_set, 0);
                release_hyperplane(h);
                //if intersect, calculate the distance
                if (relation == 0)
                {
                    round++;
                    double v1 = dot_prod(u, p_set[i]);
                    double v2 = dot_prod(u, current_use[j]);
                    if (v1 > v2)
                    {
                        halfspace_t *half = 0;
                        if((double) rand()/RAND_MAX > theta) half = alloc_halfspace(current_use[j], p_set[i], 0, true);
                        else half = alloc_halfspace(p_set[i], current_use[j], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
                    }
                    else
                    {
                        halfspace_t *half = 0;
                        if((double) rand()/RAND_MAX > theta) half = alloc_halfspace(p_set[i], current_use[j], 0, true);
                        else half = alloc_halfspace(current_use[j], p_set[i], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
                        place = j + 1;
                    }
                }
                else if (relation == -1)
                {
                    place = j + 1;
                }
            }
            current_use.insert(current_use.begin() + place, p_set[i]);
        }
    }
    stop_timer();
    p_set.clear();
    release_halfspace_set(R_half_set);
    
    //get the returned points
    int output_size = min(w, int(current_use.size()));
    bool best_point_included = false;
    for(int i=0; i < output_size; i++){
        if(dot_prod(u, current_use[i]) >= best_score){
            best_point_included = true;
            break;
        }
    }
    correct_count += best_point_included;
    question_num += round;
    return_size += output_size;
    return 0;
}

point_t* compare_random(point_t *u, double theta, point_t *a, point_t *b){
    double v1 = dot_prod(u, a);
    double v2 = dot_prod(u, b);
    if (v1 > v2)
    {
        if((double) rand()/RAND_MAX > theta){
            return a;
        }
        else{
            return b;
        }
    }
    else{
        if((double) rand()/RAND_MAX < theta){
            return a;
        }
        else{
            return b;
        }
    }
}

point_t* sample_compare_random(point_t *u, double theta, point_t *p, point_t *q, std::vector<point_t *> &p_set, int R, int &round){
    int n = p_set.size();
    std::set<point_t *> samples;
    while(samples.size() < R){ // sample R points from p_set
        int idx = rand() % n;
        if(!is_same_point(p_set[idx], p) && !is_same_point(p_set[idx], q) && samples.find(p_set[idx]) == samples.end()){
            samples.insert(p_set[idx]);
        }
    }  
    int p_vote = 0;
    round++;
    if(compare_random(u, theta, p, q) == p) p_vote++;
    else p_vote--;
    for(auto s : samples){
        point_t *w1 = 0, *w2 = 0;
        round++;
        w1 = compare_random(u, theta, s, p);
        round++;
        w2 = compare_random(u, theta, s, q);
        if(w1 == p && w2 == s) p_vote++;
        else if(w1 == s && w2 == q) p_vote--;
    }
    return (p_vote > 0)? p : q;
}


point_t* compare_persist(point_t *u, double theta, point_t *a, point_t *b, std::set<std::pair<point_t *, point_t *>> &asked_pairs){
    if(asked_pairs.find(make_pair(a, b)) != asked_pairs.end()){
        return a;
    } 
    else if(asked_pairs.find(make_pair(b, a)) != asked_pairs.end()){
        return b;
    }
    else{
        double v1 = dot_prod(u, a);
        double v2 = dot_prod(u, b);
        if (v1 > v2)
        {
            if((double) rand()/RAND_MAX > theta){
                asked_pairs.insert(make_pair(a, b));
                return a;
            }
            else{
                asked_pairs.insert(make_pair(b, a));
                return b;
            }
        }
        else{
            if((double) rand()/RAND_MAX < theta){
                asked_pairs.insert(make_pair(a, b));
                return a;
            }
            else{
                asked_pairs.insert(make_pair(b, a));
                return b;
            }
        }
    }
}

point_t* sample_compare_persist(point_t *u, double theta, point_t *p, point_t *q, std::vector<point_t *> &p_set,  std::set<std::pair<point_t *, point_t *>> &asked_pairs, int R, int &round){
    int n = p_set.size();
    std::set<point_t *> samples;
    while(samples.size() < R){ // sample R points from p_set
        int idx = rand() % n;
        if(!is_same_point(p_set[idx], p) && !is_same_point(p_set[idx], q) && samples.find(p_set[idx]) == samples.end()){
            samples.insert(p_set[idx]);
        }
    }  
    int p_vote = 0;
    round++;
    if(compare_persist(u, theta, p, q, asked_pairs) == p) p_vote++;
    else p_vote--;
    for(auto s : samples){
        point_t *w1 = 0, *w2 = 0;
        round++;
        w1 = compare_persist(u, theta, s, p, asked_pairs);
        round++;
        w2 = compare_persist(u, theta, s, q, asked_pairs);
        if(w1 == p && w2 == s) p_vote++;
        else if(w1 == s && w2 == q) p_vote--;
    }
    return (p_vote > 0)? p : q;
}


/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
int Active_Ranking_persist(std::vector<point_t *> p_set, point_t *u, int k, int w, double theta)
{
    int dim = p_set[0]->dim;
    int round = 0;
    int R = 5;
    point_random(p_set);
    std::set<std::pair<point_t *, point_t *>> asked_pairs;

    start_timer();
    //initial
    //add the basic halfspace into R_hyperplane
    halfspace_set_t *R_half_set = R_initial(dim);
    int M = p_set.size();
    std::vector<point_t *> current_use;
    current_use.push_back(p_set[0]); //store all the points in order
    for (int i = 1; i < M; i++) //compare: p_set contains all the points
    {
        bool same_exist = false;
        for (int j = 0; j < current_use.size(); j++)
        {
            if (is_same_point(p_set[i], current_use[j]))//if there exist a same point in the current_use, insert p_set[i]
            {
                same_exist = true;
                current_use.insert(current_use.begin() + j, p_set[i]);
                break;
            }
        }
        if (!same_exist)
        {
            int num_point = current_use.size();
            int place = 0; //the place of the point inserted into the current_use
            //find the question asked user
            for (int j = 0; j < num_point; j++)
            {
                hyperplane_t *h = alloc_hyperplane(p_set[i], current_use[j], 0);
                int relation = check_situation_accelerate(h, R_half_set, 0);
                release_hyperplane(h);
                //if intersect, calculate the distance
                if (relation == 0)
                {
                    point_t *p = sample_compare_random(u, theta, p_set[i], current_use[j], p_set, R, round);
                    // point_t *p = sample_compare_persist(u, theta, p_set[i], current_use[j], p_set, asked_pairs, R, round);
                     halfspace_t *half = 0;
                    if (p == p_set[i])
                    {
                        half = alloc_halfspace(current_use[j], p_set[i], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
                    }
                    else
                    {
                        half = alloc_halfspace(p_set[i], current_use[j], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
                        place = j + 1;
                    }
                }
                else if (relation == -1)
                {
                    place = j + 1;
                }
            }
            current_use.insert(current_use.begin() + place, p_set[i]);
        }
    }
    stop_timer();
    p_set.clear();
    release_halfspace_set(R_half_set);
    
    //get the returned points
    int output_size = min(w, int(current_use.size()));
    bool best_point_included = false;
    for(int i=0; i < output_size; i++){
        if(dot_prod(u, current_use[i]) >= best_score){
            best_point_included = true;
            break;
        }
    }
    correct_count += best_point_included;
    question_num += round;
    return_size += output_size;
    return 0;
}