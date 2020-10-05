#include "RH.h"
#include <sys/time.h>

/**
 * @brief Ask user question and find one of the top-k point
 *        The input set is refined by skyband
 *        For find the point which is the top-k point w.r.t any u in R, we search the whole set
 * @param p_set 		The selected 1000 points set
 * @param original_P    The set containing all the points
 * @param k 				The threshold top-k
 */
point_t* RH(FILE *wPtr, std::vector<point_t*> p_set, point_set_t* original_P, int k, int &questions)
{
    point_random(p_set);
    int dim = p_set[0]->dim, M = p_set.size(), numOfQuestion = 0;

    //initial: add the basic halfspace into R_hyperplane
    halfspace_set_t* R_half_set = R_initial(dim);
    std::vector<point_t*> top_current;
    std::vector<point_t*> current_point, current_use;
    point_t* p_result = NULL;
    current_use.push_back(p_set[0]);
    //compare: p_set contains all the points
    for(int i=1; i < M; i++)
    {
        bool same_exist=false;
        for(int j=0; j < current_use.size(); j++)
        {
            if(is_same_point(p_set[i], current_use[j]))
            {
                same_exist=true;
                break;
            }
        }
        if(!same_exist)
        {
            for(int j=0; j < current_use.size(); j++)
            {
                current_point.push_back(current_use[j]);
            }
            current_use.push_back(p_set[i]);
            while(current_point.size()>0)
            {
                int num_point = current_point.size();
                int scan_index = 0;//the index where the points we have scanned
                bool need_ask = false;
                double distance = 999999;
                int p_index = -1;// the index where the point we select to ask question
                //find the question asked user
                for(int j=0; j < num_point; j++)
                {
                    hyperplane_t* h = alloc_hyperplane(p_set[i], current_point[scan_index], 0);
                    int relation = check_situation_accelerate(h, R_half_set, 0);
                    //if intersect, calculate the distance
                    if(relation==0)
                    {
                        need_ask=true;
                        double d_h;
                        d_h=calculate_distance(h, R_half_set->in_center);
                        if(d_h < distance)
                        {
                            distance=d_h;
                            p_index=scan_index;
                        }
                        scan_index++;
                    }
                    else
                    {
                        current_point.erase(current_point.begin() + scan_index);
                    }
                }
                if(need_ask)
                {
                    numOfQuestion++;
                    int opt = show_to_user(original_P, p_set[i]->id, current_point[p_index]->id);
                    if(opt == 1)
                    {
                        halfspace_t* half=alloc_halfspace(current_point[p_index], p_set[i], 0, true);
                        R_half_set->halfspaces.push_back(half);
                    }
                    else
                    {
                        halfspace_t* half=alloc_halfspace(p_set[i], current_point[p_index], 0, true);
                        R_half_set->halfspaces.push_back(half);
                    }
                    current_point.erase(current_point.begin() + p_index);
                    get_extreme_pts_refine_halfspaces_alg1(R_half_set);

                    //check if we can find top-k point in R_half_set
                    top_current.clear();
                    //top_current.shrink_to_fit();
                    bool check_result;
                    check_result = find_possible_topk(p_set, R_half_set, k, top_current);
                    if(check_result)
                    {
                        p_result = check_possible_topk(p_set, R_half_set, k, top_current);
                        //printf("question %d  ext_num %d top_size %d\n", numOfQuestion, R_half_set->ext_pts.size(), top_current.size());
                    }
                    if(p_result != NULL)
                    {
                        printf("\n--------------------------------------------------------\n");
                        printf("%s %10d \n", "No. of questions asked:", numOfQuestion);
                        printf("--------------------------------------------------------\n");
                        printf("Recommended cars:\n");
                        printf("--------------------------------------------------------\n");
                        printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
                        printf("---------------------------------------------------------\n");
                        printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", original_P->points[p_result->id]->coord[0], original_P->points[p_result->id]->coord[1],
                                   original_P->points[p_result->id]->coord[2], original_P->points[p_result->id]->coord[3]);
                        printf("---------------------------------------------------------\n");
                        printf("Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                               "how bored you feel when you are asked with %d questions for this round \n"
                               "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                               "that you feel the most bored and 1 denotes that you feel the least bored.)\n"
                               "You answer: ", numOfQuestion);
                        int sat = 0;
                        while(sat < 1 || sat > 10)
                        {
                            scanf("%d", &sat);
                        }
                        printf("\n\n");


                        fprintf(wPtr, "--------------------------------------------------------\n");
                        fprintf(wPtr, "|%30s |%10d |\n", "RH", numOfQuestion);
                        fprintf(wPtr, "--------------------------------------------------------\n");
                        fprintf(wPtr, "Recommended cars:\n");
                        fprintf(wPtr, "--------------------------------------------------------\n");
                        fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
                        fprintf(wPtr, "---------------------------------------------------------\n");
                        fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", original_P->points[p_result->id]->coord[0], original_P->points[p_result->id]->coord[1],
                                    original_P->points[p_result->id]->coord[2], original_P->points[p_result->id]->coord[3]);
                        fprintf(wPtr, "---------------------------------------------------------\n");
                        fprintf(wPtr, "Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                                      "how bored you feel when you are asked with %d questions for this round \n"
                                      "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                                      "that you feel the most bored and 1 denotes that you feel the least bored.) %d\n\n\n", numOfQuestion, sat);

                        questions = numOfQuestion;
                        return original_P->points[p_result->id];
                    }
                }
            }
        }
    }
    printf("\n--------------------------------------------------------\n");
    printf("%s %10d \n", "No. of questions asked:", numOfQuestion);
    printf("--------------------------------------------------------\n");
    printf("Recommended cars:\n");
    printf("---------------------------------------------------------\n");
    printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", original_P->points[p_result->id]->coord[0], original_P->points[p_result->id]->coord[1],
               original_P->points[p_result->id]->coord[2], original_P->points[p_result->id]->coord[3]);
    printf("---------------------------------------------------------\n");
    printf("Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
           "how bored you feel when you are asked with %d questions for this round \n"
           "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
           "that you feel the most bored and 1 denotes that you feel the least bored.)\n"
           "You answer: ", numOfQuestion);
    int sat = 0;
    while(sat < 1 || sat > 10)
    {
        scanf("%d", &sat);
    }
    printf("\n\n");

    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%30s |%10d |\n", "RH", numOfQuestion);
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "Recommended cars:\n");
    fprintf(wPtr, "---------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", original_P->points[p_result->id]->coord[0], original_P->points[p_result->id]->coord[1],
                original_P->points[p_result->id]->coord[2], original_P->points[p_result->id]->coord[3]);
    fprintf(wPtr, "---------------------------------------------------------\n");
    fprintf(wPtr, "Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                  "how bored you feel when you are asked with %d questions for this round \n"
                  "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                  "that you feel the most bored and 1 denotes that you feel the least bored.) %d\n\n\n", numOfQuestion, sat);

    questions = numOfQuestion;
    return original_P->points[p_result->id];
}
