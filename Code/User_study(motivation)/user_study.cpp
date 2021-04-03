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
#include <fstream>


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
    int k = 20, dim = 4;
    std::vector<point_t *> p_set, p_1, p_2;
    p_1 = random_choose(P0);
    normalize_set(p_1);
    skyband(p_1, p_set, k, P0->numberOfPoints);
    p_2 = scale_up(p_set);
    point_set_t *P = point_reload(p_set);
    //obatin the point set in different forms

    // the welcome message
    cout << "-------------------------Welcome to the recommending car system------------------------------\n"
         << "Please input your name: ";
    char *name = new char[20], *file_name = new char[40];
    cin >> name;
    cout << "1. In our research project, we want to ask as few questions as possible so that we could help\n"
         << "you to find some of your 20 most favorite cars in our second-hand car database. \n"
         << "2. The final car(s) returned by our system should be among your 20 most favorite cars.\n"
         << "3. There are different rounds in the system. We will ask you a list of consecutive questions\n"
         << "for each round. Each round involving consecutive questions correspond to a method in\n"
         << "our system.\n"
         << "4. You will be presented two cars each time and you need to choose the one you favor more.\n"
         << "5. Price 1049-95400 Year 2002-2015 Power 52-575 Used(km) 10000-125000\n\n\n";

    FILE *wPtr;
    sprintf(file_name, "../output/%s.txt", name);
    wPtr = (FILE *)fopen(file_name, "w");
    ifstream in_cp("utility.txt");
    point_t *u = alloc_point(dim);


    double* num_RH = new double[20], *num_HDPI_S = new double[20], *num_HDPI_A = new double[20];
    vector<point_t*> *res_RH = new vector<point_t*>[20], *res_HDPI_S = new vector<point_t*>[20], *res_HDPI_A = new vector<point_t*>[20];
    for (int i = 0; i < 4; i++)
        in_cp >> u->coord[i];

    printf("=========================Round 1=========================\n");
    RH(wPtr, p_set, P0, k, 1, u, num_RH, res_RH);

    printf("=========================Round 2=========================\n");
    HDPI_sampling(wPtr, p_set, P0, k, 1, u, num_HDPI_S, res_HDPI_S);

    printf("=========================Round 3=========================\n");
    HDPI_accurate(wPtr, p_set, P0, k, 1, u, num_HDPI_A, res_HDPI_A);


    printf("Now comes to the comparison section: \n"
           "Press 'Enter' to continue...");
    cin.get(); cin.get();
    algorithm_comparison(wPtr, P0, num_RH, res_RH, k);
    algorithm_comparison(wPtr, P0, num_HDPI_S, res_HDPI_S, k);
    algorithm_comparison(wPtr, P0, num_HDPI_A, res_HDPI_A, k);


    release_point_set(P, true);
    fclose(wPtr);
    in_cp.close();

    return 0;
}
