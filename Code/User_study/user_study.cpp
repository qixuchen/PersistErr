#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "maxUtility.h"
#include "preferLearn.h"
#include "HDPI.h"
#include "RH.h"
#include "active_ranking.h"
#include "Qhull/qhull_build.h"
#include <vector>
#include <ctime>
#include <sys/time.h>


std::vector<point_t *> random_choose(point_set_t *p_set)
{
    std::vector<point_t *> p;
    int count = 0, index = 0, dim = p_set->points[0]->dim;
    while (count < 1000)
    {
        index = rand() % p_set->numberOfPoints;
        if(1000 < p_set->points[index]->coord[0] && p_set->points[index]->coord[0] < 100000
            && 2001 < p_set->points[index]->coord[1] && p_set->points[index]->coord[1]< 2017
            && 50 < p_set->points[index]->coord[2] && p_set->points[index]->coord[2]< 1000
            && 5000 < p_set->points[index]->coord[3] && p_set->points[index]->coord[3]< 150000
        )
        {
            bool is_same = false;
            for (int j = 0; j < p.size(); j++)
            {
                if (is_same_point(p_set->points[index], p[j]))
                {
                    is_same = true;
                    break;
                }
            }
            if (!is_same)
            {
                point_t *pp = alloc_point(dim);
                for (int i = 0; i < dim; i++)
                {
                    pp->coord[i] = p_set->points[index]->coord[i];
                }
                pp->dim = p_set->points[index]->dim;
                pp->id = p_set->points[index]->id;
                p.push_back(pp);
                count++;
            }
        }
    }
    return p;
}

void normalize_set(std::vector<point_t *> p_set)
{
    int dim = p_set[0]->dim;
    double max[4];
    double min[4];
    for (int j = 0; j < dim; j++)
    {
        max[j] = -1;
        min[j] = 999999;
    }
    for (int i = 0; i < p_set.size(); i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (max[j] < p_set[i]->coord[j])
            {
                max[j] = p_set[i]->coord[j];
            }
            if (min[j] > p_set[i]->coord[j])
            {
                min[j] = p_set[i]->coord[j];
            }
        }
    }
    for (int i = 0; i < p_set.size(); i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (j == 0 || j == 3)
            {
                p_set[i]->coord[j] = 1 - (p_set[i]->coord[j] - min[j]) / (max[j] - min[j]);
            }
            else
            {
                p_set[i]->coord[j] = (p_set[i]->coord[j] - min[j]) / (max[j] - min[j]);
            }
        }
    }
}

std::vector<point_t *> scale_up(std::vector<point_t *> p_set)
{
    int dim = p_set[0]->dim;
    std::vector<point_t *> P;
    for (int i = 0; i < p_set.size(); i++)
    {
        point_t *p = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            p->coord[j] = p_set[i]->coord[j] * 1000;
        }
        p->id = p_set[i]->id;
        p->dim = p_set[i]->dim;
        P.push_back(p);
    }
    return P;
}

