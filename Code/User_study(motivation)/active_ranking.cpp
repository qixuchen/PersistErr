#include "active_ranking.h"
#include <sys/time.h>



/**
 * @brief Ask user questions and give a ranking
 * @param original_set 		The original dataset
 * @param u 				The linear function
 * @param k 				The threshold top-k
 */
point_t* Active_Ranking(FILE *wPtr, std::vector<point_t *> p_set, point_set_t* P0, int k, int &questions)
{
    int dim = p_set[0]->dim, numOfQuestion = 0;
    point_random(p_set);

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
                    numOfQuestion++;
                    printf("%d\n", numOfQuestion);
                    if(numOfQuestion>100)
                    {
                        printf("\n--------------------------------------------------------\n");
                        printf("%s %10d \n", "No. of questions asked: ", numOfQuestion);
                        printf("--------------------------------------------------------\n");
                        printf("Rcommended cars:\n");
                        printf("--------------------------------------------------------\n");
                        printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
                        int i = rand()%k;
                        printf("--------------------------------------------------------\n");
                        printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[current_use[i]->id]->coord[0], P0->points[current_use[i]->id]->coord[1],
                                   P0->points[current_use[i]->id]->coord[2], P0->points[current_use[i]->id]->coord[3]);
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
                        fprintf(wPtr, "|%30s |%10d |\n", "Active_ranking", numOfQuestion);
                        fprintf(wPtr, "--------------------------------------------------------\n");
                        fprintf(wPtr, "Rcommended cars:\n");
                        fprintf(wPtr, "--------------------------------------------------------\n");
                        fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
                        fprintf(wPtr, "--------------------------------------------------------\n");
                        fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[current_use[i]->id]->coord[0], P0->points[current_use[i]->id]->coord[1],
                                    P0->points[current_use[i]->id]->coord[2], P0->points[current_use[i]->id]->coord[3]);
                        fprintf(wPtr, "---------------------------------------------------------\n");
                        fprintf(wPtr, "Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                                      "how bored you feel when you are asked with %d questions for this round \n"
                                      "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                                      "that you feel the most bored and 1 denotes that you feel the least bored.) %d\n\n\n", numOfQuestion, sat);

                        questions = numOfQuestion;
                        return current_use[i];
                    }
                    int option = show_to_user(P0, p_set[i]->id, current_use[j]->id);

                    if (option == 1)
                    {
                        halfspace_t *half = alloc_halfspace(current_use[j], p_set[i], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        bool feasible0 = get_extreme_pts_refine_halfspaces_active(R_half_set);
                        if(!feasible0)
                        {
                            R_half_set->halfspaces.pop_back();
                        }
                    }
                    else
                    {
                        halfspace_t *half = alloc_halfspace(p_set[i], current_use[j], 0, true);
                        R_half_set->halfspaces.push_back(half);
                        bool feasible0 = get_extreme_pts_refine_halfspaces_active(R_half_set);
                        if(!feasible0)
                        {
                            R_half_set->halfspaces.pop_back();
                        }
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
    p_set.clear();
    p_set.shrink_to_fit();
    release_halfspace_set(R_half_set);

    //print
    printf("\n--------------------------------------------------------\n");
    printf("%s %10d \n", "No. of questions asked:", numOfQuestion);
    printf("--------------------------------------------------------\n");
    printf("Recommended cars:\n");
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
    int i = rand()%k;
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[current_use[i]->id]->coord[0], P0->points[current_use[i]->id]->coord[1],
               P0->points[current_use[i]->id]->coord[2], P0->points[current_use[i]->id]->coord[3]);
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
    fprintf(wPtr, "|%30s |%10d |\n", "Active_ranking", numOfQuestion);
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "Recommended cars:\n");
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[current_use[i]->id]->coord[0], P0->points[current_use[i]->id]->coord[1],
               P0->points[current_use[i]->id]->coord[2], P0->points[current_use[i]->id]->coord[3]);
    fprintf(wPtr, "---------------------------------------------------------\n");
    fprintf(wPtr, "Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                  "how bored you feel when you are asked with %d questions for this round \n"
                  "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                  "that you feel the most bored and 1 denotes that you feel the least bored.) %d\n\n\n", numOfQuestion, sat);

    questions = numOfQuestion;
    return current_use[i];
}
