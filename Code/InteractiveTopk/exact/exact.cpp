#include "exact.h"


/** @brief      Compute the number of fragments in a Region r
 */
std::vector<point_t *> extract_frags(const Region &r){
    std::vector<point_t *> results;
    for(auto f: r.frag_set) results.push_back(f.first);
    return results;
}


/** @brief      decide whether to stop the algorithm
 */
bool decide_stop(const std::vector<Region> &regions, const int w){
    int k = regions.size() - 1;
    std::set<point_t *> considered_points;
    for(int i=0; i <= k; i++){
        std::vector<point_t *> point_set = extract_frags(regions[i]);
        for(auto p : point_set) considered_points.insert(p);
    }
    return (considered_points.size() <= w);
}


/** @brief      Compute the priority of an item's related hyperplane
 */
double compute_hy_priority(const item *item_ptr, const std::vector<std::vector<point_t*>> &points_in_region){
    int k = points_in_region.size()-1;
    double score = 0;
    for(int i=0; i<=k; ++i){
        double weight = pow(Alpha, i);
        int pos_count = 0, neg_count = 0;
        for(auto f: points_in_region[i]){
            if(item_ptr->pos_points.find(f) != item_ptr->pos_points.end()) pos_count++;
            else if(item_ptr->neg_points.find(f) != item_ptr->neg_points.end()) neg_count++;
        }
        score += weight * min(pos_count, neg_count);
    }
    return score;
}


/** @brief       Find the best hyperplane to ask
 */
int find_best_hyperplane(const std::vector<item *> &choose_item_set, 
                            std::map<int, hyperplane_t *> &hyperplane_candidates, std::vector<Region> &regions){
    int k = regions.size()-1;
    std::vector<std::vector<point_t*>> points_in_region;
    std::vector<int> cand_to_be_removed;
    for(int i=0; i<=k; ++i){
        std::vector<point_t*> frags = extract_frags(regions[i]);
        points_in_region.push_back(frags);
    }
    double highest_priority = 0;
    int highest_index = -1;
    hyperplane_t *hy_res = 0;
    for(auto c : hyperplane_candidates){
        item_t *item_ptr = choose_item_set[c.first];
        double priority = compute_hy_priority(item_ptr, points_in_region);
        if(priority > highest_priority){
            highest_priority = priority;
            highest_index = c.first;
            hy_res = c.second;
        }
        if(priority == 0){ // The hyperplane does not intersect any region. Remove it.
            cand_to_be_removed.push_back(c.first);
        }
    }
    // MUST: remove the selected candidate
    if(highest_index >= 0) hyperplane_candidates.erase(highest_index);
    for(auto ind : cand_to_be_removed) hyperplane_candidates.erase(ind);
    return highest_index;
}


/** @brief      Initialize the hyperplane candidates 
 */
void construct_hy_candidates(std::map<int, hyperplane_t *> &hyperplane_candidates, const std::vector<item *> &choose_item_set){
    for(int i = 0; i<choose_item_set.size(); ++i){
        hyperplane_candidates.insert(make_pair(i, choose_item_set[i]->hyper));
    }
}


/** @brief      construct the partition fragment in the initialized R0/R1/.../Rt 
 */
void construct_fragment_in_initialized_R(const std::vector<halfspace_set_t *> &half_set_set, std::map<point_t *, frag_t *> &frag_set){
    for(auto hss_ptr = half_set_set.cbegin(); hss_ptr != half_set_set.cend(); ++hss_ptr){
        frag_t *frag_ptr = alloc_fragment(); // LEAK
        frag_ptr->point_belongs = (*hss_ptr)->represent_point[0];
        for(auto ext_pt_ptr = (*hss_ptr)->ext_pts.cbegin(); ext_pt_ptr != (*hss_ptr)->ext_pts.cend(); ++ext_pt_ptr){
            frag_ptr->ext_pts.insert(*ext_pt_ptr);
        }
        frag_set.insert(make_pair(frag_ptr->point_belongs, frag_ptr));
    }
}


/** @brief       Record the ext_pt belonging to frag into frag_carry
 */
