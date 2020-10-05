#include "maxUtility.h"
#include <sys/time.h>

/**
 * @brief Get the index of the "current best" point.
 *        Find the average utility vector u of the extreme points of range R. Find the point with the maximum utility w.r.t u
 * @param P         the input car set
 * @param C_idx     the indexes of the current candidate favorite car in P
 * @param ext_vec   the set of extreme vector
 * @return the index of the point in C_idx
 */
int get_current_best_pt(point_set_t *P, vector<int> &C_idx, vector<point_t *> &ext_vec)
{
    int dim = P->points[0]->dim;

    // the set of extreme points of the candidate utility range R
    vector<point_t *> ext_pts;
    ext_pts = get_extreme_pts(ext_vec);

    // use the "mean" utility vector in R (other strategies could also be used)
    point_t *mean = alloc_point(dim);
    for (int i = 0; i < dim; i++)
    {
        mean->coord[i] = 0;
    }
    for (int i = 0; i < ext_pts.size(); i++)
    {
        for (int j = 0; j < dim; j++)
            mean->coord[j] += ext_pts[i]->coord[j];
    }
    for (int i = 0; i < dim; i++)
    {
        mean->coord[i] /= ext_pts.size();
    }

    // look for the maximum utility point w.r.t. the "mean" utility vector
    int best_pt_idx;
    double max = 0;
    for (int i = 0; i < C_idx.size(); i++)
    {
        point_t *pt = P->points[C_idx[i]];

        double v = dot_prod(pt, mean);
        if (v > max)
        {
            max = v;
            best_pt_idx = C_idx[i];
        }
    }

    for (int i = 0; i < ext_pts.size(); i++)
        release_point(ext_pts[i]);
    return best_pt_idx;
}

/**
 * @brief Generate s cars for selection in a round
 * @param P                     the input car set
 * @param C_idx                 the indexes of the current candidate favorite car in P
 * @param s                     the number of cars for user selection
 * @param current_best_idx      the current best car
 * @param last_best             the best car in previous interaction
 * @param frame                 the frame for obtaining the set of neigbouring vertices of the current best vertex
 *                              (used only if cmp_option = SIMPLEX)
 * @param cmp_option            the car selection mode, which must be either SIMPLEX or RANDOM
 * @return The set of indexes of the points chosen for asking questions
 */
