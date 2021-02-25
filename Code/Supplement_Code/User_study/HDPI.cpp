#include "HDPI.h"
#include <sys/time.h>

/**
 * @brief Build all the partition(intersection of the halfspace), each partition corresponds to a top-1 point
 * @param p_set 				The set containing all the points which are possible to be the top-1 point
 *                              for some utility vectors
 * @param choose_item_points		The set containing points used to build choose_item
 * @param half_set_set			The returned partitions
 * @param considered_half_set	The set recorded all the partitions in the form of index
 */
void construct_halfspace_set(std::vector<point_t *> &p_set, halfspace_set_t *R, std::vector<point_t *> &choose_item_points,
                             std::vector<halfspace_set_t *> &half_set_set, std::vector<int> &considered_half_set)
{
    //initial
    choose_item_points.clear(); half_set_set.clear(); considered_half_set.clear();
    int M = p_set.size();
    if (M < 2)
        return;

    //record we use i-th point as the pivot(p[i]>p)
    for (int i = 0; i < M; i++)
    {
        halfspace_set_t *half_set = alloc_halfspace_set(R);
        get_extreme_pts_refine_from_halfspaces(half_set);
        for (int j = 0; j < M; j++)
        {
            if (!is_same_point(p_set[i], p_set[j]))
            {
                //halfspace in the form of <=
                halfspace_t *h = alloc_halfspace(p_set[j], p_set[i], 0, true);
                half_set->halfspaces.push_back(h);
            }
        }
        bool result = get_extreme_pts_refine_from_halfspaces(half_set);
        if (result)
        {
            half_set->represent_point.push_back(p_set[i]);
            half_set_set.push_back(half_set);
            choose_item_points.push_back(p_set[i]);
            considered_half_set.push_back(half_set_set.size() - 1);
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
                            std::vector<choose_item *> &choose_item_set)
{
    int M = p_set.size(); //The number of points used to construct hyperplane(questions)
    int H_M = half_set_set.size(); //The number of halfspace sets
    if (M < 2)
    {
        //printf("%s\n", "Error: The number of points is smaller than 2.");
        return -1;
    }
    if (H_M < 2)
    {
        //printf("%s\n", "Error: The number of partition is smaller than 1.");
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
            choose_item *c_item = (choose_item *) malloc(sizeof(choose_item));
            memset(c_item, 0, sizeof(choose_item));
            c_item->hyper = h;

            //classify the halfspace sets to the hyperplane
            for (int k = 0; k < H_M; k++)
            {
                int which_side = check_situation_accelerate(h, half_set_set[k], 0);
                //int which_side = check_situation_accelerate_LP(h, half_set_set[k], 0);
                if (which_side == 1)
                {
                    c_item->positive_side.push_back(k);
                }
                else if (which_side == -1)
                {
                    c_item->negative_side.push_back(k);
                }
                else if (which_side == 0)
                {
                    c_item->intersect_case.push_back(k);
                }
                else
                {
                    printf("Error: check side failed.\n");
                    return -1;
                }
            }
            choose_item_set.push_back(c_item);

            //calculate the even score
            double ES_i;
            int p_s = c_item->positive_side.size();
            int n_s = c_item->negative_side.size();
            int i_s = c_item->intersect_case.size();
            if (p_s > n_s)
            {
                ES_i = n_s - (Beta * i_s);
            }
            else
            {
                ES_i = p_s - (Beta * i_s);
            }
            //renew even score if necessary
            if (ES_h < ES_i)
            {
                ES_h = ES_i;
                num_hyperplane = choose_item_set.size() - 1;
            }
        }
    }
    return num_hyperplane;
}

/**
 * @brief Based on the answer from the user, modify the choose_item table
 * @param choose_item_set 		The choose_item table which will be modified
 * @param half_set_set 			All the partitions
 * @param considered_half_set 	Based on the user's answer, the partitions still considered
 * @param H_num					The choose_item used for asking question
 * @param direction 			The user's answer(which side)
 *                              -true on the positive side
 *                              -false on the negative side
 * @return The choose_item(hyperplane) which divide the half_set most evenly
 */
int modify_choose_item_table(std::vector<choose_item *> &choose_item_set,
                             std::vector<halfspace_set_t *> half_set_set, std::vector<int> &considered_half_set,
                             int H_num, bool direction)
{
    int M = choose_item_set.size();
    if (M < 1)
    {
        printf("%s\n", "Error: no item is in the choose_item_table.");
        return -1;
    }
    //refine the considered half_set
    if (direction == true)//delete all the negative side partitions
    {
        int consider_count = 0; //index for scanning the considered_half_set
        for (int i = 0; i < choose_item_set[H_num]->negative_side.size(); i++)
        {
            while (consider_count < considered_half_set.size() &&
                   considered_half_set[consider_count] <= choose_item_set[H_num]->negative_side[i])
            {

                if (considered_half_set[consider_count] == choose_item_set[H_num]->negative_side[i])
                {
                    considered_half_set.erase(considered_half_set.begin() + consider_count);
                }
                else
                {
                    consider_count++;
                }
            }
        }
    }
    else
    {
        int consider_count = 0;
        for (int i = 0; i < choose_item_set[H_num]->positive_side.size(); i++)
        {
            while (consider_count < considered_half_set.size() &&
                   considered_half_set[consider_count] <= choose_item_set[H_num]->positive_side[i])
            {
                if (considered_half_set[consider_count] == choose_item_set[H_num]->positive_side[i])
                {
                    considered_half_set.erase(considered_half_set.begin() + consider_count);
                }
                else
                {
                    consider_count++;
                }
            }
        }
    }

    //delete all the half_sets which are on the non-direction side
    if (direction)
    {
        //delete negative side
        for (int j = 0; j < M; j++)//scan all the choose_item, j-th choose_item
        {
            if (H_num != j)
            {
                //deal with one item, delete all the half_set on the negative side of the hyperplane
                int positive_count = 0, negative_count = 0, intersect_count = 0;
                for (int i = 0; i < choose_item_set[H_num]->negative_side.size(); i++)
                {
                    //deal with vector positive_side
                    while (positive_count < choose_item_set[j]->positive_side.size() &&
                           choose_item_set[j]->positive_side[positive_count] <=
                           choose_item_set[H_num]->negative_side[i])
                    {
                        if (choose_item_set[j]->positive_side[positive_count] ==
                            choose_item_set[H_num]->negative_side[i])
                        {
                            choose_item_set[j]->positive_side.erase(
                                    choose_item_set[j]->positive_side.begin() + positive_count);
                        }
                        else
                        {
                            positive_count++;
                        }
                    }

                    //deal with vector negative_side
                    while (negative_count < choose_item_set[j]->negative_side.size() &&
                           choose_item_set[j]->negative_side[negative_count] <=
                           choose_item_set[H_num]->negative_side[i])
                    {
                        if (choose_item_set[j]->negative_side[negative_count] ==
                            choose_item_set[H_num]->negative_side[i])
                        {
                            choose_item_set[j]->negative_side.erase(
                                    choose_item_set[j]->negative_side.begin() + negative_count);
                        }
                        else
                        {
                            negative_count++;
                        }
                    }

                    //deal with vector intersect_side
                    while (intersect_count < choose_item_set[j]->intersect_case.size() &&
                           choose_item_set[j]->intersect_case[intersect_count] <=
                           choose_item_set[H_num]->negative_side[i])
                    {
                        if (choose_item_set[j]->intersect_case[intersect_count] ==
                            choose_item_set[H_num]->negative_side[i])
                        {
                            choose_item_set[j]->intersect_case.erase(
                                    choose_item_set[j]->intersect_case.begin() + intersect_count);
                        }
                        else
                        {
                            intersect_count++;
                        }
                    }
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
                //deal with one item, delete all the half_set on the positive side of the hyperplane
                int positive_count = 0, negative_count = 0, intersect_count = 0;
                for (int i = 0; i < choose_item_set[H_num]->positive_side.size(); i++)
                {
                    //deal with vector positive_side
                    while (positive_count < choose_item_set[j]->positive_side.size() &&
                           choose_item_set[j]->positive_side[positive_count] <=
                           choose_item_set[H_num]->positive_side[i])
                    {
                        if (choose_item_set[j]->positive_side[positive_count] ==
                            choose_item_set[H_num]->positive_side[i])
                        {
                            choose_item_set[j]->positive_side.erase(
                                    choose_item_set[j]->positive_side.begin() + positive_count);
                        }
                        else
                        {
                            positive_count++;
                        }
                    }

                    //deal with vector negative_side
                    while (negative_count < choose_item_set[j]->negative_side.size() &&
                           choose_item_set[j]->negative_side[negative_count] <=
                           choose_item_set[H_num]->positive_side[i])
                    {
                        if (choose_item_set[j]->negative_side[negative_count] ==
                            choose_item_set[H_num]->positive_side[i])
                        {
                            choose_item_set[j]->negative_side.erase(
                                    choose_item_set[j]->negative_side.begin() + negative_count);
                        }
                        else
                        {
                            negative_count++;
                        }
                    }

                    //deal with vector intersect_side
                    while (intersect_count < choose_item_set[j]->intersect_case.size() &&
                           choose_item_set[j]->intersect_case[intersect_count] <=
                           choose_item_set[H_num]->positive_side[i])
                    {
                        if (choose_item_set[j]->intersect_case[intersect_count] ==
                            choose_item_set[H_num]->positive_side[i])
                        {
                            choose_item_set[j]->intersect_case.erase(
                                    choose_item_set[j]->intersect_case.begin() + intersect_count);
                        }
                        else
                        {
                            intersect_count++;
                        }
                    }
                }
            }
        }
    }

    //build a halfspace based on choose_item[H_num]
    halfspace_t *half;
    if (direction == true)
    {
        half = alloc_halfspace(choose_item_set[H_num]->hyper->point2, choose_item_set[H_num]->hyper->point1, 0, true);
    }
    else
    {
        half = alloc_halfspace(choose_item_set[H_num]->hyper->point1, choose_item_set[H_num]->hyper->point2, 0, true);
    }

    //refine all the half_set in the intersect.case
    for (int i = 0; i < choose_item_set[H_num]->intersect_case.size(); i++)
    {
        //num_set: index of the halfspace_set in half_set_set
        int num_set = choose_item_set[H_num]->intersect_case[i];
        half_set_set[num_set]->halfspaces.push_back(half);
        get_extreme_pts_refine_from_halfspaces(half_set_set[num_set]);
    }

    //deal with the refined half_set
    for (int i = 0; i < choose_item_set[H_num]->intersect_case.size(); i++)
    {
        int num_set = choose_item_set[H_num]->intersect_case[i];
        //scan all the choose_item, j-th choose_item
        for (int j = 0; j < M; j++)
        {
            if (H_num != j)
            {
                //deal with vector intersect_case
                int intersect_count = 0;
                while (intersect_count < choose_item_set[j]->intersect_case.size() &&
                       choose_item_set[j]->intersect_case[intersect_count] <= num_set)
                {
                    if (choose_item_set[j]->intersect_case[intersect_count] == num_set)
                    {
                        int which_side = check_situation_accelerate(choose_item_set[j]->hyper, half_set_set[num_set],
                                                                    0);
                        if (which_side == -2)
                        {
                            printf("%s\n", "Error: check side failed.");
                            return -1;
                        }
                        if (which_side != 0)
                        {
                            choose_item_set[j]->intersect_case.erase(
                                    choose_item_set[j]->intersect_case.begin() + intersect_count);
                            if (which_side == 1)//the half_set is on the positive side
                            {
                                bool is_insert = false;
                                for (int q = 0; q < choose_item_set[j]->positive_side.size(); q++)
                                {
                                    if (num_set < choose_item_set[j]->positive_side[q])
                                    {
                                        choose_item_set[j]->positive_side.insert(
                                                choose_item_set[j]->positive_side.begin() + q, num_set);
                                        is_insert = true;
                                        break;
                                    }
                                }
                                if (is_insert == false)
                                {
                                    choose_item_set[j]->positive_side.push_back(num_set);
                                }

                            }
                            else//the half_set is on the negative side
                            {
                                bool is_insert = false;
                                for (int q = 0; q < choose_item_set[j]->negative_side.size(); q++)
                                {
                                    if (num_set < choose_item_set[j]->negative_side[q])
                                    {
                                        choose_item_set[j]->negative_side.insert(
                                                choose_item_set[j]->negative_side.begin() + q, num_set);
                                        is_insert = true;
                                        break;
                                    }
                                }
                                if (is_insert == false)
                                {
                                    choose_item_set[j]->negative_side.push_back(num_set);
                                }
                            }

                        }
                        break;
                    }
                    intersect_count++;
                }
            }
        }
    }

    //refine the choose_item_set and select the hyperplane asked user
    choose_item_set.erase(choose_item_set.begin() + H_num);

    int item_count = 0;                         //the index for scanning the choose_item_set
    int ES_h = -1000;                           //Even score
    int num_hyperplane = 0;                     //index of the chosen hyperplane(question)
    int choose_size = choose_item_set.size();   //the original size of the choose_item_set
    for (int i = 0; i < choose_size; i++)
    {
        choose_item_t *c_item = choose_item_set[item_count];
        int M_positive = choose_item_set[item_count]->positive_side.size();
        int M_negative = choose_item_set[item_count]->negative_side.size();
        int M_intersect = choose_item_set[item_count]->intersect_case.size();
        if (M_positive + M_negative == 0 || M_positive + M_intersect == 0 || M_negative + M_intersect == 0)
        {
            choose_item_set.erase(choose_item_set.begin() + item_count);
        }
        else
        {
            if (M_positive < M_negative)
            {
                double ES_i = M_positive - (Beta * M_intersect);
                if (ES_h < ES_i)
                {
                    ES_h = ES_i;
                    num_hyperplane = item_count;
                }
            }
            else
            {
                double ES_i = M_negative - (Beta * M_intersect);
                if (ES_h < ES_i)
                {
                    ES_h = ES_i;
                    num_hyperplane = item_count;
                }
            }
            item_count++;
        }
    }
    return num_hyperplane;
}


/**
 * @brief Find some of the top-k points by interacting with users
 * @param wPtr          The file recording all interacting information
 * @param p_set         The selected 1000 cars
 * @param original_P    The orignial dataset
 * @param k             parameter k
 * @param interval      Used for the number of returned point
 * @param u             The utility vector (for test only)
 * @param numQ          The number of questions asked
 * @param resPoint      The output set
 */
void HDPI_sampling(FILE *wPtr, std::vector<point_t *> p_set, point_set_t *original_P, int k, int interval, point_t* u, double *numQ, vector<point_t*> *resPoint)
{
    //p_top1 contains the points which are the convex points
    std::vector<point_t *> p_top1, p_refined, p_result;
    int dim = p_set[0]->dim, s = 1;
    point_t *uk = alloc_point(dim);
    find_top1_sampling(p_set, p_top1, uk, 10, 0, 0); //sampling method
    release_point(uk);
    for(int i = 0; i < p_set.size(); ++i)
        p_refined.push_back(p_set[i]);

    /* half_set_set          contains all the partitions(intersection of halfspaces)
     * considered_half_set   contains all the possible partitions considered
     * choose_item_points    contains all the points used to construce hyperplanes(questions)
     * choose_item_set       contains all the hyperplanes(questions) which can be asked user
     * R_half_set            The utility range R
     */
    halfspace_set_t *R_half_set = R_initial(dim);
    std::vector<halfspace_set_t *> half_set_set;
    std::vector<int> considered_half_set;
    std::vector<point_t *> choose_item_points;
    std::vector<choose_item *> choose_item_set;
    construct_halfspace_set(p_top1, R_half_set, choose_item_points, half_set_set, considered_half_set);

    int index = build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
    int opt;

    /*
    double v1 = dot_prod(u, choose_item_set[index]->hyper->point1), v2 = dot_prod(u, choose_item_set[index]->hyper->point2);
    if(v1 >= v2)
        opt = 1;
    else
        opt = 2;
    */
    opt = show_to_user(original_P, choose_item_set[index]->hyper->point1->id, choose_item_set[index]->hyper->point2->id);

    halfspace_t *hy; int numOfQuestion = 0;
    while (p_result.size() < k)
    {
        numOfQuestion++;
        if (opt == 1)
        {
            hy = alloc_halfspace(choose_item_set[index]->hyper->point2, choose_item_set[index]->hyper->point1, 0, true);
            index = modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, true);
        }
        else
        {
            hy = alloc_halfspace(choose_item_set[index]->hyper->point1, choose_item_set[index]->hyper->point2, 0, true);
            index = modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, false);
        }

        //Find whether there exist point which is the topk point w.r.t any u in R
        R_half_set->halfspaces.push_back(hy);
        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
        std::vector<point_t *> top_current;
        if(find_possible_topk(p_set, R_half_set, k, top_current))
        {
            if (check_possible_s_topk(p_set, R_half_set, k, s, top_current))
            {
                if(s == 1 || s == 5 || s == 10 ||s == 15 || s == 20)
                    print_interaction_result(wPtr, "HD-PI-sampling", k, s, numOfQuestion, top_current, original_P);
                numQ[s - 1] = numOfQuestion;
                resPoint[s - 1] = top_current;

                s += interval;
                if(s > k)
                    return;
            }
        }
        double *len = new double[dim], *position = new double[dim];
        calculate_length(R_half_set, len, position);
        while(considered_half_set.size() < 2 && p_result.size() < k)
        {
            point_t *point_sure;
            if(half_set_set.size() < 1)
                point_sure = p_top1[0];
            else
                point_sure = half_set_set[considered_half_set[0]]->represent_point[0];
            p_result.push_back(point_sure);
            if(p_result.size() == s)
            {
                if(s == 1 || s == 5 || s == 10 ||s == 15 || s == 20)
                    print_interaction_result(wPtr, "HD-PI-sampling", k, s, numOfQuestion, p_result, original_P);
                numQ[s - 1] = numOfQuestion;
                resPoint[s - 1] = p_result;

                s += interval;
                if(s > k)
                    return;
            }
            //find all new possible top-1 points
            for(int i = 0; i < p_refined.size(); ++i)
            {
                if(is_same_point(p_refined[i], point_sure))
                {
                    p_refined.erase(p_refined.begin() + i);
                    break;
                }
            }
            p_top1.clear();

            uk = alloc_point(dim);
            find_top1_sampling(p_refined, p_top1, uk, 10, 0, 0, position, len);//use sampling method
            //find_top1_sampling(p_refined, p_top1, uk, 0, 0);
            release_point(uk);

            construct_halfspace_set(p_top1, R_half_set, choose_item_points, half_set_set, considered_half_set);
            index = build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
        }
        if(p_result.size() < k)
        {
            /*
            v1 = dot_prod(u, choose_item_set[index]->hyper->point1);
            v2 = dot_prod(u, choose_item_set[index]->hyper->point2);
            if(v1 >= v2)
                opt = 1;
            else
                opt = 2;
            */
            opt = show_to_user(original_P, choose_item_set[index]->hyper->point1->id, choose_item_set[index]->hyper->point2->id);
        }
    }
}