void record_ext_pt(const frag_t *frag, point_t *ext_pt, std::map<point_t *, frag_t *> &frag_carry){
    point_t *target_point = frag->point_belongs;
    auto crspd_frag = frag_carry.find(target_point);
    if(crspd_frag != frag_carry.end()){ // the fragment belongs to this point is already in frag_return
        (*crspd_frag).second->ext_pts.insert(ext_pt);
    }
    else{
        frag_t *new_frag = alloc_fragment(); // LEAK
        new_frag->point_belongs = target_point;
        new_frag->ext_pts.insert(ext_pt);
        frag_carry.insert(make_pair(target_point, new_frag));
    }
}


/** @brief      Erase the extreme points in Region r that is not supported by hs_ptr
 *              Record the erased points in frag_carry to insert them to other Region later
 */
void erase_fragments(Region &r, halfspace_t *hs_ptr, std::map<point_t *, frag_t *> &frag_carry){
    std::vector<point_t *> frag_remove;
    for(auto frag_ptr = r.frag_set.begin(); frag_ptr != r.frag_set.end(); ++frag_ptr){
        frag_t *frag = (*frag_ptr).second;
        std::vector<point_t*> ext_pt_remove;
        for(auto p_ptr = (*frag).ext_pts.cbegin(); p_ptr != (*frag).ext_pts.cend(); ++p_ptr){
            // if this ext point is not supported, first record it in frag_return, then erase it
            // The normals are outward-pointing, thus remove those dot product > 0
            if(dot_prod(*p_ptr, hs_ptr->normal) > -Precision/2){ 
                record_ext_pt(frag, *p_ptr, frag_carry);
                ext_pt_remove.push_back(*p_ptr);
            }
        }
        for(auto p : ext_pt_remove){
            (*frag).ext_pts.erase(p);
        }
        if((*frag).ext_pts.size() == 0){ // frag is completely outside Rk, record and remove it from Rk later
            frag_remove.push_back((*frag_ptr).first);
        }
    }
    for(auto f: frag_remove){
        auto to_be_rmv = r.frag_set.find(f);
        if(to_be_rmv != r.frag_set.end()){
            delete (*to_be_rmv).second;
            r.frag_set.erase(to_be_rmv);
        }
    }
    return;
}


/** @brief      Add fragments in frag_carry into Region r
 */
void add_fragments(Region &r, std::map<point_t *, frag_t *> &frag_carry){
    for(auto frag_pair : frag_carry){
        point_t *pt = frag_pair.first;
        frag_t *frag = frag_pair.second;
        auto result = r.frag_set.find(pt);
        if(result != r.frag_set.end()){ // the fragment is already inside region r
            for(auto ext_pt : frag->ext_pts){
                (*result).second->ext_pts.insert(ext_pt);
            }
        }
        else{   // the fragement is previously not in r
            frag_t *new_frag = alloc_fragment(); // LEAK
            new_frag->point_belongs = pt;
            for(auto ext_pt : frag->ext_pts){
                new_frag->ext_pts.insert(ext_pt);
            }
            r.frag_set.insert(make_pair(pt,new_frag));
        }
    }
}


/** @brief        Clear the fragments in frag_carry since they are dynamically created
 */
void clear_frag_map(std::map<point_t *, frag_t *> &frag_carry){
    for(auto frag_pair : frag_carry) delete frag_pair.second;
    frag_carry.clear();
}


/** @brief          The recurrence relation of the exact algorithm
 */
void rt_recurrence(std::vector<Region> &regions, halfspace_t* hs_ptr){
    int k = regions.size()-1;
    std::map<point_t *, frag_t *> frag_carry;

    // base case: remove those inside Rk and not supported by hs_ptr
    Region &cur_region = regions[k];
    erase_fragments(cur_region, hs_ptr, frag_carry);
    clear_frag_map(frag_carry);

    // non-base case: first remove frags from cur_region, then add these frags to prev_region
    // e.g., first remove fragments in R0, then add them to R1
    for(int i=k-1; i>=0; --i){
        Region &cur_region = regions[i], &prev_region=regions[i+1];
        erase_fragments(cur_region, hs_ptr, frag_carry);
        add_fragments(prev_region, frag_carry);
        clear_frag_map(frag_carry);
    }
}


