#include "Exact_revised.h"


namespace exact_rev{

    bool check_correctness(const std::set<point_t *> &considered_points, point_t *u, double best_score){
        for(auto p : considered_points){
            if(dot_prod(u, p) == best_score) return true;
        }
        return false;
    }


    /** @brief compute the set of points whose partition intersects with R^0 ... R^k
    */
    std::set<point_t *> compute_considered_set(const std::vector<conf_region> &conf_regions){
        std::set<point_t *> point_return;
        int k = conf_regions.size() - 1;
        for(int i = 0; i <= k; i++){
            point_return.insert(conf_regions[i].points.begin(), conf_regions[i].points.end());
        }
        return point_return;
    }

    /** @brief      Initialize the hyperplane candidates 
     */
    void construct_hy_candidates(std::map<int, hyperplane_t *> &hyperplane_candidates, const std::vector<item *> &choose_item_set){
        for(int i = 0; i<choose_item_set.size(); ++i){
            hyperplane_candidates.insert(make_pair(i, choose_item_set[i]->hyper));
        }
    }


    /** @brief      Compute the priority of an item's related hyperplane
     */
    double compute_hy_priority(const item *item_ptr, const std::vector<std::vector<point_t*>> &points_in_region){
        int k = points_in_region.size()-1;
        double pos_score = 0, neg_score = 0;
        std::unordered_set<point_t*> processed;
        for(int i=0; i<=k; ++i){
            double weight = pow(Alpha, i);
            int pos_count = 0, neg_count = 0;
            for(auto f: points_in_region[i]){
                if(processed.find(f) != processed.end()) continue;
                if(item_ptr->pos_points.find(f) != item_ptr->pos_points.end()) pos_count++;
                else if(item_ptr->neg_points.find(f) != item_ptr->neg_points.end()) neg_count++;
                processed.insert(f);
            }
            pos_score += weight * pos_count;
            neg_score += weight * neg_count;
        }
        return min(pos_score, neg_score);
    }


