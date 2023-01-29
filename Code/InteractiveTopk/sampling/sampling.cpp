#include "sampling.h"


namespace sampling{


    /** @brief load samples to a point_t vector
    */
    void load_sample(std::vector<point_t *> &samples, std::vector<std::vector<double>> &randPoints, const std::vector<double> shift_point){
        int size = randPoints.size();
        if(size == 0){
            cout << __func__ << "ERROR: empty randPoint set" << endl;
        }
        int dim = randPoints[0].size();
        for(int i = 0; i < size; i++){
            point_t *s = alloc_point(dim); //LEAK: need to free samples later
            for(int j = 0; j < dim; j++){
                s->coord[j] = randPoints[i][j] + shift_point[j];
            }
            samples.push_back(s);
        }
    }


    /** @brief compute a point in the half set as the starting point for sampling
    */
    point_t *compute_sample_start(const halfspace_set_t* hs){
        int size = hs->ext_pts.size();
        if(size == 0){
            cout << "ERROR: Invalid input halfspace set" << endl;
            return NULL;
        }
        int dim = hs->ext_pts[0]->dim;
        point_t *start_pt = alloc_point(dim); //LEAK: need to free this point
        for (int i = 0; i < size; i++){
            for (int j = 0; j < dim; j++){
                start_pt->coord[j] += hs->ext_pts[i]->coord[j];
            }
        }
        for (int i = 0; i < dim; i++){
            start_pt->coord[i] /= size;
        }
        return start_pt;
    }


    /** @brief check if the sampled points are indeed inside the half set
     *  @return     1: if the sample is valid
     *              0: if invalid
    */
    int check_sample_validity(const halfspace_set_t* hs, const std::vector<point_t *> samples){
        int valid = 0, invalid = 0;
        int sample_size = samples.size();
        int hs_size = hs->halfspaces.size();
        for(int i = 0; i < sample_size; i++){
            int rel = check_point_halfset_relation(samples[i], hs);
            if(rel == 1) ++valid;
            else ++invalid;
        }
        return (valid == sample_size);
    }


    /**
     * @brief                   Sample a number of points from R
     * 
     * @param R                 The input polytope 
     * @param num_point         Number of points to be sampled
     * @param randPoints        The vector to store sampled points
     * @param shift_point       The point used to transform hyperplanes later
     */
    void polytope_sampling(halfspace_set_t* partition, int num_point, std::vector<std::vector<double>> &randPoints, std::vector<double> &shift_point){
        typedef double NT;
        typedef Cartesian<NT>    Kernel;
        typedef typename Kernel::Point    Point;
        typedef boost::mt19937    RNGType;
        typedef HPolytope<Point> Hpolytope;
        typedef BoostRandomNumberGenerator<boost::mt11213b, NT> RNG;

        int M = partition->halfspaces.size();
        if (M < 1){
            printf("%s\n", "Error: No halfspace in the set.");
            return;
        }
        int dim = partition->halfspaces[0]->normal->dim;
        vector<vector<double>> HP_vec;
        vector<double> d_v;
        d_v.push_back(M+1);
        d_v.push_back(dim+1);
        HP_vec.push_back(d_v);
        //Initialize Hpolytope
        //IMPORTANT: need to multiply -1
        for(int i=0;i<M;i++){
            HP_vec.push_back(std::vector<double>());
            HP_vec[i+1].push_back(0);
            for (int j = 0; j < dim; j++){
                HP_vec[i+1].push_back(-1*partition->halfspaces[i]->normal->coord[j]);
            }
        }
        HP_vec.push_back(std::vector<double>());
        HP_vec[M+1].push_back(1);
        for (int i = 0; i < dim; i++){
            HP_vec[M+1].push_back(-1);
        }

        Hpolytope HP;
        HP.init(HP_vec);
        //Compute chebychev ball
        std::pair<Point,NT> CheBall;
        CheBall = HP.ComputeInnerBall();

        // Setup the parameters
        int n = HP.dimension();
        int walk_len=2;
        int n_threads=1;
        NT e=1, err=0.1;
        NT C=2.0, ratio, frac=0.1, delta=-1.0;
        int N = 500 * ((int) C) + ((int) (n * n / 2));
        int W = 6*n*n+800;
        ratio = 1.0-1.0/(NT(n));

        unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
        RNGType rng(seed);
        boost::normal_distribution<> rdist(0,1);
        boost::random::uniform_real_distribution<>(urdist);
        boost::random::uniform_real_distribution<> urdist1(-1,1);

        RNG gen(n);
        auto InnerBall = HP.ComputeInnerBall();
        Point c = InnerBall.first;
        HP.normalize();
        HP.shift(c.getCoefficients());
        Point p(n);
        vector<Point> p_vec;
        uniform_sampling<BilliardWalk>(p_vec, HP, gen, walk_len, num_point, p, 10);
        //load points into randPoints
        randPoints.clear();
        for(int i=0; i<p_vec.size(); i++){
            randPoints.push_back(vector<double>());
            for (int j = 0; j < dim; j++)
            {
                randPoints[i].push_back(p_vec[i][j]);
            }
        }
        //load points into shift_point
        for (int j = 0; j < dim; j++){
            shift_point.push_back(c[j]);
        }
    }


