#include "approx.h"

using std::vector; 
using std::set;
using std::map;

/** @brief      Compute the number of fragments in a Region r
 */
vector<point_t *> extract_frags(const Enclosure &e){
    vector<point_t *> results;
    if(e.frag_set.size() != 0){ // debug purpose
        for(auto f: e.frag_set) results.push_back(f.first);
    }
    return results;
}


/**
 *  @brief      Return true if the point is inside the Enclosure, false otherwise
 *              To accelerate, first check if the point is inside the newly add halfspace
 *              If true, then it is inside the Enclosure
*/
bool check_inside_enclosure(const point_t *ptr, const Enclosure &e, const halfspace_t *new_hs){
    if(dot_prod(ptr, new_hs->normal) < -Precision/2000) return true;
    for(auto hs : e.half_set->halfspaces){
        if(dot_prod(ptr, hs->normal) > Precision/2000) return false;
    }
    return true;
}


/** @brief      Erase the extreme points in Region r that is not supported by hs_ptr
 *              Record the erased points in frag_carry to insert them to other Region later
 */
void enclosure_erase_fragments(Enclosure &e, const halfspace_t *hs_ptr){
    std::vector<point_t *> frag_remove;
    for(auto frag_ptr = e.frag_set.begin(); frag_ptr != e.frag_set.end(); ++frag_ptr){
        frag_t *frag = (*frag_ptr).second;
        std::vector<point_t*> ext_pt_remove;
        for(auto p_ptr = (*frag).ext_pts.cbegin(); p_ptr != (*frag).ext_pts.cend(); ++p_ptr){
            // The normals are outward-pointing, thus remove those dot product > 0
            if(!check_inside_enclosure(*p_ptr, e, hs_ptr)){
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
        auto to_be_rmv = e.frag_set.find(f);
        if(to_be_rmv != e.frag_set.end()){
            delete (*to_be_rmv).second;
            e.frag_set.erase(to_be_rmv);
        }
    }
    return;
}


/** @brief        The recurrence relation of the approx algorithm
 */
void et_recurrence(vector<Enclosure> &enclosures, halfspace_t *hs){
    int k = enclosures.size() - 1;
    enclosures[0].half_set->halfspaces.push_back(hs);
    get_extreme_pts_refine_halfspaces_alg1(enclosures[0].half_set);
    enclosure_erase_fragments(enclosures[0], hs);
}


/** @brief       Find the best hyperplane to ask
 */
int enclosure_find_best_hyperplane(const std::vector<item *> &choose_item_set, 
                            std::map<int, hyperplane_t *> &hyperplane_candidates, std::vector<Enclosure> &enclosures){
    int k = enclosures.size()-1;
    std::vector<std::vector<point_t*>> points_in_region;
    std::vector<int> cand_to_be_removed;
    for(int i=0; i<=k; ++i){
        std::vector<point_t*> frags = extract_frags(enclosures[i]);
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


/** @brief      construct the partition fragment in the initialized R0/R1/.../Rt 
 */
void construct_fragment_in_initialized_Enclosure(const std::vector<halfspace_set_t *> &half_set_set, Enclosure &e, int dim){
    for(auto hss_ptr = half_set_set.cbegin(); hss_ptr != half_set_set.cend(); ++hss_ptr){
        frag_t *frag_ptr = alloc_fragment(); // LEAK
        frag_ptr->point_belongs = (*hss_ptr)->represent_point[0];
        for(auto ext_pt_ptr = (*hss_ptr)->ext_pts.cbegin(); ext_pt_ptr != (*hss_ptr)->ext_pts.cend(); ++ext_pt_ptr){
            frag_ptr->ext_pts.insert(*ext_pt_ptr);
        }
        e.frag_set.insert(make_pair(frag_ptr->point_belongs, frag_ptr));
    }

    e.half_set = R_initial(dim);
    get_extreme_pts_refine_halfspaces_alg1(e.half_set);
}



int Approx(vector<point_t *> p_set, point_t *u, int k){
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

    vector<Enclosure> enclosures;
    for(int i=0; i<=k; i++){
        Enclosure e = Enclosure(i);
        enclosures.push_back(e);
    }
    construct_fragment_in_initialized_Enclosure(half_set_set, enclosures[0], dim);

    // key: index of the item in choose_item_set; value: pointer to the hyperplane
    std::map<int, hyperplane_t *> hyperplane_candidates;
    construct_hy_candidates(hyperplane_candidates, choose_item_set);
    int round = 0;
    while(enclosures[0].frag_set.size() > w){
        int best_idx = enclosure_find_best_hyperplane(choose_item_set, hyperplane_candidates, enclosures);
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
        et_recurrence(enclosures, hs);
        round++;
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
    //TODO clear the enclosures

    return 0;
}   