#include "user_study.h"


std::vector<point_t *> random_choose(point_set_t *p_set)
{
    std::vector<point_t *> p;
    int count = 0, index = 0, dim = p_set->points[0]->dim;
    while(count < 1000)
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


void normalize_set(std::vector<point_t *> p_set, double *max, double *min)
{
    int dim = p_set[0]->dim;
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


int user_study_main_body(){

    // parameters
    int k = 2;
    int num_repeat = 1;
    int w = 4;
    double theta = 0.05;

    srand(time(NULL));
    point_set_t *P0 = read_points((char*)"car.txt");

    std::vector<point_t *> p_set, p_1, p_2;
    p_1 = random_choose(P0);
    int dim = p_1[0]->dim;
    double max[dim], min[dim];
    normalize_set(p_1, max, min);
    skyband(p_1, p_set, 1, P0->numberOfPoints);
    p_2 = scale_up(p_set);
    point_set_t *P = point_reload(p_set);

    for(int i = 0; i < num_repeat; i++){

        // the exact Algorithm
        // exact_rev::Exact_revised(p_set, P0, k, w, SCORE_SELECT);

        // the sampling Algorithm
        // sampling::sampling(p_set, P0, k, w, SCORE_SELECT);

        // the optimal Algorithm
        optimal::optimal(p_set, P0, k, w, PURE_RANDOM);

        // the HDPI Algorithm
        // rev_HDPI(p_set, P0, w);

        // PointPrune Algorithm
        // PointPrune_v2(p_set, P0, 3, w, max, min);

        // Algorithm: Active Ranking
        // Active_Ranking(p_set, P0, w);
        
        // Algorithm: Preference Learning
        // Preference_Learning(p_set, P0, w);
    }
    release_point_set(P, true);
    release_point_set(P0, true);
    return 0;

}