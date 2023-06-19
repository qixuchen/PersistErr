#include "optimal_exact_question.h"

namespace opt_exact_question{

    /** @brief check if a preference is already in the known_preference set
     *  @return true if the preference is already in the set
    */
    bool preference_exist(std::set<std::pair<point_t *, point_t *>> &known_preferences, point_t *p1, point_t *p2){
        return known_preferences.find(std::make_pair(p1, p2)) != known_preferences.end() || known_preferences.find(std::make_pair(p2, p1)) != known_preferences.end();
    }

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


    /** @brief      Find s points using the score-based selection
     *              first find the first p_size hyperplanes with the highest priority
     *              then find s points from these hyperplanes
     *              If in stage 1, first add the two points for stage 1 
     */ 
    std::vector<point_t *> score_select_s_points_lazy_update(std::vector<item *> &choose_item_set, std::map<int, hyperplane_t *> &hyperplane_candidates, std::vector<conf_region> &conf_regions, 
                                                                std::set<std::pair<point_t *, point_t *>> &known_preferences, bool stage1, std::pair<point_t *, point_t *> *stg1_pair, int s){
        int k = conf_regions.size() - 1;
        std::vector<std::vector<point_t*>> points_in_region;
        std::vector<int> cand_to_be_removed;
        for(int i=0; i<=k; ++i){
            std::vector<point_t*> points(conf_regions[i].points.begin(), conf_regions[i].points.end());
            points_in_region.push_back(points);
        }
        int p_size = (s - 1) * (s - 2) / 2 + 1;
        std::vector<std::pair<int, double>> highest_priorities(p_size, std::make_pair(-1, 0));
        for(auto c : hyperplane_candidates){
            item_t *item_ptr = choose_item_set[c.first];
            point_t *p1 = item_ptr->hyper->point1, *p2 = item_ptr->hyper->point2;
            if(preference_exist(known_preferences, p1, p2)){ // the preference is already known
                cand_to_be_removed.push_back(c.first);
                continue;
            }
            double priority = compute_hy_priority_update_upper_bound(item_ptr, points_in_region);
            if(priority == 0){ // The hyperplane does not intersect any region. Remove it.
                cand_to_be_removed.push_back(c.first);
            }
            if(priority > highest_priorities[p_size - 1].second){
                highest_priorities[p_size - 1].second = priority;
                highest_priorities[p_size - 1].first = c.first;
                std::sort(highest_priorities.begin(), highest_priorities.end(), [](const std::pair<int, double> &a, const std::pair<int, double> &b){
                    return a.second > b.second;
                });
            }
        }
        std::set<point_t *> selected_points;
        if(stage1 && stg1_pair != NULL){
            selected_points.insert(stg1_pair->first);
            selected_points.insert(stg1_pair->second);
        }
        int i = 0;
        while(i < p_size && highest_priorities[i].first >= 0 && selected_points.size() < s){
            int ind = highest_priorities[i].first;
            hyperplane_t *hy = hyperplane_candidates[ind];
            point_t *p1 = hy->point1, *p2 = hy->point2;
            // MUST: make sure there is no "incomplete pair" in the selected points
            // If s = 4, and we have pair (p1, p2), (p1, p3), (p4, p5)
            // If the function returns p1 - p4 and the user chooses p4 in this case
            // nothing will be updated and there will be a dead loop
            std::set<point_t *> test = selected_points;
            test.insert(p1);
            test.insert(p2);
            if(test.size() <= s){
                selected_points.insert(p1);
                selected_points.insert(p2);
            }
            i++;
        }
        for(auto ind : cand_to_be_removed) hyperplane_candidates.erase(ind);
        std::vector<point_t *> res(selected_points.begin(), selected_points.end());
        return res;
    }