    /**
     * @brief   Build the lookup table
     */
    void build_lookup_table(std::map<point_t *, point_t *> &lookup, const std::vector<halfspace_set_t *> &half_set_set, int num_samples){
        std::vector<std::vector<double>> rand_points;
        std::vector<double> shift_point;
        int size = half_set_set.size();
        for(int i = 0; i < size; i++){
            std::vector<point_t *> samples;
            rand_points.clear();
            shift_point.clear();
            polytope_sampling(half_set_set[i], num_samples, rand_points, shift_point);
            load_sample(samples, rand_points, shift_point); // LEAK: need to clear the samples in the lookup table
            int val = check_sample_validity(half_set_set[i], samples);
            for(int j = 0; j < samples.size(); j++){
                lookup.insert(make_pair(samples[j], half_set_set[i]->represent_point[0]));
            }
        }
    }

    /**
     * @brief   Update the set of data points whose partition intersecting with the confidence region
     *          using the samples
     */
    void update_points_in_region(sample_set &set, const std::map<point_t *, point_t *> &lookup){
        set.data.clear();
        for(auto it = set.sample.begin(); it != set.sample.end(); it++){
            auto match = lookup.find(*it);
            if(match == lookup.end()){
                cout << __func__ << "ERROR: the sample point does not match any point in the lookup table" << endl;
            }
            set.data.insert((*match).second);
        }
    }

    /**
     * @brief   Initialize sample sets
     */
    void initialize_sample_sets(std::vector<sample_set> &s_sets, const std::map<point_t *, point_t *> &lookup){
        for(auto it = lookup.begin(); it != lookup.end(); it++){
            s_sets[0].sample.insert((*it).first);
        }
        update_points_in_region(s_sets[0], lookup);
    }

    /**
     * @brief   Check if the best point is returned in considered_points
     */
    bool check_correctness(const std::vector<point_t *> &considered_points, point_t *u, double best_score){
        for(auto p : considered_points){
            if(dot_prod(u, p) == best_score) return true;
        }
        return false;
    }


