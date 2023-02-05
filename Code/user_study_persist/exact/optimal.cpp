#include "optimal.h"

double Check_set::prod(int i, point_t *p2){
        if(i < this->size()) return dot_prod(this->PS[i], p2);
        return -1;
    }


bool validity_check(Check_set &cs, point_t *u){
    for(int i = 0; i < cs.size() - 1; i++){
        if(dot_prod(cs.PS[i], u) < dot_prod(cs.PS[i+1], u)) return false;
    }
    return true;

}


std::pair<point_t *, point_t *> get_next_point_pair(const std::vector<halfspace_set_t *> &half_set_set, Check_set &cs, int focus){
    if(cs.size() == 0) return make_pair(half_set_set[0]->represent_point[0], half_set_set[1]->represent_point[0]);
    // half_set_set[cs.size()] is our currently focused point
    return make_pair(half_set_set[focus]->represent_point[0], cs.PS[cs.cur]);
}

/**
 * @brief       If a point is inserted into PS
 *              Or if a cycle is handled, reset cs to prepare for the new round
*/
void prepare_next_round(Check_set &cs){
    cs.lower = -1;
    cs.upper = cs.size();
    cs.cur = 0;
}   


int find_point_in_PS(Check_set &cs, const point_t *p){
    for(int i = 0; i < cs.size(); i++){
        if(cs.PS[i]==p) return i;
    }
    cout << __func__ << ": Cannot find point in PS" <<endl;
    return -1;
}


/**
 * @brief       p1 is the focusing point and p2 is the point already in the set
 *              If relation == true, p1 has a higher rank than p2 (p1 should be placed before p2 in PS)
 *              Return true if we finished processing the current point and need to process the next
*/
bool update_check_set(Check_set &cs, point_t * p1, point_t * p2, bool relation, int &cycle_count, point_t *u){
    if(cs.size() == 0){ // base case
        if(relation == true) cs.push_back(p1).push_back(p2);
        else cs.push_back(p2).push_back(p1);
        prepare_next_round(cs);
        return true;
    }
    int idx = find_point_in_PS(cs, p2);
    if(relation == false){
        if(cs.upper < idx){
            // a cycle detected since we know (in terms of index in PS) p1 < upper, p2 < p1, upper < p2
            cs.PS.erase(cs.PS.begin() + idx);
            cs.PS.erase(cs.PS.begin() + cs.upper);
            prepare_next_round(cs);
            cycle_count++;
            return true;
        }
    }
    else{ // p1 has a higher rank
        if(cs.upper > idx) cs.upper = idx;
    }
    ++cs.cur;
    if(cs.cur == cs.size()){
        cs.PS.insert(cs.PS.begin() + cs.upper, p1);
        prepare_next_round(cs);
        return true;
    }
    return false;
}


int Optimal(std::vector<point_t *> p_set, point_t *u, int k){
    int w = 4;
    int dim = p_set[0]->dim;
    vector<point_t *> convh;
    find_convh_vertices(p_set, convh);

    //half_set_set          contains all the partitions(intersection of halfspaces)
    //considered_half_set   contains all the possible partitions considered
    //choose_item_points    contains all the points used to construct hyperplanes(questions)
    //choose_item_set       contains all the hyperplanes(questions) which can be asked user
    std::vector<halfspace_set_t *> half_set_set;
    std::set<int> considered_half_set;   //[i] shows the index in half_set_set
    std::vector<point_t *> choose_item_points;
    std::vector<item *> choose_item_set;
    construct_halfspace_set(convh, choose_item_points, half_set_set, considered_half_set);
    build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
    
    // initialize Region R^0 ... R^k
    vector<Region> regions;
    for(int i=0; i<=k; i++){
        Region r = Region(i);
        regions.push_back(r);
    }
    construct_fragment_in_initialized_R(half_set_set, regions[0].frag_set);

    // key: index of the item in choose_item_set; value: pointer to the hyperplane
    std::map<int, hyperplane_t *> hyperplane_candidates;
    construct_hy_candidates(hyperplane_candidates, choose_item_set);
    int round = 0;
    int focus = 2, cycle_count = 0; // focus = 2 since the non-base case starts at the 3rd point
    bool next_point = false;
    Check_set cs;

    std::set<point_t *> points_return = compute_considered_set(regions);
    while(points_return.size() > w && cycle_count < k && focus < half_set_set.size()){ // when there is no available questions, we stop.
        std::pair<point_t *, point_t *> point_pair = get_next_point_pair(half_set_set, cs, focus);
        point_t* p1 = point_pair.first;
        point_t* p2 = point_pair.second;
        halfspace_t *hs = 0;
        if(dot_prod(u, p1) > dot_prod(u, p2)){ //p1 > p2
            // hs = alloc_halfspace(p2, p1, 0, true);
            if((double) rand()/RAND_MAX > 0.05){ //user answers correctly
                hs = alloc_halfspace(p2, p1, 0, true);
            }
            else{
                hs = alloc_halfspace(p1, p2, 0, true);
            }
        }
        else{
            // hs = alloc_halfspace(p1, p2, 0, true);
            if((double) rand()/RAND_MAX > 0.05){ //user answers correctly
                hs = alloc_halfspace(p1, p2, 0, true);
            }
            else{
                hs = alloc_halfspace(p2, p1, 0, true);
            }
        }
        // update the Check_set
        if(hs->point1 == p2){ // p1 ranks higher than p2
            next_point = update_check_set(cs, p1, p2, true, cycle_count, u);
        }
        else{
            next_point = update_check_set(cs, p1, p2, false, cycle_count, u);
        }
        if(next_point) focus++;
        rt_recurrence(regions, hs);
        release_halfspace(hs);
        std::set<point_t *> considered_points = compute_considered_set(regions);
        if(considered_points.size() == 0) break;
        points_return = considered_points;
        round++;
    }
    // still need to perform regular pruning
    if(points_return.size() > w){
        while(points_return.size() > w){
            int best_idx = find_best_hyperplane(choose_item_set, hyperplane_candidates, regions);
            if(best_idx < 0) break;
            point_t* p1 = choose_item_set[best_idx]->hyper->point1;
            point_t* p2 = choose_item_set[best_idx]->hyper->point2;
            halfspace_t *hs = 0;
            if(dot_prod(u, p1) > dot_prod(u, p2)){ //p1 > p2
                hs = alloc_halfspace(p2, p1, 0, true);
            }
            else{
                hs = alloc_halfspace(p1, p2, 0, true);
            }
            rt_recurrence(regions, hs);
            release_halfspace(hs);
            std::set<point_t *> considered_points = compute_considered_set(regions);
            if(considered_points.size() == 0) break;
            points_return = considered_points;
            round++;
        }

    }

    // free the related data structures
    while(half_set_set.size() > 0){
        halfspace_set_t * hs_s_ptr = half_set_set[half_set_set.size() - 1];
        release_halfspace_set(hs_s_ptr);
        half_set_set.pop_back();
    }
    while(choose_item_set.size() > 0){
        item * item_ptr = choose_item_set[choose_item_set.size() - 1];
        release_item(item_ptr);
        choose_item_set.pop_back();
    }

    //TODO: clear regions
    bool success = check_correctness(points_return, u, best_score);
    if(success) ++correct_count;
    question_num += round;
    return_size += points_return.size();
    return 0;
}