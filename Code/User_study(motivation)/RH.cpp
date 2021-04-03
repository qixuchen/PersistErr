#include "RH.h"
#include <sys/time.h>

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
void RH(FILE *wPtr, std::vector<point_t*> &p_set, point_set_t* original_P, int k, int interval, point_t* u, double *numQ, vector<point_t*> *rePoint)
{
    point_random(p_set);
    int dim = p_set[0]->dim, M = p_set.size(), numOfQuestion = 0, s = 1;

    //initial: add the basic halfspace into R_hyperplane
    halfspace_set_t* R_half_set = R_initial(dim);
    std::vector<point_t*> top_current, current_point, current_use;
    point_t* p_result = NULL;
    current_use.push_back(p_set[0]);
    //compare: p_set contains all the points
    for(int i = 1; i < M; i++)
    {
        if(!is_same_exist(p_set[i], current_use))
        {
            for(int j = 0; j < current_use.size(); ++j)
                current_point.push_back(current_use[j]);
            current_use.push_back(p_set[i]);
            while(current_point.size() > 0)
            {
                int num_point = current_point.size(), scan_index = 0; //the index where the points we have scanned
                int p_index = -1; // the index where the point we select to ask question
                bool need_ask = false;
                double distance = 999999;
                //find the question asked user
                for(int j = 0; j < num_point; j++)
                {
                    hyperplane_t* h = alloc_hyperplane(p_set[i], current_point[scan_index], 0);
                    int relation = check_situation_accelerate(h, R_half_set, 0);
                    //if intersect, calculate the distance
                    if(relation == 0)
                    {
                        need_ask = true;
                        double d_h;
                        d_h = calculate_distance(h, R_half_set->in_center);
                        if(d_h < distance)
                        {
                            distance = d_h;
                            p_index = scan_index;
                        }
                        scan_index++;
                    }
                    else
                        current_point.erase(current_point.begin() + scan_index);
                }
                if(need_ask)
                {
                    numOfQuestion++;
                    int opt;
                    opt = show_to_user(original_P, p_set[i]->id, current_point[p_index]->id);
                    /*
                    double val1 = dot_prod(p_set[i], u), val2 = dot_prod(current_point[p_index], u);
                    if(val1 >= val2)
                        opt = 1;
                    else
                        opt = 2;
                    */
                    if(opt == 1)
                    {
                        halfspace_t* half = alloc_halfspace(current_point[p_index], p_set[i], 0, true);
                        R_half_set->halfspaces.push_back(half);
                    }
                    else
                    {
                        halfspace_t* half = alloc_halfspace(p_set[i], current_point[p_index], 0, true);
                        R_half_set->halfspaces.push_back(half);
                    }
                    current_point.erase(current_point.begin() + p_index);
                    get_extreme_pts_refine_halfspaces_alg1(R_half_set);

                    //check if we can find top-k point in R_half_set
                    top_current.clear();
                    if(find_possible_topk(p_set, R_half_set, k, top_current))
                    {
                        while(check_possible_s_topk(p_set, R_half_set, k, s, top_current))
                        {
                            if(s == 1 || s == 5 || s == 10 ||s == 15 || s == 20)
                                print_interaction_result(wPtr, "RH", k, s, numOfQuestion, top_current, original_P);
                            numQ[s - 1] = numOfQuestion;
                            rePoint[s - 1] = top_current;

                            s += interval;
                            if(s > k)
                                return;

                            top_current.clear();
                            find_possible_topk(p_set, R_half_set, k, top_current);
                        }

                    }
                }
            }
        }
    }
}