vector<int> generate_S(point_set_t *P, vector<int> &C_idx, int s, int current_best_idx, int &last_best,
                       vector<int> &frame, int cmp_option)
{
    // the set of s cars for selection
    vector<int> S;
    if (cmp_option == RANDOM) // RANDOM car selection mode
    {
        // randoming select at most s non-overlaping cars in the candidate set
        while (S.size() < s && S.size() < C_idx.size())
        {
            int idx = rand() % C_idx.size();

            bool isNew = true;
            for (int i = 0; i < S.size(); i++)
            {
                if (S[i] == idx)
                {
                    isNew = false;
                    break;
                }
            }
            if (isNew)
            {
                S.push_back(idx);
            }
        }
    }
    else if (cmp_option == SIMPLEX) // SIMPLEX car selection mode
    {
        if (last_best != current_best_idx ||
            frame.size() == 0) // the new frame is not computed before (avoid duplicate frame computation)
        {
            // create one ray for each car in P for computing the frame
            vector<point_t *> rays;
            int best_i = -1;
            for (int i = 0; i < P->numberOfPoints; i++)
            {
                if (i == current_best_idx)
                {
                    best_i = i;
                    continue;
                }

                point_t *best = P->points[current_best_idx];
                point_t *newRay = sub(P->points[i], best);
                rays.push_back(newRay);
            }

            // frame compuatation
            frameConeFastLP(rays, frame);
            // update the indexes lying after current_best_idx
            for (int i = 0; i < frame.size(); i++)
            {
                if (frame[i] >= current_best_idx)
                {
                    frame[i]++;
                }
                //S[i] = C_idx[S[i]];
            }
            for (int i = 0; i < rays.size(); i++)
                release_point(rays[i]);
        }
        for (int j = 0; j < C_idx.size(); j++)
        {
            if (C_idx[j] ==
                current_best_idx)// it is possible that current_best_idx is no longer in the candidate set, then no need to compare again
            {
                S.push_back(j);
                break;
            }
        }
        // select at most s non-overlaping cars in the candidate set based on "neighboring vertices" obtained via frame compuation
        for (int i = 0; i < frame.size() && S.size() < s; i++)
        {
            for (int j = 0; j < C_idx.size() && S.size() < s; j++)
            {
                if (C_idx[j] == current_best_idx)
                {
                    continue;
                }

                if (C_idx[j] == frame[i])
                {
                    S.push_back(j);
                }
            }
        }

        // if less than s car are selected, fill in the remaining one
        if (S.size() < s && C_idx.size() > s)
        {
            for (int j = 0; j < C_idx.size(); j++)
            {
                bool noIn = true;
                for (int i = 0; i < S.size(); i++)
                {
                    if (j == S[i])
                    {
                        noIn = false;
                    }
                }
                if (noIn)
                {
                    S.push_back(j);
                }

                if (S.size() == s)
                {
                    break;
                }
            }
        }
    }
    else // for testing only. Do not use this!
    {
        vector<point_t *> rays;

        int best_i = -1;
        for (int i = 0; i < C_idx.size(); i++)
        {
            if (C_idx[i] == current_best_idx)
            {
                best_i = i;
                continue;
            }

            point_t *best = P->points[current_best_idx];

            point_t *newRay = sub(P->points[C_idx[i]], best);

            rays.push_back(newRay);
        }

        partialConeFastLP(rays, S, s - 1);
        if (S.size() > s - 1)
        {
            S.resize(s - 1);
        }
        for (int i = 0; i < S.size(); i++)
        {
            if (S[i] >= best_i)
            {
                S[i]++;
            }

            //S[i] = C_idx[S[i]];
        }
        S.push_back(best_i);


        for (int i = 0; i < rays.size(); i++)
            release_point(rays[i]);
    }
    return S;
}

/**
 * @brief Generate the options for user selection. Update the extreme vectors based on the user feedback
 *        Prune points which are impossible to be top-1
 * @param P                     The skyline dataset
 * @param C_idx                 the indexes of the current candidate favorite car in P
 * @param u                     the utility vector
 * @param s                     the number of cars for user selection
 * @param ext_vec               the set of extreme vecotr
 * @param current_best_idx      the current best car
 * @param last_best             the best car in previous interaction
 * @param frame                 the frame for obtaining the set of neigbouring vertices of the current best vertex
 *                              (used only if cmp_option = SIMPLEX)
 * @param cmp_option            the car selection mode, which must be either SIMPLEX or RANDOM
 */
void
update_ext_vec(point_set_t *P, vector<int> &C_idx, point_t *u, int s, vector<point_t *> &ext_vec, int &current_best_idx,
               int &last_best, vector<int> &frame, int cmp_option)
{
    // generate s cars for selection in a round
    vector<int> S = generate_S(P, C_idx, s, current_best_idx, last_best, frame, cmp_option);

    int max_i = -1;
    double max = -1;
    for (int i = 0; i < S.size(); i++)
    {
        point_t *p = P->points[C_idx[S[i]]];
        double v = dot_prod(u, p);
        if (v > max)
        {
            max = v;
            max_i = i;
        }
    }
    //printf("\n");

    // get the better car among those from the user
    last_best = current_best_idx;
    current_best_idx = C_idx[S[max_i]];

    // for each non-favorite car, create a new extreme vector
    for (int i = 0; i < S.size(); i++)
    {
        if (max_i == i)
        {
            continue;
        }
        point_t *tmp = sub(P->points[C_idx[S[i]]], P->points[C_idx[S[max_i]]]);
        C_idx[S[i]] = -1;
        point_t *new_ext_vec = scale(1 / calc_len(tmp), tmp);
        release_point(tmp);
        ext_vec.push_back(new_ext_vec);
    }

    // directly remove the non-favorite car from the candidate set
    vector<int> newC_idx;
    for (int i = 0; i < C_idx.size(); i++)
    {
        if (C_idx[i] >= 0)
        {
            newC_idx.push_back(C_idx[i]);
        }
    }
    C_idx = newC_idx;
}