    /** @brief      Find the points still not pruned
    */
    std::vector<point_t *> compute_considered_set(const std::vector<sample_set> &s_sets){
        int k = s_sets.size() - 1;
        std::set<point_t *> considered_points;
        for(int i=0; i <= k; i++){
            considered_points.insert(s_sets[i].data.begin(), s_sets[i].data.end());
        }
        return std::vector<point_t *> (considered_points.begin(), considered_points.end());
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
                                std::map<int, hyperplane_t *> &hyperplane_candidates, std::vector<sample_set> &s_sets){
        int k = s_sets.size()-1;
        std::vector<std::vector<point_t*>> points_in_region;
        std::vector<int> cand_to_be_removed;
        for(int i=0; i<=k; ++i){
            std::vector<point_t*> points(s_sets[i].data.begin(), s_sets[i].data.end());
            points_in_region.push_back(points);
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


    void sampling_recurrence(std::vector<sample_set> &s_sets, halfspace_t* hs, const std::map<point_t *, point_t *> &lookup){
        int k = s_sets.size() - 1;
        for(int i = k; i >= 0; i--){
            std::vector<point_t *> carry;
            for(auto it = s_sets[i].sample.begin(); it != s_sets[i].sample.end(); it++){
                if(dot_prod(*it, hs->normal) > 0){ // this sample is not in hs since it is outward pointing
                    carry.push_back(*it);
                }
            }
            // put the samples in carry to the outer region
            if(i < k){
                for(int j = 0; j < carry.size(); j++){
                    s_sets[i+1].sample.insert(carry[j]);
                }
            }
            // remove the samples in carry from this region
            for(int j = 0; j < carry.size(); j++){
                auto it = s_sets[i].sample.find(carry[j]);
                if(it != s_sets[i].sample.end()) s_sets[i].sample.erase(it);
            }
        }
        for(int i = 0; i <= k; i++){
            update_points_in_region(s_sets[i], lookup);
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


    int sampling(std::vector<point_t *> p_set, point_t *u, int k, int w, double theta){
        int num_sample = 250;
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
        std::vector<std::vector<double>> rand_points;
        std::vector<double> shift_point;
        std::map<point_t *, point_t *> lookup;

        construct_halfspace_set(convh, choose_item_points, half_set_set, considered_half_set);
        build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
        build_lookup_table(lookup, half_set_set, num_sample);

        std::vector<sample_set> s_sets;
        for(int i=0; i <= k; i++){
            s_sets.push_back(sample_set());
        }
        initialize_sample_sets(s_sets, lookup);

        std::map<int, hyperplane_t *> hyperplane_candidates;
        construct_hy_candidates(hyperplane_candidates, choose_item_set);
        std::vector<point_t *> points_return = compute_considered_set(s_sets);
        //double score = max_score(points_return, u);
        int round = 0;
        start_timer();
        while(points_return.size() > w){
            int best_idx = find_best_hyperplane(choose_item_set, hyperplane_candidates, s_sets);
            if(best_idx < 0) break;
            point_t* p1 = choose_item_set[best_idx]->hyper->point1;
            point_t* p2 = choose_item_set[best_idx]->hyper->point2;
            halfspace_t *hs = 0;
            if(dot_prod(u, p1) > dot_prod(u, p2)){ //p1 > p2
                if((double) rand()/RAND_MAX > theta) hs = alloc_halfspace(p2, p1, 0, true);
                else hs = alloc_halfspace(p1, p2, 0, true);
            }
            else{
                if((double) rand()/RAND_MAX > theta) hs = alloc_halfspace(p1, p2, 0, true);
                else hs = alloc_halfspace(p2, p1, 0, true);
            }
            sampling_recurrence(s_sets, hs, lookup);
            release_halfspace(hs);

            std::vector<point_t *> considered_points = compute_considered_set(s_sets);
            if(considered_points.size() == 0) {
                break;
            }
            points_return = considered_points;
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
        // clear sample points
        std::vector<point_t *> sample_free;
        for(int i = 0; i <= k; i++){
            for(auto it = s_sets[i].sample.begin(); it != s_sets[i].sample.end(); it++){
                sample_free.push_back(*it);
            }
        }
        for(auto p : sample_free) release_point(p);

        stop_timer();
        bool success = check_correctness(points_return, u, best_score);
        if(success) ++correct_count;
        question_num += round;
        return_size += points_return.size();
        return 0;
    }
}