    /** @brief User's choose 1 out of s points if persistent error is made
     *         For each pair of points, first check if this pair is already compared before
     *         If yes, use the previous choice
     *         If no, choose one with error rate theta and record the choice
     *         return the final choice 
     */
    int persist_listwise_user_choice(std::vector<point_t *> point_cand, point_t *u, double theta, std::set<std::pair<point_t *, point_t *>> &underlying_preferences){
        std::vector<int> wins(point_cand.size(), 0);
        for(int i = 0; i < point_cand.size()-1; i++){
            for(int j = i+1; j < point_cand.size(); j++){
                if(underlying_preferences.find(make_pair(point_cand[i], point_cand[j])) != underlying_preferences.end()){
                    wins[i]++;
                }
                else if(underlying_preferences.find(make_pair(point_cand[j], point_cand[i])) != underlying_preferences.end()){
                    wins[j]++;
                }
                else{
                    double u1 = dot_prod(point_cand[i], u), u2 = dot_prod(point_cand[j], u);
                    if(u1 > u2){
                        if((double) rand()/RAND_MAX > theta){
                            wins[i]++;
                            underlying_preferences.insert(make_pair(point_cand[i], point_cand[j]));
                        }
                        else{
                            wins[j]++;
                            underlying_preferences.insert(make_pair(point_cand[j], point_cand[i]));
                        }
                    }
                    else{
                        if((double) rand()/RAND_MAX > theta){
                            wins[j]++;
                            underlying_preferences.insert(make_pair(point_cand[j], point_cand[i]));
                        }
                        else{
                            wins[i]++;
                            underlying_preferences.insert(make_pair(point_cand[i], point_cand[j]));
                        }
                    }
                }
            }
        }
        int cur_best = 0;
        for(int i = 1; i < wins.size(); i++){
            if(wins[i] > wins[cur_best]){
                cur_best = i;
            }
        }
        return cur_best;
    }


    /** @brief User's choose 1 out of s points if persistent error is made
     *         Randomly pick two points in each round, compare them, discard the loser
     *         Return the final choice
     */
    int persist_listwise_user_choice_v2(std::vector<point_t *> point_cand, point_t *u, double theta, std::set<std::pair<point_t *, point_t *>> &underlying_preferences){
        std::vector<point_t *> point_cand_cp = point_cand;
        while(point_cand.size() > 1){
            int i = (int) rand() % point_cand.size();
            int j = (int) rand() % point_cand.size();
            if(i == j) continue;
            if(underlying_preferences.find(make_pair(point_cand[i], point_cand[j])) != underlying_preferences.end()){
                point_cand.erase(point_cand.begin() + j);
            }
            else if(underlying_preferences.find(make_pair(point_cand[j], point_cand[i])) != underlying_preferences.end()){
                point_cand.erase(point_cand.begin() + i);
            }
            else{
                double u1 = dot_prod(point_cand[i], u), u2 = dot_prod(point_cand[j], u);
                if(u1 > u2){
                    if((double) rand()/RAND_MAX > theta){
                        underlying_preferences.insert(make_pair(point_cand[i], point_cand[j]));
                        point_cand.erase(point_cand.begin() + j);
                    }
                    else{
                        underlying_preferences.insert(make_pair(point_cand[j], point_cand[i]));
                        point_cand.erase(point_cand.begin() + i);
                    }
                }
                else{
                    if((double) rand()/RAND_MAX > theta){
                        underlying_preferences.insert(make_pair(point_cand[j], point_cand[i]));
                        point_cand.erase(point_cand.begin() + i);
                    }
                    else{
                        underlying_preferences.insert(make_pair(point_cand[i], point_cand[j]));
                        point_cand.erase(point_cand.begin() + j);
                    }
                }
            }
        }
        int final_choice = 0;
        for(int i = 0; i < point_cand_cp.size(); i++){
            if(point_cand_cp[i] == point_cand[0]){
                final_choice = i;
                break;
            }
        }
        return final_choice;
    }

    