/**
 * @brief The interactive algorithm Random/Simplex. Find best points/return a point which satisfy the regret ratio
 * @param original_P     the original dataset
 * @param u              the unkonwn utility vector
 * @param s              the question size
 * @param epsilon        the required regret ratio
 * @param maxRound       the maximum number of rounds of interactions
 * @param cmp_option     the car selection mode
 *                       -SIMPLEX UH-Simplex method
 *                       -RANDOM UH-Simplex method
 * @param stop_option    the stopping condition
 *                       -NO_BOUND
 *                       -EXACT_BOUND
 *                       -APRROX_BOUND
 * @param prune_option   the skyline algorithm
 *                       -SQL
 *                       -RTREE
 * @param dom_option     the domination checking mode
 *                       -HYPER_PLANE
 *                       -CONICAL_HULL
 */
int max_utility(point_set_t *P, point_t *u, int s, double epsilon, int maxRound,
                int cmp_option, int stop_option, int prune_option, int dom_option)
{
    //point_set_t *P = skyline_point(P0);
    int dim = P->points[0]->dim;

    // Qcount - the number of questions asked
    // Csize - the size of the current candidate set
    int Qcount = 0;
    double rr = 1;

    // the indexes of the candidate set
    // record skyline points, used for selecting hyperplanes(questions)
    vector<int> C_idx;
    for (int i = 0; i < P->numberOfPoints; i++)
        C_idx.push_back(i);

    // the initial extreme vector sets V = {−ei | i ∈ [1, d], ei [i] = 1 and ei [j] = 0 if i , j}.
    vector<point_t *> ext_vec, ext_pts;
    for (int i = 0; i < dim; i++)
    {
        point_t *e = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            if (i == j)
            {
                e->coord[j] = -1;
            }
            else
            {
                e->coord[j] = 0;
            }
        }
        ext_vec.push_back(e);
    }

    int current_best_idx = -1;
    int last_best = -1;
    vector<int> frame;

    // get the index of the "current best" point
    current_best_idx = get_current_best_pt(P, C_idx, ext_vec);

    // if not skyline
    //sql_pruning(P, C_idx, ext_vec);
    // interactively reduce the candidate set and shrink the candidate utility range
    while (C_idx.size() > 1 && (rr > epsilon && !isZero(rr - epsilon)) && Qcount < maxRound)
        // while none of the stopping condition is true
    {
        Qcount++;
        //printf("Number of Question %d\n", Qcount);
        sort(C_idx.begin(), C_idx.end()); // prevent select two different points after different skyline algorithms
        // generate the options for user selection and update the extreme vectors based on the user feedback
        update_ext_vec(P, C_idx, u, s, ext_vec, current_best_idx, last_best, frame, cmp_option);
        if (C_idx.size() == 1)
        { // || global_best_idx == current_best_idx
            break;
        }
        //update candidate set
        if (prune_option == SQL)
        {
            sql_pruning(P, C_idx, ext_vec, rr, stop_option, dom_option);
        }
        else
        {
            rtree_pruning(P, C_idx, ext_vec, rr, stop_option, dom_option);
        }
    }

    // get the final result
    point_t *result = P->points[get_current_best_pt(P, C_idx, ext_vec)];

    for (int i = 0; i < ext_vec.size(); i++)
        release_point(ext_vec[i]);

   if (cmp_option == RANDOM)
    {
        printf("|%30s |%10d |%10s |\n", "UH-RANDOM", Qcount, "--");
    }
    else
    {
        printf("|%30s |%10d |%10s |\n", "UH-SIMPLEX", Qcount, "--");
    }
    printf("|%30s |%10s |%10d |\n", "Point", "--", result->id);
    printf("---------------------------------------------------------\n");
    return Qcount;
}