/**
 * @brief Build all the partition(intersection of the halfspace), each partition corresponds to a top-1 point
 * @param p_set 				The set containing all the points which are possible to be the top-1 point
 *                              for some utility vectors
 * @param choose_item_points		The set containing points used to build choose_item
 * @param half_set_set			The returned partitions
 * @param considered_half_set	The set recorded all the partitions in the form of index
 */
void construct_halfspace_set(std::vector<point_t *> &p_set, std::vector<point_t *> &choose_item_points,
                             std::vector<halfspace_set_t *> &half_set_set, std::set<int> &considered_half_set)
{
    int M = p_set.size();
    if (M < 2)
    {
        printf("%s\n", "Error: The number of points is smaller than 2.");
        return;
    }
    int dim = p_set[0]->dim;
    halfspace_set_t *half_set;
    //record we use i-th point as the pivot(p[i]>p)
    for (int i = 0; i < M; i++)
    {
        //printf("%d \n", i);
        half_set = R_initial(dim);
        for (int j = 0; j < M; j++)
        {
            if (!is_same_point(p_set[i], p_set[j]))
            {
                //halfspace in the form of <=
                halfspace_t *h = alloc_halfspace(p_set[j], p_set[i], 0, true);
                half_set->halfspaces.push_back(h);
            }
        }
        bool result = get_extreme_pts_refine_halfspaces_alg1(half_set);
        if (result)
        {
            half_set->represent_point.push_back(p_set[i]);
            half_set_set.push_back(half_set);
            choose_item_points.push_back(p_set[i]);
            considered_half_set.insert(half_set_set.size() - 1);
        }
    }
}


/**
 * @brief Build the choose_item table used for selecting the hyperplane(question) asked user
 * @param half_set_set 		All the partitions
 * @param p_set 			All the points which used to build choose_item
 * @param choose_item_set 	The returned choose_item
 * @return The index of the choose_item(hyperplane) which divide the half_set most evenly
 */
int build_choose_item_table(std::vector<halfspace_set_t *> half_set_set, std::vector<point_t *> p_set,
                            std::vector<item *> &choose_item_set)
{
    int M = p_set.size(); //The number of points used to construct hyperplane(questions)
    int H_M = half_set_set.size(); //The number of halfspace sets
    if (M < 2)
    {
        printf("%s\n", "Error: The number of points is smaller than 2.");
        return -1;
    }
    if (H_M < 2)
    {
        printf("%s\n", "Error: The number of partition is smaller than 1.");
        return -1;
    }
    double ES_h = -1000; //Even score for finding hyperplane(question)
    int num_hyperplane = 0; //The index of the chosen hyperplane(question)
    for (int i = 0; i < M; i++)
    {
        for (int j = i + 1; j < M; j++)
        {
            //printf("hyperplane %d %d\n", i, j);
            hyperplane_t *h = alloc_hyperplane(p_set[i], p_set[j], 0);
            //new a choose_item
            item *c_item = alloc_item();
            c_item->hyper = h;

            //classify the halfspace sets to the hyperplane
            for (int k = 0; k < H_M; k++)
            {
                int which_side = check_situation_accelerate(h, half_set_set[k], 0);
                point_t *pt = half_set_set[k]->represent_point[0];
                if (which_side == 1)
                {
                    c_item->positive_side.insert(k);
                    c_item->pos_points.insert(pt);
                }
                else if (which_side == -1)
                {
                    c_item->negative_side.insert(k);
                    c_item->neg_points.insert(pt);
                }
                else if (which_side == 0)
                {
                    c_item->intersect_case.insert(k);
                    c_item->int_points.insert(pt);
                }
                else
                {
                    printf("Error: check side failed.\n");
                    return -1;
                }
            }
            choose_item_set.push_back(c_item);
        }
    }
    return 0;
}


int Exact(std::vector<point_t *> p_set, point_t *u, int k){
    using std::vector;
    int w = 4;
    int dim = p_set[0]->dim;
    vector<point_t *> convh;
    find_convh_vertices(p_set, convh);

    //half_set_set          contains all the partitions(intersection of halfspaces)
    //considered_half_set   contains all the possible partitions considered
    //choose_item_points    contains all the points used to construct hyperplanes(questions)
    //choose_item_set       contains all the hyperplanes(questions) which can be asked user
    //R_half_set            The utility range R
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
    while(!decide_stop(regions, w)){
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
        round++;
    }
    return 0;
}