    /** @brief       Find the best hyperplane to ask
     */
    int find_best_hyperplane(const std::vector<item *> &choose_item_set, 
                                std::map<int, hyperplane_t *> &hyperplane_candidates, const std::vector<conf_region> &conf_regions){
        int k = conf_regions.size() - 1;
        std::vector<std::vector<point_t*>> points_in_region;
        std::vector<int> cand_to_be_removed;
        for(int i=0; i<=k; ++i){
            std::vector<point_t*> frags(conf_regions[i].points.begin(), conf_regions[i].points.end());
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


    /** @brief      Compute the priority of an item's related hyperplane
     */
    double compute_hy_priority_update_upper_bound(item *item_ptr, const std::vector<std::vector<point_t*>> &points_in_region){
        int k = points_in_region.size()-1;
        double pos_score = 0, neg_score = 0;
        std::unordered_set<point_t*> processed;
        for(int i=0; i<=k; ++i){
            double weight = pow(Alpha, i);
            int pos_count = 0, neg_count = 0;
            for(auto f: points_in_region[i]){
                if(processed.find(f) != processed.end()) continue;
                if(item_ptr->pos_points.find(f) != item_ptr->pos_points.end()) pos_count++;
                else if(item_ptr->neg_points.find(f) != item_ptr->neg_points.end()) neg_count++;
                processed.insert(f);
            }
            pos_score += weight * pos_count;
            neg_score += weight * neg_count;
        }

        double score = min(pos_score, neg_score);
        // update the upper bound of this item
        item_ptr->upper_bound = score;
        return score;
    }


    /** @brief       Find the best hyperplane to ask
     *               Using lazy updating trick
     */
    int find_best_hyperplane_lazy_update(std::vector<item *> &choose_item_set, 
                                std::map<int, hyperplane_t *> &hyperplane_candidates, const std::vector<conf_region> &conf_regions){
        int k = conf_regions.size() - 1;
        std::vector<std::vector<point_t*>> points_in_region;
        std::vector<int> cand_to_be_removed;
        for(int i=0; i<=k; ++i){
            std::vector<point_t*> frags(conf_regions[i].points.begin(), conf_regions[i].points.end());
            points_in_region.push_back(frags);
        }
        double highest_priority = 0;
        int highest_index = -1;
        hyperplane_t *hy_res = 0;
        for(auto c : hyperplane_candidates){
            item_t *item_ptr = choose_item_set[c.first];
            if(item_ptr->upper_bound > highest_priority || item_ptr->upper_bound == 0){
                double priority = compute_hy_priority_update_upper_bound(item_ptr, points_in_region);
                if(priority > highest_priority){
                    highest_priority = priority;
                    highest_index = c.first;
                    hy_res = c.second;
                }
                if(priority == 0){ // The hyperplane does not intersect any region. Remove it.
                    cand_to_be_removed.push_back(c.first);
                }
            }
        }
        // MUST: remove the selected candidate
        if(highest_index >= 0) hyperplane_candidates.erase(highest_index);
        for(auto ind : cand_to_be_removed) hyperplane_candidates.erase(ind);
        return highest_index;
    }


    /** @brief get points whose partition intersects with a confidence region
     *  
    */
    void get_intersecting_points(conf_region &cr){
        cr.points.clear();
        for(auto it = cr.partitions.cbegin(); it != cr.partitions.cend(); it++){
            cr.points.insert((*it)->represent_point[0]);
        }
    }

    /** @brief    Randomly determine the next question
     */
    std::pair<point_t *, point_t *> rand_select_hyperplane(const vector<conf_region> &conf_regions, std::set<std::pair<point_t *, point_t *>> &selected_questions){
        point_t *p1 = 0, *p2 = 0; 
        std::set<point_t *> candidate_set;
        int k = conf_regions.size() - 1;
        for(int i=0; i <= k; i++){
            for(int j = 0; j <= i; j++){
                for(auto p: conf_regions[j].points){
                    candidate_set.insert(p);
                }
            }
            if(candidate_set.size() < 2) continue;
            std::vector<point_t *> candidate_points(candidate_set.begin(), candidate_set.end());
            // Obtain a random seed for the random number engine
            std::random_device rd;
            // Use the random seed to initialize the random number engine
            std::mt19937 g(rd());
            // shuffle the vector
            std::shuffle(candidate_points.begin(), candidate_points.end(), g);
            for(int j = 0; j < candidate_points.size() - 1; j++){
                for(int k = 1; k < candidate_points.size(); k++){
                    point_t *cand1 = candidate_points[j], *cand2 = candidate_points[k];
                    if(selected_questions.find(make_pair(cand1, cand2)) != selected_questions.end() ||
                        selected_questions.find(make_pair(cand2, cand1)) != selected_questions.end()) continue; // make sure this pair was not used before
                    p1 = cand1, p2 = cand2;
                    selected_questions.insert(make_pair(p1, p2));
                    return make_pair(p1, p2);
                }
            }
        }
        return make_pair(p1, p2);
    }


    /** @brief initialize the confidence region R^0 to contain all partitions
     *  
    */
    void initialize_conf_region(conf_region &cr, std::vector<halfspace_set_t *> &half_set_set){
        for(int i = 0; i < half_set_set.size(); i++){
            halfspace_set_t *half_set = alloc_halfspace_set(half_set_set[i]);
            point_t *correspond_point = half_set_set[i]->represent_point[0];
            if(correspond_point == NULL){ // check this point must not be empty
                cout << "ERROR: the represent point is empty" << endl;
                exit(1);
            }
            half_set->represent_point.push_back(correspond_point);
            cr.partitions.push_back(half_set);
        }
        get_intersecting_points(cr);
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

    void exact_recur(std::vector<conf_region> &conf_regions, halfspace_t *hs){
        //  1, for each partition in each conf region, decides its relation with hs
        //      if it lies inside hs, do nothing
        //      if it lies outside hs, move it to the outer conf region
        //      if it intersects with hs, split it
        //  2, update the intersecting points of each conf region
        int k = conf_regions.size() - 1;
        for(int i = k; i >= 0; i--){
            auto it = conf_regions[i].partitions.begin();
            while(it != conf_regions[i].partitions.end()){
                int pos = check_situation(hs, (*it));
                if(pos == -2){
                    if((*it)->ext_pts.size() == 0){ // avoid errors caused by precision issue
                        release_halfspace_set(*it);
                        it = conf_regions[i].partitions.erase(it);
                    }
                }
                else if(pos == 1){ // the partition lie completely in hs
                    ++it;
                }
                else if(pos == -1){ // the partition lies completely outside hs
                    if(i == k){
                        // LEAK: need to free the allocated memory 
                        release_halfspace_set(*it);
                        it = conf_regions[i].partitions.erase(it);
                    }
                    else{
                        // move the partition to the outer confidence region
                        conf_regions[i+1].partitions.push_back(*it);
                        it = conf_regions[i].partitions.erase(it);
                    }
                }
                else{ // the partition intersects with hs
                    halfspace_set_t * other_half = alloc_halfspace_set(*it);
                    halfspace_t * hs_minus = reverse_halfspace(hs);
                    
                    // first update the partition in this conf region
                    (*it)->halfspaces.push_back(alloc_halfspace(hs->point1, hs->point2, hs->offset, hs->direction));
                    get_extreme_pts_refine_halfspaces_alg1(*it);

                    // then move the pruned part of this partition to the outer conf region (but only when i < k)
                    if(i < k){
                        other_half->halfspaces.push_back(hs_minus);
                        get_extreme_pts_refine_halfspaces_alg1(other_half);
                        if(other_half->ext_pts.size() == 0){ // avoid errors caused by precision issue
                            release_halfspace_set(other_half);
                        }
                        else{
                            other_half->represent_point.push_back((*it)->represent_point[0]);
                            conf_regions[i+1].partitions.push_back(other_half);
                        }
                    }
                    else{
                        release_halfspace_set(other_half);
                        release_halfspace(hs_minus);
                    }
                    if((*it)->ext_pts.size() == 0){ // avoid errors caused by precision issue
                        release_halfspace_set(*it);
                        it = conf_regions[i].partitions.erase(it);
                    }
                    else{
                        ++it;
                    }
                }
            }
        }
        for(int i = 0; i <= k; i++){
            get_intersecting_points(conf_regions[i]);
        }

    }


    int Exact_revised(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta){
        start_timer();
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
        
        // initialize conf_Region R^0 ... R^k
        vector<conf_region> conf_regions;
        for(int i=0; i<=k; i++){
            conf_regions.push_back(conf_region());
        }
        initialize_conf_region(conf_regions[0], half_set_set); // conf_region[0] now contains all partitions

        // key: index of the item in choose_item_set; value: pointer to the hyperplane
        std::map<int, hyperplane_t *> hyperplane_candidates;
        construct_hy_candidates(hyperplane_candidates, choose_item_set);
        std::set<point_t *> points_return = compute_considered_set(conf_regions);
        std::set<std::pair<point_t *, point_t *>> selected_questions;
        int round = 0;
        while(points_return.size() > w){
            point *p1 =0, *p2 =0;
            if(select_opt == SCORE_SELECT){
                // int best_idx = find_best_hyperplane(choose_item_set, hyperplane_candidates, conf_regions);
                int best_idx = find_best_hyperplane_lazy_update(choose_item_set, hyperplane_candidates, conf_regions);
                if(best_idx < 0) break;
                p1 = choose_item_set[best_idx]->hyper->point1;
                p2 = choose_item_set[best_idx]->hyper->point2;
            }
            else{ // using random select
                std::pair<point_t *, point_t *> point_pair = rand_select_hyperplane(conf_regions, selected_questions);
                p1 = point_pair.first, p2 = point_pair.second;
                if(p1 == 0 || p2 == 0) break;
            }
            halfspace_t *hs = 0;
            if(dot_prod(u, p1) > dot_prod(u, p2)){ //p1 > p2
                if((double) rand()/RAND_MAX > theta) hs = alloc_halfspace(p2, p1, 0, true);
                else hs = alloc_halfspace(p1, p2, 0, true);
            }
            else{
                if((double) rand()/RAND_MAX > theta) hs = alloc_halfspace(p1, p2, 0, true);
                else hs = alloc_halfspace(p2, p1, 0, true);
            }
            exact_recur(conf_regions, hs);
            release_halfspace(hs);
            std::set<point_t *> considered_points = compute_considered_set(conf_regions);
            if(considered_points.size() == 0){ 
                break;
            }
            points_return = considered_points;
            round++;
        }
        
        stop_timer();
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

        // clear conf regions
        for(int i = 0; i <= k; i++){
            auto it = conf_regions[i].partitions.begin();
            while(it != conf_regions[i].partitions.end()){
                release_halfspace_set(*it);
                ++it;
            }
        } 
        bool success = check_correctness(points_return, u, best_score);
        if(success) ++correct_count;
        question_num += round;
        return_size += points_return.size();
        return 0;
    }
}