int demo()
{
    point_set_t *P0 = read_points((char*)"car.txt");
    int k = 20;
    std::vector<point_t *> p_set, p_1, p_2;
    p_1 = random_choose(P0);
    normalize_set(p_1);
    skyband(p_1, p_set, k, P0->numberOfPoints);
    p_2 = scale_up(p_set);
    point_set_t *P = point_reload(p_set);
    //obatin the point set in different forms

    // the welcome message
    printf("-------------------------Welcome to the recommending car system------------------------------\n");
    printf("1. In our research project, we want to ask as few questions as possible so that we could help\n"
           "you to find one of your 20 most favorite cars in our used car database. \n"
           "2. The final car(s) returned by our system should be one of your 20 most favorite cars.\n"
           "3. There are different rounds in the system. We will ask you a list of consecutive questions\n"
           "for each round. Each round involving consecutive questions correspond to a method in\n"
           "our system.\n"
           "4. You will be presented two cars each time and you need to choose the one you favor more.\n"
           "5. Price 1049-95400 Year 2002-2015 Power 52-575 Used(km) 10000-125000\n\n\n");

    FILE *wPtr;
    wPtr = (FILE *)fopen("../output/result.txt", "w");
    point_t* r[7];
    int questions[7];

    printf("=========================Round 1=========================\n");
    // the UH-Simplex algorithm
    r[6] = max_utility(wPtr, P, P0, SIMPLEX, questions[6]);

    printf("=========================Round 2=========================\n");
    printf("waiting...\n");
    //Algorithm: Preference Learning
    r[0] = Preference_Learning(wPtr, p_set, P0, k, questions[0]);

    printf("=========================Round 3=========================\n");
    //Algorithm: RH
    r[2] = RH(wPtr, p_set, P0, k, questions[2]);

    printf("=========================Round 4=========================\n");
    //the UH-Random algorithm
    r[5] = max_utility(wPtr, P, P0, RANDOM, questions[5]);

    printf("=========================Round 5=========================\n");
    //Algorithm: Active Ranking
    r[1] = Active_Ranking(wPtr, p_2, P0, k, questions[1]);

    printf("=========================Round 6=========================\n");
    //Algorithm HDPI
    r[3] = HDPI_sampling(wPtr, p_set, P0, k, questions[3]);

    printf("=========================Round 7=========================\n");
    r[4] = HDPI_accurate(wPtr, p_set, P0, k, questions[4]);

    int Rindex[7] = {2, 5, 3, 6, 7, 4, 1};
    printf("Now comes to the comparison section\n");
    for(int i = 2; i < 6; i++)
    {
        for(int j = i + 1; j< 7; j++)
        {
            printf("=========================== Round %d ===========================\n", Rindex[i]);
            fprintf(wPtr, "=========================== Round %d ===========================\n", Rindex[i]);
            printf("No. of questions asked: %d \n", questions[i]);
            fprintf(wPtr, "No. of questions asked: %d \n", questions[i]);
            printf("Recommended cars:\n");
            fprintf(wPtr, "Recommended cars:\n");
            printf("--------------------------------------------------------\n");
            fprintf(wPtr, "--------------------------------------------------------\n");
            printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
            fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
            printf("---------------------------------------------------------\n");
            fprintf(wPtr, "---------------------------------------------------------\n");
            printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[r[i]->id]->coord[0], P0->points[r[i]->id]->coord[1],
                    P0->points[r[i]->id]->coord[2], P0->points[r[i]->id]->coord[3]);
            fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[r[i]->id]->coord[0], P0->points[r[i]->id]->coord[1],
                    P0->points[r[i]->id]->coord[2], P0->points[r[i]->id]->coord[3]);
            printf("---------------------------------------------------------\n");
            fprintf(wPtr, "---------------------------------------------------------\n");
            printf("=========================== Round %d ===========================\n", Rindex[j]);
            fprintf(wPtr, "=========================== Round %d ===========================\n", Rindex[j]);
            printf("No. of questions asked: %d \n", questions[j]);
            fprintf(wPtr, "No. of questions asked: %d \n", questions[j]);
            printf("Recommended cars:\n");
            fprintf(wPtr, "Recommended cars:\n");
            printf("--------------------------------------------------------\n");
            fprintf(wPtr, "--------------------------------------------------------\n");
            printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
            fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
            printf("---------------------------------------------------------\n");
            fprintf(wPtr, "---------------------------------------------------------\n");
            printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[r[j]->id]->coord[0], P0->points[r[j]->id]->coord[1],
                    P0->points[r[j]->id]->coord[2], P0->points[r[j]->id]->coord[3]);
            fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[r[j]->id]->coord[0], P0->points[r[j]->id]->coord[1],
                    P0->points[r[j]->id]->coord[2], P0->points[r[j]->id]->coord[3]);
            printf("---------------------------------------------------------\n");
            fprintf(wPtr, "---------------------------------------------------------\n");
            printf("Which round are you satisfied more? \n"
                   "Please enter:\n"
                   "  (1) 1 for Round %d \n"
                   "  (2) 2 for Round %d \n"
                   "  (3) 3 for both Round %d and Round %d.\n"
                   "Note that all car(s) returned by each round must be one of your 20 most favorite cars\n"
                   "in our used car database. Please answer these based on two criteria at the same time:\n"
                   "(a) No. of questions asked and (b) The recommended car(s)\n"
                   "You answer: ", Rindex[i], Rindex[j], Rindex[i], Rindex[j]);
            int sat = 4;
            while(sat!= 1 && sat!= 2 && sat!= 3)
            {
                scanf("%d", &sat);
            }
            fprintf(wPtr, "Which round are you satisfied more?  %d\n\n", sat);
            printf("\n\n");
        }
    }

    release_point_set(P, true);
    fprintf(wPtr, "\n");
    fclose(wPtr);

    return 0;
}