/**
 * @brief Find some of the top-k points by interacting with users
 * @param wPtr          The file recording all interacting information
 * @param p_set         The selected 1000 cars
 * @param original_P    The orignial dataset
 * @param k             parameter k
 * @param interval      Used for the number of returned point
 * @param u             The utility vector (for test only)
 * @param numQ          The number of questions asked
 * @param resPoint      The output set
 */
void HDPI_accurate(FILE *wPtr, std::vector<point_t *> p_set, point_set_t *original_P, int k, int interval, point_t* u, double *numQ, vector<point_t*> *resPoint)
{
    //p_top1 contains the points which are the convex points
    std::vector<point_t *> p_convex, p_top1, p_refined, p_result;
    int dim = p_set[0]->dim, s = 1;
    find_top1(p_set, p_convex);
    skyline_c(p_convex, p_top1);
    for(int i = 0; i < p_set.size(); ++i)
        p_refined.push_back(p_set[i]);

    /* half_set_set          contains all the partitions(intersection of halfspaces)
     * considered_half_set   contains all the possible partitions considered
     * choose_item_points    contains all the points used to construce hyperplanes(questions)
     * choose_item_set       contains all the hyperplanes(questions) which can be asked user
     * R_half_set            The utility range R
     */
    halfspace_set_t *R_half_set = R_initial(dim);
    std::vector<halfspace_set_t *> half_set_set;
    std::vector<int> considered_half_set;
    std::vector<point_t *> choose_item_points;
    std::vector<choose_item *> choose_item_set;
    construct_halfspace_set(p_top1, R_half_set, choose_item_points, half_set_set, considered_half_set);

    int index = build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
    int opt;

    /*
    double v1 = dot_prod(u, choose_item_set[index]->hyper->point1), v2 = dot_prod(u, choose_item_set[index]->hyper->point2);
    if(v1 >= v2)
        opt = 1;
    else
        opt = 2;
    */
    opt = show_to_user(original_P, choose_item_set[index]->hyper->point1->id, choose_item_set[index]->hyper->point2->id);

    halfspace_t *hy; int numOfQuestion = 0;
    while (p_result.size() < k)
    {
        numOfQuestion++;
        if (opt == 1)
        {
            hy = alloc_halfspace(choose_item_set[index]->hyper->point2, choose_item_set[index]->hyper->point1, 0, true);
            index = modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, true);
        }
        else
        {
            hy = alloc_halfspace(choose_item_set[index]->hyper->point1, choose_item_set[index]->hyper->point2, 0, true);
            index = modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, false);
        }

        //Find whether there exist point which is the topk point w.r.t any u in R
        R_half_set->halfspaces.push_back(hy);
        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
        std::vector<point_t *> top_current;
        if(find_possible_topk(p_set, R_half_set, k, top_current))
        {
            if (check_possible_s_topk(p_set, R_half_set, k, s, top_current))
            {
                if(s == 1 || s == 5 || s == 10 ||s == 15 || s == 20)
                    print_interaction_result(wPtr, "HD-PI-accurate", k, s, numOfQuestion, top_current, original_P);
                numQ[s - 1] = numOfQuestion;
                resPoint[s - 1] = top_current;

                s += interval;
                if(s > k)
                    return;

            }
        }
        double *len = new double[dim], *position = new double[dim];
        calculate_length(R_half_set, len, position);
        while(considered_half_set.size() < 2 && p_result.size() < k)
        {
            point_t *point_sure = half_set_set[considered_half_set[0]]->represent_point[0];
            p_result.push_back(point_sure);
            if(p_result.size() == s)
            {
                if(s == 1 || s == 5 || s == 10 ||s == 15 || s == 20)
                    print_interaction_result(wPtr, "HD-PI-accurate", k, s, numOfQuestion, p_result, original_P);
                numQ[s - 1] = numOfQuestion;
                resPoint[s - 1] = p_result;

                s += interval;
                if(s > k)
                    return;
            }

            //find all new possible top-1 points
            for(int i = 0; i < p_refined.size(); ++i)
            {
                if(is_same_point(p_refined[i], point_sure))
                {
                    p_refined.erase(p_refined.begin() + i);
                    break;
                }
            }
            p_convex.clear(); p_top1.clear();
            find_top1(p_refined, p_convex);
            skyline_c(p_convex, p_top1);
            construct_halfspace_set(p_top1, R_half_set, choose_item_points, half_set_set, considered_half_set);
            index = build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
        }
        if(p_result.size() < k)
        {
            /*
            v1 = dot_prod(u, choose_item_set[index]->hyper->point1);
            v2 = dot_prod(u, choose_item_set[index]->hyper->point2);
            if(v1 >= v2)
                opt = 1;
            else
                opt = 2;
            */
            opt = show_to_user(original_P, choose_item_set[index]->hyper->point1->id, choose_item_set[index]->hyper->point2->id);
        }
    }
}