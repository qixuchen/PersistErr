#include "used_function.h"


int show_to_user(point_set_t* P, int p_idx, int q_idx)
{

    int option = 0;
    // ask the user for the better car among two given options
    while (option != 1 && option != 2)
    {
        printf("Please choose the car you favor more:\n");
        printf("--------------------------------------------------------\n");
        printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
        printf("--------------------------------------------------------\n");
        printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 1", P->points[p_idx]->coord[0], P->points[p_idx]->coord[1], P->points[p_idx]->coord[2], P->points[p_idx]->coord[3]);
        printf("--------------------------------------------------------\n");
        printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Option 2", P->points[q_idx]->coord[0], P->points[q_idx]->coord[1], P->points[q_idx]->coord[2], P->points[q_idx]->coord[3]);
        printf("--------------------------------------------------------\n");
        printf("Your choice: ");
        scanf("%d", &option);
    }

    return option;
}


/**
 * @brief Used to check whether these two points have the same coordinates
 * @param p1    Point 1
 * @param p2    Point 2
 * @return      Whether the points are the same
 *              1 same
 *              0 different
 */
bool is_same_point(point_t *p1, point_t *p2)
{
    int dim = p1->dim;
    for (int i = 0; i < dim; i++)
    {
        if (p1->coord[i] != p2->coord[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief Reload the points to the data structure vector
 *        Does not change the original dataset
 * @param original_set	The original dataset
 * @param p_set 	    The returned dataset
 */
void point_reload(point_set_t *original_set, std::vector<point_t *> &p_set)
{
    int M = original_set->numberOfPoints;
    for (int i = 0; i < M; i++)
    {
        p_set.push_back(original_set->points[i]);
    }
}

/**
 * @brief Reload the points to the data structure point_set
 *        Does not change the original dataset
 * @param original_set	The original dataset
 * @param p_set 		The returned dataset
 */
point_set_t *point_reload(std::vector<point_t *> p_set)
{
    int num = p_set.size(), dim = p_set[0]->dim;
    point_set_t *point_set = alloc_point_set(num);
    for (int i = 0; i < num; i++)
    {
        point_t *p = alloc_point(dim, i);
        for (int j = 0; j < dim; j++)
        {
            p->coord[j] = p_set[i]->coord[j];
        }
        p->id = p_set[i]->id;
        point_set->points[i] = p;
    }
    return point_set;
}

/*
 * @brief Reload the points to the data structure vector Randomly
 *		  Define "RandRate" to control how many point reinserted
 *        Does not change the original dataset
 * @param p_set 	The returned dataset where the points are inserted randomly
 */
void point_random(std::vector<point_t *> &p_set)
{
    int M = p_set.size();
    //Reinsert
    for (int i = 0; i < M * RandRate; i++)
    {
        int n = ((int) rand()) % M;
        point_t *pp = p_set[n];
        p_set.erase(p_set.begin() + n);
        p_set.push_back(pp);
    }
}


/*
 * @brief Reload the points to the data structure vector Randomly
 *		 Delete the overlapped points, remain only one
 *		 Define "RandRate" to control how many point reinserted
 * 		 Does not change the original dataset
 * @param original_set	The original dataset
 * @param p_set 		The returned dataset where the points are inserted randomly
 */
void point_reload_with_random_delete_same(point_set_t *original_set, std::vector<point_t *> &p_set)
{
    int M = original_set->numberOfPoints;
    int dim = original_set->points[0]->dim;
    //insert all the points
    p_set.push_back(original_set->points[0]);
    for (int i = 1; i < M; i++)
    {
        bool same_exist = false;
        for (int j = 0; j < p_set.size(); j++)
        {
            bool is_same = true;
            for (int w = 0; w < dim; w++)
            {
                if (original_set->points[i]->coord[w] != p_set[j]->coord[w])
                {
                    is_same = false;
                    break;
                }
            }
            if (is_same)
            {
                same_exist = true;
                break;
            }
        }
        if (!same_exist)
        {
            p_set.push_back(original_set->points[i]);
        }
    }
    //reinsert
    M = p_set.size();
    for (int i = 0; i < M * RandRate; i++)
    {
        int n = ((int) rand()) % M;
        point_t *pp = p_set[n];
        p_set.erase(p_set.begin() + n);
        p_set.push_back(pp);
    }
}

void delete_same(std::vector<point_t *> p_set, std::vector<point_t *> &p0)
{
    int M = p_set.size();
    //insert all the points
    p0.push_back(p_set[0]);
    for (int i = 1; i < M; i++)
    {
        bool same_exist = false;
        for (int j = 0; j < p0.size(); j++)
        {
            if (is_same_point(p0[j], p_set[i]))
            {
                same_exist = true;
                break;
            }
        }
        if (!same_exist)
        {
            p0.push_back(p_set[i]);
        }
    }
}

/*
 * @brief Sort points based on their utility w.r.t. linear function u
 * 		  Does not change the original dataset
 *@param p_set 			The original dataset
 *@param return_point	The returned dataset containing all the sorted points
 *@param u 				The linear function
 */
void sort_point(std::vector<point_t *> p_set, std::vector<point_t *> &return_point, point_t *u)
{
    int M = p_set.size();
    return_point.push_back(p_set[0]);
    for (int i = 1; i < M; i++)
    {
        double v0 = dot_prod(u, p_set[i]);
        int left = 0, right = return_point.size() - 1;
        //find the place for p_set[i] in return_point
        //record the place index in "left"
        while (left <= right)
        {
            int middle = (left + right) / 2;
            double v = dot_prod(u, return_point[middle]);
            if (v0 > v)
            {
                right = middle - 1;
            }
            else
            {
                left = middle + 1;
            }
        }
        return_point.insert(return_point.begin() + left, p_set[i]);
    }
    /*
    for(int i=0; i<return_point.size();i++)
    {
        printf("point %d  %lf %lf\n", return_point[i]->id, return_point[i]->coord[0], return_point[i]->coord[1]);
    }
    */
}

/*
 * @brief Calculate the distance between a point and a hyperplane
 * @param hyper 	The hyperplane
 * @param p 		The point
 * @return The distance(Due to the property of C++, the distance can not be accurate enough)
 */
double calculate_distance(hyperplane_t *hyper, point_t *p)
{
    double sum = 0, value = 0;
    int dim = hyper->normal->dim;
    for (int i = 0; i < dim; i++)
    {
        value = value + hyper->normal->coord[i] * p->coord[i];
        sum = sum + hyper->normal->coord[i] * hyper->normal->coord[i];
    }
    sum = sqrt(sum);
    if (value >= 0)
    {
        return value / sum;
    }
    else
    {
        return -value / sum;
    }
}

/*
 * @brief Calculate the distance between a point and a hyperplane
 * @param hyper 	The hyperplane
 * @param p 		The point
 * @return The distance(Due to the property of C++, the distance can not be accurate enough)
 */
double calculate_absolute_distance(hyperplane_t *hyper, point_t *p)
{
    double sum = 0, value = 0;
    int dim = hyper->normal->dim;
    for (int i = 0; i < dim; i++)
    {
        value = value + hyper->normal->coord[i] * p->coord[i];
        sum = sum + hyper->normal->coord[i] * hyper->normal->coord[i];
    }
    sum = sqrt(sum);
    return value / sum;
}

//@brief Find the top-k point w.r.t linear function u
//@param u 			The linear function
//@param p_set 		The original dataset
//@param top 		The returned top-k point
//@param k 			The threhold of top-k
void find_top_k(point_t *u, point_set_t *p_set, std::vector<point_t *> &top, int k)
{
    std::vector<double> value;
    //set the initial k points
    top.push_back(p_set->points[0]);
    value.push_back(dot_prod(u, p_set->points[0]));
    for (int j = 1; j < k; j++)
    {
        int z;
        double sum0 = dot_prod(u, p_set->points[j]);
        for (z = 0; z < value.size(); z++)
        {
            if (sum0 > value[z])
            {
                break;
            }
        }
        top.insert(top.begin() + z, p_set->points[j]);
        value.insert(value.begin() + z, sum0);
    }

    for (int j = k; j < p_set->numberOfPoints; j++)
    {
        int z;
        double sum0 = dot_prod(u, p_set->points[j]);
        for (z = k; z > 0; z--)
        {
            if (sum0 <= value[z - 1])
            {
                break;
            }
        }
        if (z < k)
        {
            top.insert(top.begin() + z, p_set->points[j]);
            value.insert(value.begin() + z, sum0);
            top.pop_back();
            value.pop_back();
        }
    }
}

//@brief Find the top-k point w.r.t linear function u
//@param u 			The linear function
//@param p_set 		The original dataset(in the form of vector)
//@param top 		The returned top-k point
//@param k 			The threhold of top-k
void find_top_k(point_t *u, std::vector<point_t *> p_set, std::vector<point_t *> &top, int k)
{
    std::vector<double> value;
    //set the initial k points
    top.push_back(p_set[0]);
    value.push_back(dot_prod(u, p_set[0]));
    for (int j = 1; j < k; j++)
    {
        int z;
        double sum0 = dot_prod(u, p_set[j]);
        for (z = 0; z < value.size(); z++)
        {
            if (sum0 > value[z])
            {
                break;
            }
        }
        top.insert(top.begin() + z, p_set[j]);
        value.insert(value.begin() + z, sum0);
    }

    for (int j = k; j < p_set.size(); j++)
    {
        int z;
        double sum0 = dot_prod(u, p_set[j]);
        for (z = k; z > 0; z--)
        {
            if (sum0 <= value[z - 1])
            {
                break;
            }
        }
        if (z < k)
        {
            top.insert(top.begin() + z, p_set[j]);
            value.insert(value.begin() + z, sum0);
            top.pop_back();
            value.pop_back();
        }
    }
}

//@brief Use sampling to find all the points which is able to be top-1 at some utility vector
//@param p_set 		The point set containing all the points
//@param top_set	The returned point set containing all the possible top-1 point
//@param u 			The utility vector. For user, point_t* u = alloc_point(dim)
//@param level		The number of dimensions we have set. For user, only need to set level=0
//@param used_seg	The range which has been assigned to the u[i]. For user, set rest_seg=0
void find_top1_sampling(std::vector<point_t *> p_set, std::vector<point_t *> &top_set,
                        point_t *u, int level, int used_seg)
{
    int dim = p_set[0]->dim;
    int M = p_set.size();
    double segment = 10, length = 1 / segment;
    if (level >= dim - 2)
    {
        for (int j = 0; j <= segment - used_seg; j++)
        {
            u->coord[level] = j * length;
            u->coord[dim - 1] = 1;
            for (int i = 0; i < dim - 1; i++)
            {
                u->coord[dim - 1] -= u->coord[i];
            }
            /*
            printf("u: ");
            for(int i=0; i < dim; i++)
            {
                printf("%lf  ", u->coord[i]);
            }
            printf("\n");
            */
            //Find the point with max utility w.r.t u
            double maxValue = 0;
            int maxIdx = 0;
            for (int i = 0; i < M; i++)
            {
                double value = dot_prod(u, p_set[i]);//utility of the points
                if (value > maxValue)
                {
                    maxValue = value;
                    maxIdx = i;
                }
            }
            //Check if it is already in top_set
            bool is_inside = false;
            for (int i = 0; i < top_set.size(); i++)
            {
                if (top_set[i]->id == p_set[maxIdx]->id)
                {
                    is_inside = true;
                    break;
                }
            }
            if (!is_inside)
            {
                top_set.push_back(p_set[maxIdx]);
                //printf("push point id%d  ", skyline->points[maxIdx]->id);
            }
        }
    }
    else
    {
        for (int i = 0; i <= segment - used_seg; i++)
        {
            u->coord[level] = i * length;
            find_top1_sampling(p_set, top_set, u, level + 1, used_seg + i);
        }
    }
    //printf("top1_size%d\n", p_set.size());
}

//@brief Use LP to check each point, whether it is able to be top-1 at some utility vector
//@param p_set 		The point set containing all the points
//@param top_set	The returned point set containing all the possible top-1 point
void find_top1_accurate(std::vector<point_t *> p_set, std::vector<point_t *> &top_set)
{
    int M = p_set.size();
    if (M < 1)
    {
        printf("%s\n", "Error: The number of points is smaller than 1.");
        return;
    }
    int dim = p_set[0]->dim;

    //use i-th point as the pivot
    for (int i = 0; i < M; i++)
    {
        vector<hyperplane_t *> utility_hyperplane;
        point_t *normal;
        normal = alloc_point(dim);
        for (int i = 0; i < dim; i++)
            normal->coord[i] = 1;
        utility_hyperplane.push_back(alloc_hyperplane(normal, -1));
        //Note all the inequality are <=
        for (int a = 0; a < dim; a++)
        {
            normal = alloc_point(dim);
            for (int b = 0; b < dim; b++)
            {
                if (a == b)
                {
                    normal->coord[b] = -1;
                }
                else
                {
                    normal->coord[b] = 0;
                }
            }
            utility_hyperplane.push_back(alloc_hyperplane(normal, 0));
        }
        for (int j = 0; j < M; j++)
        {
            if (i != j)
            {
                normal = alloc_point(dim);
                for (int b = 0; b < dim; b++)
                {
                    normal->coord[b] = p_set[j]->coord[b] - p_set[i]->coord[b];
                }
                utility_hyperplane.push_back(alloc_hyperplane(normal, 0));
            }
        }
        point_t *feasible_pt = find_feasible(utility_hyperplane);

        if (feasible_pt != NULL)
        {
            top_set.push_back(p_set[i]);
        }
    }
    /*for(int i=0; i<top_set.size();i++)
    {
         printf("point %d\n", top_set[i]->id);
    }
    */
}

void find_top1_qhull(std::vector<point_t *> p_set, std::vector<point_t *> &top)
{
    int dim = p_set[0]->dim;
    int M = p_set.size();
    FILE *rPtr = NULL, *wPtr = NULL;
    rPtr = (FILE *) fopen("../output/point.txt", "w");
    // write the feasible point
    fprintf(rPtr, "%i\n%i\n", dim, M);
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < dim; j++)
            fprintf(rPtr, "%lf ", p_set[i]->coord[j]);
        fprintf(rPtr, "\n");
    }
    fclose(rPtr);
    rPtr = (FILE *) fopen("../output/point.txt", "r");
    wPtr = (FILE *) fopen("../output/top.txt", "w");
    Convex_H(rPtr, wPtr);
    fclose(rPtr);
    fclose(wPtr);

    //obtain the points
    wPtr = (FILE *) fopen("../output/top.txt", "r");
    fscanf(wPtr, "%i", &M);
    int index;
    for (int i = 0; i < M; i++)
    {
        fscanf(wPtr, "%i", &index);
        top.push_back(p_set[index]);
    }
    fclose(wPtr);
}

/**
 * @brief Find all the points which are not dominated by >=k points
 * @param p_set 			The original dataset
 * @param return_point 	The returned points which are not dominated by >=k points
 * @param k 				The threshold
 */
void skyband(point_set_t *p_set, std::vector<point_t *> &return_point, int k)
{
    int num = p_set->numberOfPoints;
    point_t *pt;
    int *dominated = new int[num + 1];
    for (int i = 0; i < num; i++)
    {
        pt = p_set->points[i];
        dominated[pt->id] = 0;
        //check if pt is dominated k times by the return_point so far   
        for (int j = 0; j < return_point.size() && dominated[pt->id] < k; j++)
        {
            if (dominates_same(return_point[j], pt))
            {
                dominated[pt->id]++;
            }
        }
        if (dominated[pt->id] < k)
        {
            // eliminate any points in return_point dominated k times
            int m = return_point.size();
            int index = 0;
            for (int j = 0; j < m; j++)
            {
                if (dominates_same(pt, return_point[index]))
                {
                    dominated[return_point[index]->id]++;
                }
                if (dominated[return_point[index]->id] >= k)
                {
                    return_point.erase(return_point.begin() + index);
                }
                else
                {
                    index++;
                }
            }
            return_point.push_back(pt);
        }
    }
    delete[] dominated;
    //printf("size%d\n", return_point.size());
    /*
    for(int i=0; i<return_point.size();i++)
    {
        printf("point %d %lf %lf\n", return_point[i]->id, return_point[i]->coord[0], return_point[i]->coord[1]);
    }
    */
}

/**
 * @brief Find all the points which are not dominated by >=k points
 * @param p_set 		The original dataset
 * @param return_point 	The returned points which are not dominated by >=k points
 * @param k 			The threshold
 * @param id_num        The maximum id of points
 */
void skyband(std::vector<point_t *> p_set, std::vector<point_t *> &return_point, int k, int id_num)
{
    int num = p_set.size();
    point_t *pt;
    int *dominated = new int[id_num + 1];
    for (int i = 0; i < num; i++)
    {
        pt = p_set[i];
        dominated[pt->id] = 0;
        //check if pt is dominated k times by the return_point so far 
        for (int j = 0; j < return_point.size() && dominated[pt->id] < k; j++)
        {
            if (dominates_same(return_point[j], pt))
            {
                dominated[pt->id]++;
            }
        }
        if (dominated[pt->id] < k)
        {
            //eliminate any points in return_point dominated k times
            int m = return_point.size();
            int index = 0;
            for (int j = 0; j < m; j++)
            {
                if (dominates_same(pt, return_point[index]))
                {
                    dominated[return_point[index]->id]++;
                }
                if (dominated[return_point[index]->id] >= k)
                {
                    return_point.erase(return_point.begin() + index);
                }
                else
                {
                    index++;
                }
            }
            return_point.push_back(pt);
        }
    }

    delete[] dominated;
    //printf("size%d\n", return_point.size());
    /*
    for(int i=0; i<return_point.size();i++)
    {
        printf("point %d %lf %lf\n", return_point[i]->id, return_point[i]->coord[0], return_point[i]->coord[1]);
    }
    */
}


/**
 * @brief Find all the points which are not dominated by >=k points
 * @param p_set 			The original dataset
 * @param return_point 	The returned points which are not dominated by >=k points
 * @param k 				The threshold
 */
void skyband(std::vector<point_t *> p_set, std::vector<point_t *> &return_point, int k)
{
    int num = p_set.size();
    point_t *pt;
    int *dominated = new int[num + 1];
    for (int i = 0; i < num; i++)
    {
        pt = p_set[i];
        dominated[pt->id] = 0;
        //check if pt is dominated k times by the return_point so far
        for (int j = 0; j < return_point.size() && dominated[pt->id] < k; j++)
        {
            if (dominates_same(return_point[j], pt))
            {
                dominated[pt->id]++;
            }
        }
        if (dominated[pt->id] < k)
        {
            //eliminate any points in return_point dominated k times
            int m = return_point.size();
            int index = 0;
            for (int j = 0; j < m; j++)
            {
                if (dominates_same(pt, return_point[index]))
                {
                    dominated[return_point[index]->id]++;
                }
                if (dominated[return_point[index]->id] >= k)
                {
                    return_point.erase(return_point.begin() + index);
                }
                else
                {
                    index++;
                }
            }
            return_point.push_back(pt);
        }
    }

    delete[] dominated;
    //printf("size%d\n", return_point.size());
    /*
    for(int i=0; i<return_point.size();i++)
    {
        printf("point %d %lf %lf\n", return_point[i]->id, return_point[i]->coord[0], return_point[i]->coord[1]);
    }
    */
}


/*
 * @brief Find all the points which are skyline points
 * param p_set 			The original dataset(in the form of vector)
 * param return_point 	The returned points which are skyline points
 */
void skyline_c(std::vector<point_t *> p_set, std::vector<point_t *> &return_point)
{
    int num = p_set.size();
    point_t *pt;
    bool dominated;
    for (int i = 0; i < num; i++)
    {
        pt = p_set[i];
        dominated = false;
        //check if pt is dominated k times by the return_point so far
        for (int j = 0; j < return_point.size() && !dominated; j++)
        {
            if (dominates_same(return_point[j], pt))
            {
                dominated = true;
            }
        }
        if (!dominated)
        {
            //eliminate any points in return_point dominated
            int m = return_point.size();
            int index = 0;
            for (int j = 0; j < m; j++)
            {
                if (dominates_same(pt, return_point[index]))
                {
                    return_point.erase(return_point.begin() + index);
                }
                else
                {
                    index++;
                }
            }
            return_point.push_back(pt);
        }
    }
}


halfspace_set_t *R_initial(int dim)
{
    halfspace_set_t *R_half_set = alloc_halfspace_set(dim);
    point_t *normal;
    point_t *normal2 = alloc_point(dim);
    for (int i = 0; i < dim; i++)
    {
        normal2->coord[i] = 0;
    }
    for (int i = 0; i < dim; i++)
    {
        normal = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            if (j == i)
            {
                normal->coord[j] = -1;
            }
            else
            {
                normal->coord[j] = 0;
            }
        }
        R_half_set->halfspaces.push_back(alloc_halfspace(normal, normal2, 0, true));
    }
    //set extreme points
    get_extreme_pts_refine_halfspaces_alg1(R_half_set);
    return R_half_set;
}

//@brief Check the relation between the hyperplane and the half_set
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param half_set	The half_set/Intersection of the halfspace
//@return The relation	1: half_set on the positive side of the hyperplane
//						-1: half_set on the negative side of the hyperplane
//						0: half_set intersects with the hyperplane
//						-2: Error for check situation
int check_situation(hyperplane_t *hyper, halfspace_set_t *half_set)
{
    int M = half_set->ext_pts.size();
    if (M < 1)
    {
        printf("%s\n", "None of the ext_pts in the set.");
        return -2;
    }
    int D = half_set->ext_pts[0]->dim;
    int posi = 0, nega = 0;
    for (int i = 0; i < M; i++)
    {
        double sum = 0;
        for (int j = 0; j < D; j++)
        {
            //printf("hyper %lf %lf \n", hyper->normal->coord[j], half_set->ext_pts[i]->coord[j]);
            sum += (hyper->normal->coord[j] * half_set->ext_pts[i]->coord[j]);
        }
        if (sum > Precision)
        {
            posi++;
        }
        else if (sum < -Precision)
        {
            nega++;
        }

        if (posi > 0 && nega > 0)
        {
            return 0;
        }
    }
    if (posi > 0)
    {
        return 1;
    }
    else if (nega > 0)
    {
        return -1;
    }
    printf("%s\n", "Error for check situation.");
    return -2;
}



//@brief Check the relation between the hyperplane and the half_set
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param half_set	The half_set/Intersection of the halfspace
//@return The relation	1: half_set on the positive side of the hyperplane
//						-1: half_set on the negative side of the hyperplane
//						0: half_set intersects with the hyperplane
//						-2: Error for check situation
int check_situation(hyperplane_t* hyper, vector<point_t *> ext_pts)
{
    int M = ext_pts.size();
    if (M < 1)
    {
        printf("%s\n", "None of the ext_pts in the set.");
        return -2;
    }
    int D = ext_pts[0]->dim;
    int posi = 0, nega = 0;
    for (int i = 0; i < M; i++)
    {
        double sum = 0;
        for (int j = 0; j < D; j++)
        {
            //printf("hyper %lf %lf \n", hyper->normal->coord[j], half_set->ext_pts[i]->coord[j]);
            sum += (hyper->normal->coord[j] * ext_pts[i]->coord[j]);
        }
        if (sum > Precision)
        {
            posi++;
        }
        else if (sum < -Precision)
        {
            nega++;
        }

        if (posi > 0 && nega > 0)
        {
            return 0;
        }
    }
    if (posi > 0)
    {
        return 1;
    }
    else if (nega > 0)
    {
        return -1;
    }
    //printf("%s\n", "Error for check situation.");
    return -2;
}



//@brief Print the information of the chosen choose_item
//@param choose_item_set 	The choose_item table
//@param i 					The index of the chosen choose_item
int print_choose_item_situation(std::vector<choose_item *> choose_item_set, int i)
{
    int dim = choose_item_set[0]->hyper->point1->dim;
    printf("\n%s\n", "*************************************************");
    printf("choose_item %d ", i);
    printf("p_1%d p_2%d \nvariable", choose_item_set[i]->hyper->point1->id, choose_item_set[i]->hyper->point2->id);
    for (int j = 0; j < dim; j++)
    {
        printf("%lf ", choose_item_set[i]->hyper->normal->coord[j]);
    }
    printf("\n%s: ", "positive");
    for (int j = 0; j < choose_item_set[i]->positive_side.size(); j++)
    {
        printf("%d ", choose_item_set[i]->positive_side[j]);
    }
    printf("\n%s: ", "negative");
    for (int j = 0; j < choose_item_set[i]->negative_side.size(); j++)
    {
        printf("%d ", choose_item_set[i]->negative_side[j]);
    }
    printf("\n%s: ", "intersect");
    for (int j = 0; j < choose_item_set[i]->intersect_case.size(); j++)
    {
        printf("%d ", choose_item_set[i]->intersect_case[j]);
    }
    printf("\n%s\n", "*************************************************");
    return 1;
}

//@brief Check the relation between the hyperplane and the half_set
//		 Use bounding sphere/bounding rectangle to accelerate
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param half_set	The half_set/Intersection of the halfspace
//@param method 	0: use bounding sphere
//					1: use bounding rectangle
//@return The relation	1: half_set on the positive side of the hyperplane
//						-1: half_set on the negative side of the hyperplane
//						0: half_set intersects with the hyperplane
//						-2: Error for check situation
int check_situation_accelerate(hyperplane_t *hyper, halfspace_set_t *half_set, int method)
{
    int M = half_set->ext_pts.size();
    if (M < 1)
    {
        printf("%s\n", "None of the ext_pts in the set.");
        return -2;
    }
    int sit = 0;
    if (method == 0)
    {
        sit = check_situation_bounding_sphere(hyper, half_set);
    }
    else
    {
        sit = check_situation_bounding_rec(hyper, half_set);
    }
    if (sit == 1)
    {
        return 1;
    }
    else if (sit == -1)
    {
        return -1;
    }
    int D = half_set->ext_pts[0]->dim;
    int posi = 0, nega = 0;
    for (int i = 0; i < M; i++)
    {
        double sum = 0;
        for (int j = 0; j < D; j++)
        {
            sum += (hyper->normal->coord[j] * half_set->ext_pts[i]->coord[j]);
        }
        if (sum > Precision / 2)
        {
            posi++;
        }
        else if (sum < -Precision / 2)
        {
            nega++;
        }

        if (posi > 0 && nega > 0)
        {
            return 0;
        }
    }
    if (posi > 0)
    {
        return 1;
    }
    else if (nega > 0)
    {
        return -1;
    }
    //printf("%s\n", "Error for check situation.");
    return 0;
}


int check_situation_accelerate(hyperplane_t *hyper, halfspace_set_t *half_set, int method, double &check_nnum,
                               double &check_nnum_yes)
{
    check_nnum++;
    int M = half_set->ext_pts.size();
    if (M < 1)
    {
        printf("%s\n", "None of the ext_pts in the set.");
        return -2;
    }
    int sit = 0;
    if (method == 0)
    {
        sit = check_situation_bounding_sphere(hyper, half_set);
    }
    else
    {
        sit = check_situation_bounding_rec(hyper, half_set);
    }
    if (sit == 1)
    {
        check_nnum_yes++;
        return 1;
    }
    else if (sit == -1)
    {
        check_nnum_yes++;
        return -1;
    }
    int D = half_set->ext_pts[0]->dim;
    int posi = 0, nega = 0;
    for (int i = 0; i < M; i++)
    {
        double sum = 0;
        for (int j = 0; j < D; j++)
        {
            sum += (hyper->normal->coord[j] * half_set->ext_pts[i]->coord[j]);
        }
        if (sum > Precision / 2)
        {
            posi++;
        }
        else if (sum < -Precision / 2)
        {
            nega++;
        }

        if (posi > 0 && nega > 0)
        {
            return 0;
        }
    }
    if (posi > 0)
    {
        return 1;
    }
    else if (nega > 0)
    {
        return -1;
    }
    //printf("%s\n", "Error for check situation.");
    return 0;
}

/**
 * @brief Check the relation between the hyperplane and the half_set
 *        Use bounding sphere/bounding rectangle to accelerate
 *        Use LP to check
 * @param hyper 		    The hyperplane
 * @param half_set	        The half_set/Intersection of the halfspace
 * @param method 	        Accelerate method
 *                          -0 use bounding sphere
 *                          -1 use bounding rectangle
 * @return The relation
 *                      -1: half_set on the positive side of the hyperplane
 *                      --1: half_set on the negative side of the hyperplane
 *                      -0: half_set intersects with the hyperplane
 *                      --2: Error for check situation
 */
int check_situation_accelerate_LP(hyperplane_t *hyper, halfspace_set_t *half_set, int method)
{
    int M = half_set->ext_pts.size();
    if (M < 1)
    {
        printf("%s\n", "None of the ext_pts in the set.");
        return -2;
    }
    int sit = 0;
    if (method == 0)
    {
        sit = check_situation_bounding_sphere(hyper, half_set);
    }
    else
    {
        sit = check_situation_bounding_rec(hyper, half_set);
    }
    if (sit == 1)
    {
        return 1;
    }
    else if (sit == -1)
    {
        return -1;
    }
    bool s1, s2;
    s1 = check_hyperplane_intersect(hyper, half_set, true);
    s2 = check_hyperplane_intersect(hyper, half_set, false);
    if (s1 && !s2)
    {
        return 1;
    }
    if (!s1 && s2)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

//@brief Check whether the half_set is on the positive side of the hyperplane
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param half_set	The half_set/Intersection of the halfspace
//@return The relation	1: half_set on the positive side of the hyperplane
//						0: half_set on the negative side of the hyperplane/intersects with the hyperplane
//						-2: Error for check situation
int check_situation_positive(hyperplane_t *hyper, halfspace_set_t *half_set)
{
    int M = half_set->ext_pts.size();
    if (M < 1)
    {
        printf("%s\n", "None of the ext_pts in the set.");
        return -2;
    }
    int D = half_set->ext_pts[0]->dim;
    for (int i = 0; i < M; i++)
    {
        double sum = 0;
        for (int j = 0; j < D; j++)
        {
            //printf("hyper %lf %lf \n", hyper->normal->coord[j], half_set->ext_pts[i]->coord[j]);
            sum += (hyper->normal->coord[j] * half_set->ext_pts[i]->coord[j]);
        }
        if (sum < -Precision / 2)
        {
            return 0;
        }
    }
    return 1;
}

//@brief Check whether the half_set is on the positive side of the hyperplane
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param ext_pts	All the extreme points
//@return The relation	1: half_set on the positive side of the hyperplane
//						0: half_set on the negative side of the hyperplane/intersects with the hyperplane
//						-2: Error for check situation
int check_situation_positive(hyperplane_t *hyper, vector<point_t *> ext_pts)
{
    int M = ext_pts.size();
    if (M < 1)
    {
        printf("%s\n", "None of the ext_pts in the set.");
        return -2;
    }
    int D = ext_pts[0]->dim;
    for (int i = 0; i < M; i++)
    {
        double sum = 0;
        for (int j = 0; j < D; j++)
        {
            sum += (hyper->normal->coord[j] * ext_pts[i]->coord[j]);
        }
        if (sum < -Precision / 2)
        {
            return 0;
        }
    }
    return 1;
}

/*
 * @brief Find points which could be the top-k points for any utility vector in half_set
 *        Not accurate. It needs to be used with function check_possible_top_k()
 * @param p_set			The dataset
 * @param half_set		The half_set/intersection of the halfspace
 * @param top_current 	THe possible top-k points
 * @return              If there are possible top-k points, return true.
 *                      Otherwise return false
 */
bool find_possible_topk(std::vector<point_t *> p_set, halfspace_set_t *half_set, int k,
                        std::vector<point_t *> &top_current)
{
    if (p_set.size() <= k)
    {
        printf("%s\n", "Number of points is smaller than k.");
        for (int i = 0; i < p_set.size(); i++)
        {
            top_current.push_back(p_set[i]);
        }
        return true;
    }
    for (int i = 0; i < half_set->ext_pts.size()&&i <= 2; i++)
    {
        //top       used to store the top-k point for a single ext_pts
        //value     used to store the utility of the top-k point for a single ext_pts
        std::vector<point_t *> top;
        std::vector<double> value;
        //set the initial k points
        top.push_back(p_set[0]);
        value.push_back(dot_prod(half_set->ext_pts[i], p_set[0]));
        for (int j = 1; j < k; j++)
        {
            int z;
            double sum0 = dot_prod(half_set->ext_pts[i], p_set[j]);
            for (z = 0; z < value.size(); z++)
            {
                if (sum0 > value[z])
                {
                    break;
                }
            }
            top.insert(top.begin() + z, p_set[j]);
            value.insert(value.begin() + z, sum0);
        }

        //insert the other points
        for (int j = k; j < p_set.size(); j++)
        {
            int z;
            double sum0 = dot_prod(half_set->ext_pts[i], p_set[j]);
            for (z = top.size(); z > 0; z--)
            {
                if (value[z - 1] > sum0)
                {
                    break;
                }
            }
            if (z < top.size() || (value[top.size() - 1] - sum0) < 0.000001)
            {
                top.insert(top.begin() + z, p_set[j]);
                value.insert(value.begin() + z, sum0);
                while ((top.size() > k) && value[k - 1] > value[top.size() - 1] + 0.000001)
                {
                    top.pop_back();
                    value.pop_back();
                }
            }
        }

        if (i == 0)
        {
            for (int j = 0; j < top.size(); j++)
            {
                top_current.push_back(top[j]);
            }
        }
        else
        {
            int scan_index = 0;
            double top_current_size = top_current.size();
            for (int j = 0; j < top_current_size; j++)
            {
                bool is_in = false;
                for (int a = 0; a < top.size(); a++)
                {
                    if (top_current[scan_index]->id == top[a]->id)
                    {
                        is_in = true;
                        break;
                    }
                }
                if (is_in)
                {
                    scan_index++;
                }
                else
                {
                    top_current.erase(top_current.begin() + scan_index);
                }
            }
        }
        if (top_current.size() < 1)
        {
            return false;
        }
    }
    return true;
}

/*
 * @brief Used to check whether there is a top-k points w.r.t any utility vector in half_set
 * @param p_set			The dataset containing all the points
 * @param half_set 		The half_set/intersection of the halfspace
 * @param k 			top-k
 * @param top_current 	The dataset containing all the possible top-k point found by function find_top_k_point_by_ext()
 * @return              If there is a top-k point w.r.t any utility vector in half_set, return it
 *                      Otherwise, return false
 */
point_t* check_possible_topk(std::vector<point_t *> p_set, halfspace_set_t *half_set, int k,
                             std::vector<point_t *> &top_current)
{
    point_t* point_topk = NULL;
    int size = top_current.size();
    for (int i = 0; i < size; i++)
    {
        bool is_top = true;
        int large_num = 0;
        for (int j = 0; j < p_set.size(); j++)
        {
            //if the points have the same coordinates, we do not need to use function check_situation
            bool is_same = true;
            for (int w = 0; w < top_current[i]->dim; w++)
            {
                if (top_current[i]->coord[w] != p_set[j]->coord[w])
                {
                    is_same = false;
                    break;
                }
            }

            if (!is_same)
            {
                hyperplane_t *h = alloc_hyperplane(top_current[i], p_set[j], 0);
                int relation = check_situation_positive(h, half_set);
                if (relation != 1)
                {
                    large_num++;
                    if (large_num >= k)
                    {
                        is_top = false;
                        break;
                    }
                }
            }
        }
        if (is_top)
        {
            return top_current[i];
        }
    }
    return point_topk;
}

/*
 * @brief Print the information of halfspaces and extreme points of R
 * @param R     The utility range R
 */
void print_halfspace_ext_R(halfspace_set_t *R)
{
    int dim = R->ext_pts[0]->dim;
    for (int i = 0; i < R->halfspaces.size(); i++)
    {
        printf("hyperplane ");
        for (int j = 0; j < dim; j++)
        {
            printf("%lf ", R->halfspaces[i]->normal->coord[j]);
        }
        printf("\n");
    }
    for (int i = 0; i < R->ext_pts.size(); i++)
    {
        printf("point ");
        for (int j = 0; j < dim; j++)
        {
            printf("%lf ", R->ext_pts[i]->coord[j]);
        }
        printf("\n");
    }
}