/**
 * @brief The interactive algorithm UH-Random-Adapt.
 *        Find a points/return a point which satisfy the regret ratio
 *        Modified stopping condition, prune points which are not able to be top-k
 * @param original_P     the original dataset
 * @param u              the unkonwn utility vector
 * @param s              the question size
 * @param epsilon        the required regret ratio
 * @param maxRound       the maximum number of rounds of interactions
 * @param cmp_option     the car selection mode
 *                       -SIMPLEX UH-Simplex method
 *                       -RANDOM UH-Simplex method
 * @param stop_option    the stopping condition
 *                       -NO_BOUND
 *                       -EXACT_BOUND
 *                       -APRROX_BOUND
 * @param prune_option   the skyline algorithm
 *                       -SQL
 *                       -RTREE
 * @param dom_option     the domination checking mode
 *                       -HYPER_PLANE
 *                       -CONICAL_HULL
 * @param k              The threshold top-k
 */
int Random_Adapt(std::vector<point_t *> p_set, point_t *u, double epsilon, int k)
{
    int dim = p_set[0]->dim, Qcount = 0; //the number of questions asked
    double rr = 1; //regret ratio
    halfspace_set_t *half = R_initial(dim);
    bool orderd = false;

    // interactively reduce the candidate set and shrink the candidate utility range
    while (p_set.size() > k && (rr > epsilon && !isZero(rr - epsilon)) && (!orderd))
    {
        Qcount++;
        bool choose = false;
        int idx1, idx2, M = p_set.size(), considered_count = 0;
        bool **is_considered = new bool *[M];
        for (int i = 0; i < M; i++)
            is_considered[i] = new bool[M];
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < M; j++)
            {
                is_considered[i][j] = false;
            }
        }
        while (!choose)
        {
            idx1 = rand() % M;
            idx2 = rand() % M;
            if (!is_considered[idx1][idx2] && !is_considered[idx2][idx1])
            {
                is_considered[idx1][idx2] = true;
                is_considered[idx2][idx1] = true;
                considered_count++;
                if(!is_same_point(p_set[idx1], p_set[idx2]))
                {
                    hyperplane_t *h = alloc_hyperplane(p_set[idx1], p_set[idx2], 0);
                    if (check_situation(h, half->ext_pts) == 0)
                    {
                        choose = true;
                    }
                    release_hyperplane(h);
                }
                if (considered_count >= (M * (M + 1)) / 2 && (!choose))
                {
                    orderd = true;
                    break;
                }
            }
        }
        for(int i=0; i< M;i++)
            delete []is_considered[i];
        delete []is_considered;
        if (orderd)
        {
            break;
        }
        point_t *p = p_set[idx1];
        double v1 = dot_prod(u, p);
        p = p_set[idx2];
        double v2 = dot_prod(u, p);
        if (v1 > v2)
        {
            halfspace_t *h = alloc_halfspace(p_set[idx2], p_set[idx1], 0, true);
            half->halfspaces.push_back(h);
        }
        else
        {
            halfspace_t *h = alloc_halfspace(p_set[idx1], p_set[idx2], 0, true);
            half->halfspaces.push_back(h);
        }
        get_extreme_pts_refine_halfspaces_alg1(half);
        get_rrbound_exact(half->ext_pts);
        //update candidate set
        find_possible_top_k(p_set, half->ext_pts, k);
    }

    release_halfspace_set(half);
    printf("|%30s |%10d |%10s |\n", "UH-RANDOM-Adapt", Qcount, "--");
    int i = rand()%p_set.size();
    printf("|%30s |%10s |%10d |\n", "Point", "--", p_set[i]->id);
    printf("---------------------------------------------------------\n");
    return Qcount;
}

