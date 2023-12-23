#include "rev_HDPI.h"
#include <sys/time.h>

int total_ext_pt(std::vector<halfspace_set_t *> &half_set_set){
    int total_num = 0;
    for(auto hss_ptr = half_set_set.cbegin(); hss_ptr != half_set_set.cend(); hss_ptr++){
        total_num += (*hss_ptr)->ext_pts.size();
    }
    return total_num;
}

/**
 * @brief Build all the partition(intersection of the halfspace), each partition corresponds to a top-1 point
 * @param p_set 				The set containing all the points which are possible to be the top-1 point
 *                              for some utility vectors
 * @param choose_item_points		The set containing points used to build choose_item
 * @param half_set_set			The returned partitions
 * @param considered_half_set	The set recorded all the partitions in the form of index
 */
void rev_construct_halfspace_set(std::vector<point_t *> &p_set, std::vector<point_t *> &choose_item_points,
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
int rev_build_choose_item_table(std::vector<halfspace_set_t *> half_set_set, std::vector<point_t *> p_set,
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
                if (which_side == 1)
                {
                    c_item->positive_side.insert(k);
                }
                else if (which_side == -1)
                {
                    c_item->negative_side.insert(k);
                }
                else if (which_side == 0)
                {
                    c_item->intersect_case.insert(k);
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


int get_best_hyperplane(const std::vector<item *> &choose_item_set){
    double best_even_score = -1000;
    auto best_item_it = choose_item_set.cbegin();
    for(auto item_it = choose_item_set.cbegin(); item_it != choose_item_set.cend(); ++item_it){
        int pos = (*item_it)->positive_side.size();
        int neg = (*item_it)->negative_side.size();
        int intersect = (*item_it)->intersect_case.size();
        double even_score = std::min(pos, neg) - (Beta * intersect);
        if(even_score > best_even_score){
            best_even_score = even_score;
            best_item_it = item_it;
        }
    }
    return best_item_it - choose_item_set.cbegin();
}


/**
 * @brief   Based on the answer from the user, modify the choose_item table
 *          This function does the following:
 *          1. Update considered_half_set to remove those partitions that are on the negative side of the tested halfspace h (choose_item_set[H_num])
 *          2. Delete all the removed partitions from other hyperplanes in choose_item_set
 *          3. Update all partitions that intersect with h
 *          4. Update the relation between the updated partition and other hyperplanes in choose_item_set
 * @param choose_item_set 		The choose_item table which will be modified
 * @param half_set_set 			All the partitions
 * @param considered_half_set 	Based on the user's answer, the partitions still considered
 * @param H_num					The choose_item used for asking question
 * @param direction 			The user's answer(which side)
 *                              -true on the positive side
 *                              -false on the negative side
 * @return The choose_item(hyperplane) which divide the half_set most evenly
 */
int rev_modify_choose_item_table(std::vector<item *> &choose_item_set,
                             std::vector<halfspace_set_t *> half_set_set, std::set<int> &considered_half_set,
                             const int H_num, const bool direction)
{
    int M = choose_item_set.size();
    if (M < 1)
    {
        printf("%s\n", "Error: no item is in the choose_item_table.");
        return -1;
    }

    item *target_item = choose_item_set[H_num];
    // refine the considered half_set
    // 1. Update considered_half_set to remove those partitions that are on the negative side of the tested halfspace h (choose_item_set[H_num])
    if (direction == true)//delete all the negative side partitions
    {
        for(auto idx = target_item->negative_side.cbegin(); idx != target_item->negative_side.cend(); ++idx){
            considered_half_set.erase(*idx);
        }
    }
    else
    {
         for(auto idx = target_item->positive_side.cbegin(); idx != target_item->positive_side.cend(); ++idx){
            considered_half_set.erase(*idx);
        }
    }

    // delete all the half_sets which are on the non-direction side
    // 2. Delete all the removed partitions from other hyperplanes in choose_item_set
    if (direction)
    {
        //delete negative side
        for (int j = 0; j < M; j++)//scan all the choose_item, j-th choose_item
        {
            if (H_num != j)
            {
                for(auto idx = target_item->negative_side.cbegin(); idx != target_item->negative_side.cend(); ++idx){
                    choose_item_set[j]->positive_side.erase(*idx);
                    choose_item_set[j]->negative_side.erase(*idx);
                    choose_item_set[j]->intersect_case.erase(*idx);
                }
            }
        }
    }
    else
    {
        //delete positive side
        for (int j = 0; j < M; j++)//scan all the choose_item, j-th choose_item
        {
            if (H_num != j)
            {
                for(auto idx = target_item->positive_side.cbegin(); idx != target_item->positive_side.cend(); ++idx){
                    choose_item_set[j]->positive_side.erase(*idx);
                    choose_item_set[j]->negative_side.erase(*idx);
                    choose_item_set[j]->intersect_case.erase(*idx);
                }
            }
        }
    }

    // 3. Update all partitions that intersects with h
    // build a halfspace based on choose_item[H_num]
    halfspace_t *half;
    if (direction == true)
    {
        half = alloc_halfspace(target_item->hyper->point2, target_item->hyper->point1, 0, true);
    }
    else
    {
        half = alloc_halfspace(target_item->hyper->point1, target_item->hyper->point2, 0, true);
    }

    // update all the half_set in the intersected case
    for (auto idx = target_item->intersect_case.cbegin(); idx != target_item->intersect_case.cend(); idx++)
    {
        //num_set: index of the halfspace_set in half_set_set
        half_set_set[*idx]->halfspaces.push_back(half);
        get_extreme_pts_refine_halfspaces_alg1(half_set_set[*idx]);
    }

    // 4. Update the relation between the updated partition and other hyperplanes in choose_item_set
    // deal with the refined half_set 
    // check the updated intersected half_set and see if it still intersect with some other hyperplane h
    // if not, update this half_set to the positive/negative side of h
    for (auto idx = target_item->intersect_case.cbegin(); idx != target_item->intersect_case.cend(); idx++)
    {
        //scan all the choose_item, j-th choose_item
        for (int j = 0; j < M; j++)
        {
            if (H_num != j)
            {
                if(choose_item_set[j]->intersect_case.find(*idx) != choose_item_set[j]->intersect_case.end()){
                    int which_side = check_situation_accelerate(choose_item_set[j]->hyper, half_set_set[*idx],0);
                    if (which_side == -2)
                    {
                        printf("%s\n", "Error: check side failed.");
                        return -1;
                    }
                    if (which_side != 0){
                        choose_item_set[j]->intersect_case.erase(*idx);
                        if(which_side == 1){// the half_set is on the positive side
                            choose_item_set[j]->positive_side.insert(*idx);
                        }
                        else{// the half_set is on the negative side
                            choose_item_set[j]->negative_side.insert(*idx);
                        }
                    }
                }
            }
        }
    }
    //refine the choose_item_set and select the hyperplane asked user
    choose_item_set.erase(choose_item_set.begin() + H_num);
    return 0;
}


/**
 * @brief Asking user question and return one of the top-k point
 *        Find the top-1 point accurately convex Hull
 * @param p_set 		 The dataset
 * @param u 			 The linear function
 * @param k 			 The parameter
 */
int rev_HDPI(std::vector<point_t *> p_set, point_set_t *P0, int w, int alg_id, int max_round){
    reset_stats();
    start_timer();
    int round = 0;
    //p_set_1 contains the points which are not dominated by >=1 points
    //p_set_k contains the points which are not dominated by >=k points
    //p_top_1 contains the points which are the convex points
    std::vector<point_t *> p_set_1, top;
    //onion(p_set, p_set_1, 1);
    int dim = p_set[0]->dim;
    find_top1(p_set, top);
    skyline_c(top, p_set_1);
    //half_set_set          contains all the partitions(intersection of halfspaces)
    //considered_half_set   contains all the possible partitions considered
    //choose_item_points    contains all the points used to construct hyperplanes(questions)
    //choose_item_set       contains all the hyperplanes(questions) which can be asked user
    //R_half_set            The utility range R
    std::vector<halfspace_set_t *> half_set_set;
    std::set<int> considered_half_set;   //[i] shows the index in half_set_set
    std::vector<point_t *> choose_item_points;
    std::vector<item *> choose_item_set;
    rev_construct_halfspace_set(p_set_1, choose_item_points, half_set_set, considered_half_set);

    //index the index of the chosen hyperplane(question)
    //v1    the utility of point 1
    //v2    the utility of point 2
    rev_build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
    int index = get_best_hyperplane(choose_item_set);

    stop_timer();
    int opt = show_to_user(P0, choose_item_set[index]->hyper->point1->id, choose_item_set[index]->hyper->point2->id);
    round++;
    start_timer();

    //initial
    halfspace_set_t *R_half_set = R_initial(dim);
    get_extreme_pts_refine_halfspaces_alg1(R_half_set);
    halfspace_t *hy;
    point_t* point_result = NULL;
    while (considered_half_set.size() > w && round < max_round)
    {
        if (opt == 1)
        {
            hy = alloc_halfspace(choose_item_set[index]->hyper->point2, choose_item_set[index]->hyper->point1, 0, true);
            rev_modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, true);
        }
        else
        {
            hy = alloc_halfspace(choose_item_set[index]->hyper->point1, choose_item_set[index]->hyper->point2, 0, true);
            rev_modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, false);
        }
        index = get_best_hyperplane(choose_item_set);

        //Find whether there exist point which is the topk point w.r.t any u in R
        R_half_set->halfspaces.push_back(hy);
        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
        std::vector<point_t *> top_current;
        point_result = NULL;
        bool check_result = find_possible_topk(p_set, R_half_set, 1, top_current);
        if (check_result)
        {
            point_result = check_possible_topk(p_set, R_half_set, 1, top_current);
        }
        if (point_result!= NULL)
        {
            break;
        }
        if(considered_half_set.size() > w){
            stop_timer();
            opt = show_to_user(P0, choose_item_set[index]->hyper->point1->id, choose_item_set[index]->hyper->point2->id);
            round++;
            start_timer();
        }
    }
    stop_timer();
    //get the returned points
    vector<point_t *> points_return;
    for(auto idx : considered_half_set) points_return.push_back(half_set_set[idx]->represent_point[0]);
    if (points_return.size() > w) points_return = sampleVector(points_return, w);
    // free the related data structures
    release_halfspace_set(R_half_set);
    while(choose_item_set.size() > 0){
        item * item_ptr = choose_item_set[choose_item_set.size() - 1];
        release_item(item_ptr);
        choose_item_set.pop_back();
    }
    print_result_list(P0, points_return);
    int alg_best = alg_top1_select(points_return);
    question_asked_list[alg_id] = round;
    best_pid_list[alg_id] = points_return[alg_best]->id; 
    proc_time_list[alg_id] = avg_time(round);
    return_size_list[alg_id] = points_return.size();
    write_results_to_file(alg_id, points_return, alg_best);
    record_ques_history(alg_id);
    for(auto p : points_return) recommendation_list[alg_id].push_back(p->id);
    return 0;
}
