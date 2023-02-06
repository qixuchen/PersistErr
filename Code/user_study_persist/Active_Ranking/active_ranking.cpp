#include "active_ranking.h"
#include <sys/time.h>

/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
int Active_Ranking(std::vector<point_t *> p_set, point_set_t *P, int w)
{
    int dim = p_set[0]->dim;
    int round = 0;
    point_random(p_set);

    start_timer();
    //initial
    //add the basic halfspace into R_hyperplane
    halfspace_set_t *R_half_set = R_initial(dim);
    int M = p_set.size();
    std::vector<point_t *> current_use;
    current_use.push_back(p_set[0]); //store all the points in order
    for (int i = 1; i < M; i++) //compare: p_set contains all the points
    {
        bool same_exist = false;
        for (int j = 0; j < current_use.size(); j++)
        {
            if (is_same_point(p_set[i], current_use[j]))//if there exist a same point in the current_use, insert p_set[i]
            {
                same_exist = true;
                current_use.insert(current_use.begin() + j, p_set[i]);
                break;
            }
        }
        if (!same_exist)
        {
            int num_point = current_use.size();
            int place = 0; //the place of the point inserted into the current_use
            //find the question asked user
            for (int j = 0; j < num_point; j++)
            {
                hyperplane_t *h = alloc_hyperplane(p_set[i], current_use[j], 0);
                int relation = check_situation_accelerate(h, R_half_set, 0);
                release_hyperplane(h);
                //if intersect, calculate the distance
                if (relation == 0)
                {
                    round++;
                    int option = show_to_user(P, p_set[i]->id, current_use[j]->id);
                    if (option == 1)
                    {
                        halfspace_t *half = alloc_halfspace(current_use[j], p_set[i], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
                    }
                    else
                    {
                        halfspace_t *half = alloc_halfspace(p_set[i], current_use[j], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        get_extreme_pts_refine_halfspaces_alg1(R_half_set);
                        place = j + 1;
                    }
                }
                else if (relation == -1)
                {
                    place = j + 1;
                }
            }
            current_use.insert(current_use.begin() + place, p_set[i]);
        }
    }
    stop_timer();
    p_set.clear();
    release_halfspace_set(R_half_set);
    
    //get the returned points
    int output_size = min(w, int(current_use.size()));
    vector<point_t *> points_return;
    for(int i=0; i < output_size; i++){
        points_return.push_back(current_use[i]);
    }
    print_result_list(P, points_return);
    question_num += round;
    return_size += output_size;
    return 0;
}