/**
 * @brief The interactive algorithm UH-Simplex-Adapt.
 *        Find a points/return a point which satisfy the regret ratio
 *        Modified stopping condition, prune points which are not able to be top-k
 * @param original_P     the original dataset
 * @param u              the unkonwn utility vector
 * @param s              the question size
 * @param epsilon        the required regret ratio
 * @param maxRound       the maximum number of rounds of interactions
 * @param cmp_option     the car selection mode
 *                       -SIMPLEX UH-Simplex method
 *                       -RANDOM UH-Simplex method
 * @param stop_option    the stopping condition
 *                       -NO_BOUND
 *                       -EXACT_BOUND
 *                       -APRROX_BOUND
 * @param prune_option   the skyline algorithm
 *                       -SQL
 *                       -RTREE
 * @param dom_option     the domination checking mode
 *                       -HYPER_PLANE
 *                       -CONICAL_HULL
 * @param k              The threshold top-k
 */
int Simplex_Adapt(std::vector<point_t *> p_set, point_t *u, double epsilon, int k)
{
    int dim = p_set[0]->dim, Qcount = 0, ccount; //the number of questions asked
    double rr = 1; //regret ratio
    bool orderd = false, last_exist = false;
    std::vector<point_t *> near_set;
    std::vector<int> considered_id;
    halfspace_set_t *half = R_initial(dim);
    int current_id, current_position;
    // interactively reduce the candidate set and shrink the candidate utility range
    while (p_set.size() > k && (rr > epsilon && !isZero(rr - epsilon)) && (!orderd))
    {
        Qcount++;
        bool choose = false;
        int idx1, idx2, M = p_set.size(), considered_count = 0;
        considered_id.clear();
        //considered_id.shrink_to_fit();
        bool **is_considered = new bool *[M];
        for (int i = 0; i < M; i++)
            is_considered[i] = new bool[M];
        for (int i = 0; i < M; i++)
        {
            for (int j = 0; j < M; j++)
            {
                is_considered[i][j] = false;
            }
        }
        if (!last_exist)
        {
            while (!choose)
            {
                idx1 = rand() % p_set.size();
                idx2 = rand() % p_set.size();
                if (!is_considered[idx1][idx2] && !is_considered[idx2][idx1])
                {
                    is_considered[idx1][idx2] = true;
                    is_considered[idx2][idx1] = true;
                    considered_count++;
                    if (!is_same_point(p_set[idx1], p_set[idx2]))
                    {
                        hyperplane_t *h = alloc_hyperplane(p_set[idx1], p_set[idx2], 0);
                        if (check_situation_accelerate(h, half, 0) == 0)
                        {
                            choose = true;
                        }
                        release_hyperplane(h);
                    }
                }
                if (considered_count >= (M * (M + 1)) / 2 && (!choose))
                {
                    orderd = true;
                    break;
                }
            }
        }
        else
        {
            idx1 = current_position, idx2 = 0;
            considered_id.push_back(p_set[idx1]->id); //record the point id, we have used for pixel
            ccount = 0;
            near_set.clear();
            //near_set.shrink_to_fit(); //record the nearest point of p_set[idx1]
            for (int q = 0; q < p_set.size(); q++)
            {
                if (q != idx1)
                {
                    p_set[q]->value = 0;
                    for (int j = 0; j < dim; j++)
                    {
                        p_set[q]->value += (p_set[q]->coord[j] - p_set[idx1]->coord[j]) *
                                           (p_set[q]->coord[j] - p_set[idx1]->coord[j]);
                    }
                    p_set[q]->value = sqrt(p_set[q]->value);
                    p_set[q]->pid = q;
                    near_set.push_back(p_set[q]);
                }
            }
            sort(near_set.begin(), near_set.end(), valueCmp());
            while (!choose)
            {
                idx2 = (near_set[idx2]->pid) % p_set.size();
                ccount++;
                if (!is_considered[idx1][idx2] && !is_considered[idx2][idx1])
                {
                    is_considered[idx1][idx2] = true;
                    is_considered[idx2][idx1] = true;
                    considered_count++;
                    if (!is_same_point(p_set[idx1], p_set[idx2]))
                    {
                        hyperplane_t *h = alloc_hyperplane(p_set[idx1], p_set[idx2], 0);
                        if (check_situation_accelerate(h, half, 0) == 0)
                        {
                            choose = true;
                        }
                        release_hyperplane(h);
                    }
                }
                if (considered_count >= (M * (M + 1)) / 2 && (!choose))
                {
                    orderd = true;
                    break;
                }
                if (ccount >= near_set.size() && !choose)
                {
                    int cd = -1;
                    bool not_use = true;
                    while (not_use && cd + 1 < near_set.size())
                    {
                        cd++;
                        not_use = false;
                        for (int q = 0; q < considered_id.size(); q++)
                        {
                            if (considered_id[q] == near_set[cd]->id)
                            {
                                not_use = true;
                                break;
                            }
                        }
                    }
                    if(cd + 1 >= near_set.size())
                    {
                        orderd = true;
                        break;
                    }
                    for (int q = 0; q < p_set.size(); q++)
                    {
                        if (p_set[q]->id == near_set[cd]->id)
                        {
                            idx1 = q;
                            considered_id.push_back(p_set[idx1]->id);
                            break;
                        }
                    }
                    near_set.clear();
                    //near_set.shrink_to_fit();
                    for (int q = 0; q < p_set.size(); q++)
                    {
                        if (q != idx1)
                        {
                            p_set[q]->value = 0;
                            for (int j = 0; j < dim; j++)
                            {
                                p_set[q]->value += (p_set[q]->coord[j] - p_set[idx1]->coord[j]) *
                                                   (p_set[q]->coord[j] - p_set[idx1]->coord[j]);
                            }
                            p_set[q]->value = sqrt(p_set[q]->value);
                            near_set.push_back(p_set[q]);
                        }
                    }
                    sort(near_set.begin(), near_set.end(), valueCmp());
                    ccount = 0;
                    idx2 = 0;
                }
            }
        }
        for(int i=0; i< M;i++)
            delete []is_considered[i];
        delete []is_considered;
        if (orderd)
        {
            break;
        }
        point_t *p = p_set[idx1];
        double v1 = dot_prod(u, p);
        p = p_set[idx2];
        double v2 = dot_prod(u, p);
        if (v1 > v2)
        {
            current_id = p_set[idx1]->id;
            halfspace_t *h = alloc_halfspace(p_set[idx2], p_set[idx1], 0, true);
            half->halfspaces.push_back(h);
        }
        else
        {
            current_id = p_set[idx2]->id;
            halfspace_t *h = alloc_halfspace(p_set[idx1], p_set[idx2], 0, true);
            half->halfspaces.push_back(h);
        }
        get_extreme_pts_refine_halfspaces_alg1(half);
        get_rrbound_exact(half->ext_pts);

        //update candidate set
        find_possible_top_k(p_set, half->ext_pts, k);
        //find whether the better point exist
        last_exist = false;
        for (int i = 0; i < p_set.size(); i++)
        {
            if (p_set[i]->id == current_id)
            {
                current_position = i;
                last_exist = true;
                break;
            }
        }
    }

    release_halfspace_set(half);
    printf("|%30s |%10d |%10s |\n", "UH-SIMPLEX-Adapt", Qcount, "--");
    int i = rand()%p_set.size();
    printf("|%30s |%10s |%10d |\n", "Point", "--", p_set[i]->id);
    printf("---------------------------------------------------------\n");
    return Qcount;
}