    /** @brief User divide s points into superior and inferior groups
     *         First compared all pairs of points, rank them in decreasing order of wins
     *         randomly pick a number t between 1 and s-1
     *         Put the first t points into superior group, the rest into inferior group
     */
    void persist_superior_inferior_user_choice(std::vector<point_t *> point_cand, point_t *u, double theta, std::set<std::pair<point_t *, point_t *>> &underlying_preferences,
                                                std::vector<point_t *> &superior_group, std::vector<point_t *> &inferior_group){
        int split = (int) rand() % (point_cand.size() - 1) + 1; 
        std::vector<int> wins(point_cand.size(), 0);
        for(int i = 0; i < point_cand.size()-1; i++){
            for(int j = i+1; j < point_cand.size(); j++){
                if(underlying_preferences.find(make_pair(point_cand[i], point_cand[j])) != underlying_preferences.end()){
                    wins[i]++;
                }
                else if(underlying_preferences.find(make_pair(point_cand[j], point_cand[i])) != underlying_preferences.end()){
                    wins[j]++;
                }
                else{
                    double u1 = dot_prod(point_cand[i], u), u2 = dot_prod(point_cand[j], u);
                    if(u1 > u2){
                        if((double) rand()/RAND_MAX > theta){
                            wins[i]++;
                            underlying_preferences.insert(make_pair(point_cand[i], point_cand[j]));
                        }
                        else{
                            wins[j]++;
                            underlying_preferences.insert(make_pair(point_cand[j], point_cand[i]));
                        }
                    }
                    else{
                        if((double) rand()/RAND_MAX > theta){
                            wins[j]++;
                            underlying_preferences.insert(make_pair(point_cand[j], point_cand[i]));
                        }
                        else{
                            wins[i]++;
                            underlying_preferences.insert(make_pair(point_cand[i], point_cand[j]));
                        }
                    }
                }
            }
        }
        // Create a vector of pairs (index, value)
        std::vector<std::pair<int, int>> pairs;
        for (int i = 0; i < wins.size(); i++) {
            pairs.push_back(std::make_pair(i, wins[i]));
        }
        // Sort the vector of pairs by value in decreasing order
        std::sort(pairs.begin(), pairs.end(), [](const std::pair<int, int> &a, const std::pair<int, int> &b){
            return a.second > b.second;
        });;
        for(int i = 0; i < split; i++){
            superior_group.push_back(point_cand[pairs[i].first]);
        }
        for(int i = split; i < point_cand.size(); i++){
            inferior_group.push_back(point_cand[pairs[i].first]);
        }   
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


    /**
     *  @brief Compute the number of rounds expected for stage 1
    */
    int compute_stage1_num_question(int k, double theta){
        int num1 = k * ( k - 1 ) / 2;
        int num2 = ceil((1 + sqrt(theta)) * k / theta);
        return max(num1, num2);
    }


    bool only_Rk_nonempty(std::vector<conf_region> &conf_regions){
        for(int i = 0; i < conf_regions.size() - 1; i++){
            if(conf_regions[i].points.size() > 0) return false;
        }
        return true;
    }


    /**
     *  @brief Decide the pair of points to be displayed in stage 1
    */
    auto stage1_decide_point_pair(const std::vector<halfspace_set_t *> &half_set_set, std::vector<point_t *> &PS, int &focus, int &PS_pos){
        if(PS.size() == 0){ // base case
            PS_pos = 0;
            PS.push_back(half_set_set[0]->represent_point[0]);
            PS.push_back(half_set_set[1]->represent_point[0]);
            auto res = make_pair(PS[0], PS[1]);
            return res;
            
        }
        auto res = make_pair(half_set_set[focus]->represent_point[0], PS[PS_pos]);
        if(PS_pos == PS.size() - 1){
            PS.push_back(half_set_set[focus]->represent_point[0]);
            PS_pos = 0;
            ++focus;
        }
        else{
            ++PS_pos;
        } 
        return res;
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


    /**
     *  @brief Decide the pair of points to be displayed in stage 1
    */
    auto stage1_decide_point_pair(const std::vector<halfspace_set_t *> &half_set_set, std::set<std::pair<point_t *, point_t *>> &known_preferences, std::vector<point_t *> &PS, int &focus, int &PS_pos){
        if(PS.size() == 0){ // base case
            PS_pos = 0;
            point_t *p1 = half_set_set[0]->represent_point[0], *p2 = half_set_set[1]->represent_point[0];
            if(preference_exist(known_preferences, p1, p2)){
                PS.push_back(p1);
                PS.push_back(p2);
            }
            else{
                auto res = make_pair(p1, p2);
                return res;
            }
            
        }
        while(preference_exist(known_preferences, half_set_set[focus]->represent_point[0], PS[PS_pos])){
            if(PS_pos == PS.size() - 1){
                PS.push_back(half_set_set[focus]->represent_point[0]);
                PS_pos = 0;
                ++focus;
            }
            else{
                ++PS_pos;
            }     
        }
        auto res = make_pair(half_set_set[focus]->represent_point[0], PS[PS_pos]);
        return res;
    }


    int opt_exact_listwise(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta, int s){
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

        // initialize set PS and focusing point
        std::vector<point_t *> PS;
        int focus = 2, PS_pos = 0;// focus = 2 since the non-base case starts at the 3rd point
        int stage1_target=0; // stage1_target is the # of questions expected for stage 1
        stage1_target = compute_stage1_num_question(k, theta);

        std::map<int, hyperplane_t *> hyperplane_candidates;
        construct_hy_candidates(hyperplane_candidates, choose_item_set);
        std::set<point_t *> points_return = compute_considered_set(conf_regions);
        std::set<std::pair<point_t *, point_t *>> selected_questions;
        std::set<std::pair<point_t *, point_t *>> known_preferences, underlying_preferences;
        
        int round = 0;
        start_timer();
        while(points_return.size() > w){
            round++;
            point *p1 = 0, *p2 = 0;
            std::vector<point_t *> considered_points, point_cand;

            if(round < stage1_target && !only_Rk_nonempty(conf_regions)){ // run stage 1
                auto point_pair = stage1_decide_point_pair(half_set_set, known_preferences, PS, focus, PS_pos);
                // point_cand.push_back(point_pair.first);
                // point_cand.push_back(point_pair.second);
                point_cand = score_select_s_points_lazy_update(choose_item_set, hyperplane_candidates, conf_regions, known_preferences, true, &point_pair, s);
            }
            else{
                point_cand = score_select_s_points_lazy_update(choose_item_set, hyperplane_candidates, conf_regions, known_preferences, false, NULL, s);
            }
            if(point_cand.size() < 2){
                std::cout << "not enough points to select" << std::endl;
                break;
            }
            int user_choice = persist_listwise_user_choice(point_cand, u, theta, underlying_preferences);

            for(int i=0; i < point_cand.size(); i++){
                if(i == user_choice) continue;
                if(preference_exist(known_preferences, point_cand[user_choice], point_cand[i])) continue;
                known_preferences.insert(make_pair(point_cand[user_choice], point_cand[i]));
                halfspace_t *hs = alloc_halfspace(point_cand[i], point_cand[user_choice], 0, true);
                exact_recur(conf_regions, hs);
                release_halfspace(hs);
                points_return = compute_considered_set(conf_regions);
                if(points_return.size() <= w) break;
                if(conf_regions[k-1].points.size() == 0) break;
            }
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

        bool success = check_correctness(points_return, u, best_score);
        if(success) ++correct_count;
        question_num += round;
        return_size += points_return.size();
        cout << "return size: " << points_return.size() << endl;
        return 0;
    }


    int opt_exact_sup_inf(std::vector<point_t *> p_set, point_t *u, int k, int w, int select_opt, double theta, int s){
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

        // initialize set PS and focusing point
        std::vector<point_t *> PS;
        int focus = 2, PS_pos = 0;// focus = 2 since the non-base case starts at the 3rd point
        int stage1_target=0; // stage1_target is the # of questions expected for stage 1
        stage1_target = compute_stage1_num_question(k, theta);

        std::map<int, hyperplane_t *> hyperplane_candidates;
        construct_hy_candidates(hyperplane_candidates, choose_item_set);
        std::set<point_t *> points_return = compute_considered_set(conf_regions);
        std::set<std::pair<point_t *, point_t *>> selected_questions;
        std::set<std::pair<point_t *, point_t *>> known_preferences, underlying_preferences;

        int round = 0;
        start_timer();
        while(points_return.size() > w){
            round++;
            point *p1 = 0, *p2 = 0;
            std::vector<point_t *> considered_points, point_cand, sup_points, inf_points;

            if(round < stage1_target && !only_Rk_nonempty(conf_regions)){ // run stage 1
                auto point_pair = stage1_decide_point_pair(half_set_set, known_preferences, PS, focus, PS_pos);
                point_cand = score_select_s_points_lazy_update(choose_item_set, hyperplane_candidates, conf_regions, known_preferences, true, &point_pair, s);
            }
            else{
                point_cand = score_select_s_points_lazy_update(choose_item_set, hyperplane_candidates, conf_regions, known_preferences, false, NULL, s);
            }
            if(point_cand.size() < 2){
                std::cout << "not enough points to select" << std::endl;
                break;
            }
            persist_superior_inferior_user_choice(point_cand, u, theta, underlying_preferences, sup_points, inf_points);

            for(int i=0; i < sup_points.size(); i++){
                for(int j=0; j<inf_points.size(); j++){
                    std::pair<point_t *, point_t *> pref = make_pair(sup_points[i], inf_points[j]);
                    if(known_preferences.find(make_pair(sup_points[i], inf_points[j])) != known_preferences.end() || 
                        known_preferences.find(make_pair(inf_points[j], sup_points[i]))!= known_preferences.end() ) continue;
                    known_preferences.insert(pref);
                    halfspace_t *hs = alloc_halfspace(inf_points[j], sup_points[i], 0, true);
                    exact_recur(conf_regions, hs);
                    release_halfspace(hs);
                    points_return = compute_considered_set(conf_regions);
                    if(points_return.size() <= w) break;
                    if(conf_regions[k-1].points.size() == 0) break;
                }
                if(points_return.size() <= w) break;
                if(conf_regions[k-1].points.size() == 0) break;
            }
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

        bool success = check_correctness(points_return, u, best_score);
        if(success) ++correct_count;
        question_num += round;
        return_size += points_return.size();
        cout << "return size: " << points_return.size() << endl;
        return 0;
    }
}