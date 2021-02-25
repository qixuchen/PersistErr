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
update_ext_vec(point_set_t *P, point_set_t* original_set, vector<int> &C_idx, int s, vector<point_t *> &ext_vec, int &current_best_idx,
               int &last_best, vector<int> &frame, int cmp_option)
{
    // generate s cars for selection in a round
    vector<int> S = generate_S(P, C_idx, s, current_best_idx, last_best, frame, cmp_option);
    if (S.size() != 2) // we fix s to be 2 in the demo
    {
        printf("invalid option numbers\n");
        exit(0);
    }

    //printf("comp: %d %d\n", skyline_proc_P->points[C_idx[S[0]]]->id, skyline_proc_P->points[C_idx[S[1]]]->id);

    // get the better car among two from the user
    int max_i = show_to_user(original_set, P->points[C_idx[S[0]]]->id, P->points[C_idx[S[1]]]->id) - 1;

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
point_t* max_utility(FILE *wPtr, point_set_t *P0, point_set_t* original_set, int cmp_option, int &questions)
{
    int s = 2, stop_option = EXACT_BOUND, prune_option = RTREE, dom_option = HYPER_PLANE;
    point_set_t *P = skyline_point(P0);
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

    // the initial extreme vector sets V
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
    // interactively reduce the candidate set and shrink the candidate utility range
    while (C_idx.size() > 1 && Qcount<=100)
    {
        Qcount++;
        sort(C_idx.begin(), C_idx.end()); // prevent select two different points after different skyline algorithms
        // generate the options for user selection and update the extreme vectors based on the user feedback
        update_ext_vec(P, original_set, C_idx, s, ext_vec, current_best_idx, last_best, frame, cmp_option);
        if (C_idx.size() == 1)
        { // || global_best_idx == current_best_idx
            break;
        }
        //update candidate set
        rtree_pruning(P, C_idx, ext_vec, rr, stop_option, dom_option);
    }

    // get the final result
    int result_id = P->points[get_current_best_pt(P, C_idx, ext_vec)]->id;

    for (int i = 0; i < ext_vec.size(); i++)
        release_point(ext_vec[i]);

    printf("\n--------------------------------------------------------\n");
    if (cmp_option == RANDOM)
    {
        printf("%s %10d \n", "No. of questions asked:", Qcount);
    }
    else
    {
        printf("%s %10d \n", "No. of questions asked:", Qcount);
    }
    printf("--------------------------------------------------------\n");
    printf("Recommended cars:\n");
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", original_set->points[result_id]->coord[0], original_set->points[result_id]->coord[1],
           original_set->points[result_id]->coord[2], original_set->points[result_id]->coord[3]);
    printf("--------------------------------------------------------\n");
    printf("Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
           "how bored you feel when you are asked with %d questions for this round \n"
           "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
           "that you feel the most bored and 1 denotes that you feel the least bored.)\n"
           "You answer: ", Qcount);
    int sat = 0;
    while(sat < 1 || sat > 10)
    {
        scanf("%d", &sat);
    }
    printf("\n\n");


    fprintf(wPtr, "--------------------------------------------------------\n");
    if (cmp_option == RANDOM)
    {
        fprintf(wPtr, "|%30s |%10d |\n", "UH-RANDOM", Qcount);
    }
    else
    {
        fprintf(wPtr, "|%30s |%10d |\n", "UH-SIMPLEX", Qcount);
    }
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "Recommended cars:\n");
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", original_set->points[result_id]->coord[0], original_set->points[result_id]->coord[1],
           original_set->points[result_id]->coord[2], original_set->points[result_id]->coord[3]);
    fprintf(wPtr, "---------------------------------------------------------\n");
    fprintf(wPtr, "Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                  "how bored you feel when you are asked with %d questions for this round \n"
                  "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                  "that you feel the most bored and 1 denotes that you feel the least bored.) %d\n\n\n", Qcount, sat);

    questions = Qcount;
    return original_set->points[result_id];
}