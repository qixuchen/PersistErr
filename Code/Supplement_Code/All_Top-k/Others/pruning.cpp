#include "pruning.h"

char hidden_options[] = " d n v Qbb QbB Qf Qg Qm Qr QR Qv Qx Qz TR E V Fa FA FC FD FS Ft FV Gt Q0 Q1 Q2 Q3 Q4 Q5 Q6 Q7 Q8 Q9 ";

#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
#endif

//#include "data_utility.h"

#include "../Qhull/mem.h"
#include "../Qhull/qset.h"
#include "../Qhull/libqhull.h"
#include "../Qhull/qhull_a.h"
#include "../Qhull/io.h"

#include <ctype.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#if __MWERKS__ && __POWERPC__
#include <SIOUX.h>
#include <Files.h>
#include <console.h>
#include <Desk.h>

#elif __cplusplus
extern "C" {
int isatty(int);
}

#elif _MSC_VER
#include <io.h>
#define isatty _isatty
int _isatty(int);

#else
int isatty(int);  /* returns 1 if stdin is a tty
                   if "Undefined symbol" this can be deleted along with call in main() */
#endif

#ifdef WIN32
#ifdef __cplusplus
}
#endif
#endif

/**
 * @brief Conduct halfspace intersection by invoking Qhull
 * @param rPtr   Contains the data of all the halfspace
 * @param wPtr   The extreme points of the intersection and the necessary halfspaces are shown in wPtr
 */
int halfspace(FILE *rPtr, FILE *wPtr)
{
    int curlong, totlong; /* used !qh_NOmem */
    int exitcode, numpoints, dim;
    coordT *points;
    boolT ismalloc;

    // the required parameters
    int argc = 3;
    char *argv[3];
    argv[0] = (char*)"qhalf";
    argv[1] = (char*)"Fp";
    argv[2] = (char*)"Fx";

    qh_init_A(rPtr, wPtr, stderr, argc, argv);  /* sets qh qhull_command */
    exitcode = setjmp(qh errexit); /* simple statement for CRAY J916 */
    if (!exitcode)
    {
        qh NOerrexit = False;
        qh_option("Halfspace", NULL, NULL);
        qh HALFspace = True;    /* 'H'   */
        qh_checkflags(qh qhull_command, hidden_options);
        qh_initflags(qh qhull_command);

        points = qh_readpoints(&numpoints, &dim, &ismalloc);

        if (dim >= 5)
        {
            qh_option("Qxact_merge", NULL, NULL);
            qh MERGEexact = True; /* 'Qx' always */
        }
        qh_init_B(points, numpoints, dim, ismalloc);
        qh_qhull();
        qh_check_output();
        qh_produce_output();
        //fclose(wPtr);
        //print_summary();

        if (qh VERIFYoutput && !qh FORCEoutput && !qh STOPpoint && !qh STOPcone)
        {
            qh_check_points();
        }
        exitcode = qh_ERRnone;
    }
    qh NOerrexit = True;  /* no more setjmp */
#ifdef qh_NOmem
    qh_freeqhull(qh_ALL);
#else
    qh_freeqhull(!qh_ALL);
    qh_memfreeshort(&curlong, &totlong);
    if (curlong || totlong)
    {
        fprintf(stderr, "qhull internal warning (main): did not free %d bytes of long memory(%d pieces)\n",
                totlong, curlong);
    }
#endif
    return exitcode;
}

/**
 * @brief Get the set of extreme points of R (bounded by the extreme vectors)
 * @param ext_vec    The hyperplanes(in the form of point) bounding the utility range R
 * @return  All the extreme points of R
 */
vector<point_t *> get_extreme_pts(vector<point_t *> &ext_vec)
{
    int dim = ext_vec[0]->dim;
    char file1[MAX_FILENAME_LENG];
    sprintf(file1, "../output/hyperplane_data.txt");
    char file2[MAX_FILENAME_LENG];
    sprintf(file2, "../output/ext_pt.txt");

    // construct the hyperplanes and a feasible point
    vector<hyperplane_t *> utility_hyperplane;
    point_t *normal;
    normal = alloc_point(dim);
    for (int i = 0; i < dim; i++)
        normal->coord[i] = 1;
    utility_hyperplane.push_back(alloc_hyperplane(normal, -1));
    for (int i = 0; i < ext_vec.size(); i++)
    {
        normal = copy(ext_vec[i]);
        utility_hyperplane.push_back(alloc_hyperplane(normal, 0));
    }
    point_t *feasible_pt = find_feasible(utility_hyperplane);

    // prepare the file for computing the convex hull (the candidate utility range R) via half space interaction
    write_hyperplanes(utility_hyperplane, feasible_pt, file1);
    for (int i = 0; i < utility_hyperplane.size(); i++)
        release_hyperplane(utility_hyperplane[i]);

    // write hyperplanes and the feasible point to file1, conduct half space intersection and write reulsts to file2
    FILE *rPtr;
    FILE *wPtr;
    if ((rPtr = fopen(file1, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open the data file.\n");
        exit(0);
    }
    wPtr = (FILE *) fopen(file2, "w");
    halfspace(rPtr, wPtr);
    fclose(rPtr);
    fclose(wPtr);

    //read extreme points in file2
    if ((rPtr = fopen(file2, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open the data file %s.\n", file2);
        exit(0);
    }
    int size;
    vector<point_t *> ext_pts;
    fscanf(rPtr, "%i%i", &dim, &size);
    for (int i = 0; i < size; i++)
    {
        bool allZero = true;
        point_t *p = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            fscanf(rPtr, "%lf", &p->coord[j]);
            if (!isZero(p->coord[j]))
            {
                allZero = false;
            }
        }
        if (allZero)
        {
            release_point(p);
        }
        else
        {
            ext_pts.push_back(p);
            //print_point(p);
        }
    }

    // update the set of extreme vectors
    vector<point_t *> new_ext_vec;
    fscanf(rPtr, "%i", &size);
    for (int i = 0; i < size; i++)
    {
        int idx;
        fscanf(rPtr, "%i", &idx);

        if (idx > 0)
        {
            new_ext_vec.push_back(copy(ext_vec[idx - 1]));
        }
    }
    for (int i = 0; i < ext_vec.size(); i++)
    {
        release_point(ext_vec[i]);
    }
    ext_vec = new_ext_vec;

    fclose(rPtr);
    return ext_pts;
}

/**
 * @brief Based on the extreme points of the halfspace set, update the information of the bounding sphere
 * @param half_set       The halfspace set
 */
void update_bounding_sphere(halfspace_set_t *half_set)
{
    int dim = half_set->ext_pts[0]->dim;
    double size = half_set->ext_pts.size();
    double *coord = half_set->in_center->coord;
    //set all coordinates as 0
    for (int i = 0; i < dim; i++)
    {
        coord[i] = 0;
    }
    //set the in_center
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            coord[j] += half_set->ext_pts[i]->coord[j];
        }
    }
    for (int i = 0; i < dim; i++)
    {
        coord[i] = coord[i] / size;
    }

    //find the radius R
    half_set->out_radius = -1;
    for (int i = 0; i < size; i++)
    {
        double dis = 0;
        for (int j = 0; j < dim; j++)
        {
            dis += (coord[j] - half_set->ext_pts[i]->coord[j]) * (coord[j] - half_set->ext_pts[i]->coord[j]);
        }
        if (half_set->out_radius < dis)
        {
            half_set->out_radius = dis;
        }
    }
    half_set->out_radius = sqrt(half_set->out_radius);
}

/**
 * @brief Set the value of the extreme points of the rectangle
 * @param place      Set high/low value for the points
 * @param high       The largest value for each dimension
 * @param low        The smallest value for each dimension
 * @param half_set   The halfspace set
 * @param dim        The number of dimensions
 * @param layer      The number of dimensions we have set for the points
 */
void build_rec_ext(bool *place, double *high, double *low, halfspace_set_t *half_set, int dim, int layer)
{
    if (layer + 1 == dim)
    {
        point_t *p1 = alloc_point(dim);
        point_t *p2 = alloc_point(dim);
        for (int i = 0; i < dim - 1; i++)
        {
            if (place[i] == true)
            {
                p1->coord[i] = high[i];
                p2->coord[i] = high[i];
            }
            else
            {
                p1->coord[i] = low[i];
                p2->coord[i] = low[i];
            }
        }
        p1->coord[dim - 1] = high[dim - 1];
        p2->coord[dim - 1] = low[dim - 1];
        half_set->rec.push_back(p1);
        half_set->rec.push_back(p2);
    }
    else
    {
        place[layer] = true;
        build_rec_ext(place, high, low, half_set, dim, layer + 1);
        place[layer] = false;
        build_rec_ext(place, high, low, half_set, dim, layer + 1);
    }
}

/**
 * @brief Based on the extreme points of the halfspace set, update the information of the bounding rectangle
 * @param half_set       The halfspace set
 */
void update_bounding_rectangle(halfspace_set_t *half_set)
{
    int dim = half_set->ext_pts[0]->dim;
    double *high = new double[dim];
    double *low = new double[dim];
    for (int j = 0; j < dim; j++)
    {
        high[j] = half_set->ext_pts[0]->coord[j];
        low[j] = half_set->ext_pts[0]->coord[j];
    }
    for (int i = 1; i < half_set->ext_pts.size(); i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (high[j] < half_set->ext_pts[i]->coord[j])
            {
                high[j] = half_set->ext_pts[i]->coord[j];
            }
            if (low[j] > half_set->ext_pts[i]->coord[j])
            {
                low[j] = half_set->ext_pts[i]->coord[j];
            }
        }
    }
    bool *place = new bool[dim];
    build_rec_ext(place, high, low, half_set, dim, 0);
}

/**
 * @brief Used to check whether the hyperplane is on one side of the bounding sphere of the halfspace set
 *        Not the relation between the hyperplanes and the halfspace set
 * @param hyper     The hyperplane
 * @param half_set  The halfspace set
 * @return The situation of the hyperplane and the bounding shpere
 *      -1 The hyperplane on the positive side of the bounding shpere
 *      -0 The hyperplane intersects with the halfspace set
 *     --1 The hyperplane on the negative side of the bounding sphere
 *     --2 The hyperplane intersects with the bounding sphere, not sure with the halfspace set
 */
int check_situation_bounding_sphere(hyperplane_t *hyper, halfspace_set_t *half_set)
{
    double sum = 0, value = 0, distance;
    int dim = hyper->normal->dim;
    for (int i = 0; i < dim; i++)
    {
        value += hyper->normal->coord[i] * half_set->in_center->coord[i];
        sum += hyper->normal->coord[i] * hyper->normal->coord[i];
    }
    sum = sqrt(sum);
    if (value >= 0)
    {
        distance = value / sum;
        if (distance >= half_set->out_radius)
        {
            return 1;
        }
    }
    else
    {
        distance = -value / sum;
        if (distance >= half_set->out_radius)
        {
            return -1;
        }
    }
    return -2;
}

/**
 * @brief Used to check whether the hyperplane is on one side of the bounding rectangle of the halfspace set
 *        Not the relation between the hyperplanes and the halfspace set
 * @param hyper     The hyperplane
 * @param half_set  The halfspace set
 * @return The situation of the hyperplane and the bounding rectangle
 *      -1 The hyperplane on the positive side of the bounding rectangle
 *      -0 The hyperplane intersects with the bounding rectangle
 *     --1 The hyperplane on the negative side of the bounding rectangle
 */
int check_situation_bounding_rec(hyperplane_t *hyper, halfspace_set_t *half_set)
{
    int M = half_set->rec.size();
    int dim = half_set->rec[0]->dim;
    int posi = 0, nega = 0;
    for (int i = 0; i < M; i++)
    {
        double sum = 0;
        for (int j = 0; j < dim; j++)
        {
            sum += (hyper->normal->coord[j] * half_set->rec[i]->coord[j]);
        }
        if (sum > 0)
        {
            posi++;
        }
        else if (sum < 0)
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
    else
    {
        return -1;
    }
}

/**
 * @brief Check whether the intersection of the halfspaces exists
 *        Set the extreme points of the halfspace set. Set the average point of ext_pts as a ext_pt
 *        Refine the bounded hyperplanes
 *        Set the bounding sphere
 * @param half_set The halfspace set. All the halfspaces are in the form of <=0
 * @return  Whether the intersection of the halfspaces exists
 *   -true  The intersection exist
 *   -false The intersection does not exist
 */
bool get_extreme_pts_refine_halfspaces_alg1(halfspace_set_t *half_set)
{
    int M = half_set->halfspaces.size();
    if (M < 1)
    {
        printf("%s\n", "Error: No halfspace in the set.");
        return false;
    }
    int dim = half_set->halfspaces[0]->normal->dim;
    char file1[MAX_FILENAME_LENG];
    sprintf(file1, "../output/hyperplane_data.txt");
    char file2[MAX_FILENAME_LENG];
    sprintf(file2, "../output/ext_pt.txt");

    // construct the hyperplanes and a feasible point
    vector<hyperplane_t *> utility_hyperplane;
    point_t *normal;
    normal = alloc_point(dim);
    for (int i = 0; i < dim; i++)
        normal->coord[i] = 1;
    utility_hyperplane.push_back(alloc_hyperplane(normal, -10000));
    //Note all the inequality are <=

    for (int i = 0; i < M; i++)
    {
        normal = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            normal->coord[j] = half_set->halfspaces[i]->normal->coord[j];
        }
        utility_hyperplane.push_back(alloc_hyperplane(normal, 0));
    }
    point_t *feasible_pt = find_feasible(utility_hyperplane);

    if (feasible_pt == NULL)
    {
        for (int i = 0; i < utility_hyperplane.size(); i++)
            release_hyperplane(utility_hyperplane[i]);
        return false;
    }

    // prepare the file for computing the convex hull (the candidate utility range R) via half space interaction
    write_hyperplanes(utility_hyperplane, feasible_pt, file1);
    for (int i = 0; i < utility_hyperplane.size(); i++)
        release_hyperplane(utility_hyperplane[i]);

    // write hyperplanes and the feasible point to file1, conduct half space intersection and write results to file2
    FILE *rPtr;
    FILE *wPtr;
    if ((rPtr = fopen(file1, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open the data file.\n");
        exit(0);
    }
    wPtr = (FILE *) fopen(file2, "w");
    halfspace(rPtr, wPtr);
    fclose(rPtr);
    fclose(wPtr);

    //read extreme points in file2
    if ((rPtr = fopen(file2, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open the data file %s.\n", file2);
        exit(0);
    }
    int size;
    fscanf(rPtr, "%i%i", &dim, &size);

    point_t *pt;
    while (half_set->ext_pts.size() > 0)
    {
        pt = half_set->ext_pts[half_set->ext_pts.size()-1];
        half_set->ext_pts.pop_back();
        release_point(pt);
    }
    half_set->ext_pts.clear();
    point_t *p_c = alloc_point(dim);
    for (int i = 0; i < dim; i++)
        p_c->coord[i] = 0;

    for (int i = 0; i < size; i++)
    {
        bool allZero = true;
        point_t *p = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            fscanf(rPtr, "%lf", &p->coord[j]);
            p->coord[j] = p->coord[j] / 10000;
            p_c->coord[j] += p->coord[j];
            if (!isZero(p->coord[j]))
            {
                allZero = false;
            }
        }
        if (allZero)
        {
            release_point(p);
        }
        else
        {
            half_set->ext_pts.push_back(p);
        }
    }
    for (int i = 0; i < dim; i++)
        p_c->coord[i] = p_c->coord[i] / half_set->ext_pts.size();
    half_set->ext_pts.push_back(p_c);

    //update the bounding ball
    update_bounding_sphere(half_set);

    //update the bounding rectangle
    //update_bounding_rectangle(half_set);
    // update the set of halfspaces
    fscanf(rPtr, "%i", &size);

    for (int i = 0; i < half_set->halfspaces.size(); i++)
    {
        half_set->halfspaces[i]->offset = 0;
    }

    for (int i = 0; i < size; i++)
    {
        int idx;
        fscanf(rPtr, "%i", &idx);
        if (idx > 0)
        {
            half_set->halfspaces[idx - 1]->offset = 5;
        }
    }

    int count_num = 0;

    for (int i = 0; i < M; i++)
    {
        if (half_set->halfspaces[count_num]->offset == 0)
        {
            half_set->halfspaces.erase(half_set->halfspaces.begin() + count_num);
        }
        else
        {
            count_num++;
        }
    }

    fclose(rPtr);
    return true;
}

/**
 * @brief Check whether the intersection of the halfspaces exists
 *        Set the extreme points of the halfspace
 *        Refine the bounded hyperplanes
 * @param half_set The halfspace set. All the halfspaces are in the form of >=0
 * @return  Whether the intersection of the halfspaces exists
 *   -true  The intersection exist
 *   -false The intersection does not exist
 */
bool get_extreme_pts_refine_from_halfspaces(halfspace_set_t *half_set)
{
    int M = half_set->halfspaces.size();
    if (M < 1)
    {
        printf("%s\n", "Error: No halfspace in the set.");
        return false;
    }
    int dim = half_set->halfspaces[0]->normal->dim;
    char file1[MAX_FILENAME_LENG];
    sprintf(file1, "../output/hyperplane_data.txt");
    char file2[MAX_FILENAME_LENG];
    sprintf(file2, "../output/ext_pt.txt");

    // construct the hyperplanes and a feasible point
    vector<hyperplane_t *> utility_hyperplane;
    point_t *normal;
    normal = alloc_point(dim);
    for (int i = 0; i < dim; i++)
        normal->coord[i] = 1;
    utility_hyperplane.push_back(alloc_hyperplane(normal, -10000));

    for (int i = 0; i < M; i++)
    {
        normal = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            normal->coord[j] = half_set->halfspaces[i]->normal->coord[j];
        }
        utility_hyperplane.push_back(alloc_hyperplane(normal, 0));
    }
    point_t *feasible_pt = find_feasible(utility_hyperplane);
    if (feasible_pt == NULL)
    {
        for (int i = 0; i < utility_hyperplane.size(); i++)
            release_hyperplane(utility_hyperplane[i]);
        return false;
    }

    // prepare the file for computing the convex hull (the candidate utility range R) via half space interaction
    write_hyperplanes(utility_hyperplane, feasible_pt, file1);
    for (int i = 0; i < utility_hyperplane.size(); i++)
        release_hyperplane(utility_hyperplane[i]);

    // write hyperplanes and the feasible point to file1, conduct half space intersection and write reulsts to file2
    FILE *rPtr;
    FILE *wPtr;
    if ((rPtr = fopen(file1, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open the data file.\n");
        exit(0);
    }
    wPtr = (FILE *) fopen(file2, "w");
    halfspace(rPtr, wPtr);
    fclose(rPtr);
    fclose(wPtr);

    //read extreme points in file2
    if ((rPtr = fopen(file2, "r")) == NULL)
    {
        fprintf(stderr, "Cannot open the data file %s.\n", file2);
        exit(0);
    }
    int size;
    fscanf(rPtr, "%i%i", &dim, &size);
    if (half_set->ext_pts.size() > 0)
    {
        half_set->ext_pts.clear();
    }
    for (int i = 0; i < size; i++)
    {
        bool allZero = true;
        point_t *p = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            fscanf(rPtr, "%lf", &p->coord[j]);
            p->coord[j] = p->coord[j] / 10000;
            if (!isZero(p->coord[j]))
            {
                allZero = false;
            }
        }
        if (allZero)
        {
            release_point(p);
        }
        else
        {
            half_set->ext_pts.push_back(p);
        }
    }

    //update the bounding sphere
    update_bounding_sphere(half_set);
    //update the bounding rec
    //update_bounding_rectangle(half_set);

    // update the set of halfspaces
    fscanf(rPtr, "%i", &size);
    for (int i = 0; i < half_set->halfspaces.size(); i++)
    {
        half_set->halfspaces[i]->offset = 0;
    }
    for (int i = 0; i < size; i++)
    {
        int idx;
        fscanf(rPtr, "%i", &idx);
        if (idx > 0)
        {
            half_set->halfspaces[idx - 1]->offset = 5;
        }
    }

    int count_num = 0;
    for (int i = 0; i < M; i++)
    {
        if (half_set->halfspaces[count_num]->offset == 0)
        {
            half_set->halfspaces.erase(half_set->halfspaces.begin() + count_num);
        }
        else
        {
            count_num++;
        }
    }
    fclose(rPtr);
    return true;
}

void print_summary(void)
{
    facetT *facet;
    int k;

    printf("\n%d vertices and %d facets with normals:\n",
           qh num_vertices, qh num_facets);

    FORALLfacets
    {
        for (k = 0; k < qh hull_dim; k++)
            printf("%lf\t", facet->normal[k]);

        printf("%lf\t%d\n", facet->offset, facet->id);
    }

}


// get bounding hyperplanes of the conical hull (used in the conical hull pruning)
void get_hyperplanes(vector<point_t *> &ext_vec, hyperplane_t *&hp, vector<point_t *> &hyperplanes)
{
    //constuct non-trivial extreme vectors
    vector<int> frame;
    frameConeLP(ext_vec, frame);
    vector<point_t *> new_ext_vec;
    for (int i = 0; i < frame.size(); i++)
        new_ext_vec.push_back(copy(ext_vec[frame[i]]));
    for (int i = 0; i < ext_vec.size(); i++)
        release_point(ext_vec[i]);
    ext_vec = new_ext_vec;

    int dim = ext_vec[0]->dim;

    // used in the necessary condiditon of conical hull pruning
    double offset = 0;
    point_t *normal = alloc_point(dim);
    for (int i = 0; i < dim; i++)
        normal->coord[i] = 0;


    if (1)
    {
        for (int i = 0; i < ext_vec.size(); i++)
        {
            point_t *minus = scale(-1, ext_vec[i]);
            double len;
            point_t *pi = alloc_point(dim);

            solveLP(ext_vec, minus, len, pi);

            point_t *new_normal = add(normal, pi);

            offset = dot_prod(normal, ext_vec[0]);
            for (int i = 1; i < ext_vec.size(); i++)
            {
                double temp = dot_prod(normal, ext_vec[i]);
                if (temp > offset)
                {
                    offset = temp;
                }
            }

            release_point(pi);
            release_point(minus);
            release_point(normal);

            normal = new_normal;

            if (offset < 0 && !isZero(offset))
            {
                break;
            }
        }

        double length = calc_len(normal);
        for (int i = 0; i < dim; i++)
            normal->coord[i] /= -length;
        offset = dot_prod(normal, ext_vec[0]);
        for (int i = 1; i < ext_vec.size(); i++)
        {
            double temp = dot_prod(normal, ext_vec[i]);
            if (temp < offset)
            {
                offset = temp;
            }
        }
    }

    // the hyperplane for the necessary condiditon of conical hull pruning
    hp = alloc_hyperplane(normal, offset);


    // invoke Qhull for computing the conical hull
    int n = ext_vec.size() + 1;
    int curlong, totlong; /* used !qh_NOmem */
    int exitcode;
    boolT ismalloc = True;

    coordT *points;
    //temp_points = new coordT[(orthNum * S->numberOfPoints + 1)*(dim)];
    points = qh temp_malloc = (coordT *) qh_malloc(n * (dim) * sizeof(coordT));

    for (int i = 0; i < ext_vec.size(); i++)
    {
        //double len = compute_intersection_len(hp, ext_vec[i]);
        //printf("%lf %lf\n", len, calc_len(ext_vec[i]));
        //point_t* tmp = scale( len, ext_vec[i]);
        //for (int j = 0; j < dim; j++)
        //	points[i*dim + j] = tmp->coord[j];
        for (int j = 0; j < dim; j++)
            points[i * dim + j] = ext_vec[i]->coord[j];
    }

    for (int i = 0; i < dim; i++)
    {
        points[ext_vec.size() * dim + i] = 0;
    }

    //printf("# of points: %d\n", count);
    qh_init_A(stdin, stdout, stderr, 0, NULL);  /* sets qh qhull_command */
    exitcode = setjmp(qh errexit); /* simple statement for CRAY J916 */

    double minCR;

    if (!exitcode)
    {

        //qh POSTmerge = True;
        ////qh postmerge_centrum = 0.01;
        //qh premerge_cos = 0.995;

        qh_initflags(qh qhull_command);
        qh_init_B(points, n, dim, ismalloc);
        qh_qhull();
        qh_check_output();


        if (qh VERIFYoutput && !qh FORCEoutput && !qh STOPpoint && !qh STOPcone)
        {
            qh_check_points();
        }
        exitcode = qh_ERRnone;

        //qh_vertexneighbors();
        //print_summary();


        //vertexT *vertex;
        //FORALLvertices
        //{
        //	bool isPt = true;
        //	for (int i = 0; i < dim; i++)
        //	{
        //		if (!isZero(vertex->point[i]))
        //		{
        //			isPt = false;
        //			break;
        //		}
        //	}

        //	if (isPt)
        //	{
        //		facetT *facet, **facetp;
        //		FOREACHfacet_(vertex->neighbors)
        //		{
        //			point_t* normal = alloc_point(dim);
        //			for (int j = 0; j < dim; j++)
        //				normal->coord[j] = facet->normal[j];
        //			hyperplanes.push_back(normal);
        //		}
        //		break;
        //	}
        //}

        // the bounding hyperplaines of the conical hull
        facetT *facet;
        FORALLfacets
        {

            if (isZero(facet->offset))
            {
                point_t *normal = alloc_point(dim);
                for (int j = 0; j < dim; j++)
                    normal->coord[j] = facet->normal[j];
                hyperplanes.push_back(normal);
            }

        }

    }

    qh NOerrexit = True;  /* no more setjmp */
#ifdef qh_NOmem
    qh_freeqhull(True);
#else
    qh_freeqhull(False);
    qh_memfreeshort(&curlong, &totlong);
    if (curlong || totlong)
    {
        fprintf(stderr, "qhull internal warning (main): did not free %d bytes of long memory(%d pieces)\n",
                totlong, curlong);
    }
#endif

}

// hyperplane pruning
int hyperplane_dom(point_t *p_i, point_t *p_j, vector<point_t *> ext_pts)
{
    int dim = p_i->dim;

    point_t *normal = sub(p_i, p_j);

    int below_count = 0;

    // to perform hyperplane pruning, check each extreme points of R
    for (int i = 0; i < ext_pts.size(); i++)
    {
        point_t *ext_pt = ext_pts[i];
        double v = dot_prod(normal, ext_pt);

        if (v < 0 && !isZero(v))
        {
            below_count++;
            break;
        }
    }
    release_point(normal);

    if (below_count == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// conical hull pruning
int
conical_hull_dom(point_t *p_i, point_t *p_j, hyperplane_t *hp, vector<point_t *> hyperplanes, vector<point_t *> ext_vec)
{
    int dim = p_i->dim;
    int dominate;

    // check the necessary condition
    point_t *minus = sub(p_j, p_i);
    double len = compute_intersection_len(hp, minus);
    if ((len < 1 && len > 0) || isZero(len - 1) || isZero(len))
    {
        bool all_below = true;
        for (int i = 0; i < hyperplanes.size(); i++)
        {
            point_t *normal = hyperplanes[i];
            double v = dot_prod(normal, minus);

            if (v > 0 && !isZero(v))
            {
                all_below = false;
                break;
            }
        }
        // check if below all bounding hyperplanes of the conical hull
        if (all_below)
        {
            dominate = 1;
        }
        else
        {
            dominate = 0;
        }
    }
    else
    {
        dominate = 0;
    }

    release_point(minus);

    return dominate;
}

// check whether p_i has a higher uitlity than p_j based on either Hyperplane Prunning or Conical Hull Pruninig (defined by dom_option)
int dom(point_t *p_i, point_t *p_j, vector<point_t *> ext_pts, hyperplane_t *hp, vector<point_t *> hyperplanes,
        vector<point_t *> ext_vec, int dom_option)
{
    if (dom_option == HYPER_PLANE)
    { // hyperplane pruning
        return hyperplane_dom(p_i, p_j, ext_pts);
    }
    else
    { // conical hull pruning
        return conical_hull_dom(p_i, p_j, hp, hyperplanes, ext_vec);
    }
}

// get an approximate upper bound bound in O(|ext_pts|) time based on the MBR of R
double get_rrbound_approx(vector<point_t *> ext_pts)
{
    if (ext_pts.size() == 0)
    {
        return 1;
    }

    int dim = ext_pts[0]->dim;

    // compute the Minimum Bounding Rectangle (MBR)
    double *max = new double[dim];
    double *min = new double[dim];
    for (int i = 0; i < dim; i++)
    {
        max[i] = ext_pts[0]->coord[i];
        min[i] = ext_pts[0]->coord[i];
    }

    for (int i = 1; i < ext_pts.size(); i++)
    {
        point_t *pt = ext_pts[i];

        for (int j = 0; j < dim; j++)
        {
            if (pt->coord[j] > max[j])
            {
                max[j] = pt->coord[j];
            }
            else if (pt->coord[j] < min[j])
            {
                min[j] = pt->coord[j];
            }
        }
    }

    double bound = 0;

    for (int i = 0; i < dim; i++)
        bound += max[i] - min[i];

    bound *= ext_pts[0]->dim;

    delete[] max;
    delete[] min;

    return bound < 1 ? bound : 1;
}

/**
 * @brief Get an "exact" upper bound bound in O(|ext_pts|^2) time based on R
 * @param ext_pts       All the extreme points of R
 * @return The regret ratio
 */
double get_rrbound_exact(vector<point_t *> ext_pts)
{
    if (ext_pts.size() == 0)
    {
        return 1;
    }

    double max = 0;
    // find the maximum pairwise L1-distance between the extreme vertices of R
    for (int i = 0; i < ext_pts.size(); i++)
    {
        for (int j = i + 1; j < ext_pts.size(); j++)
        {
            double v = calc_l1_dist(ext_pts[i], ext_pts[j]);
            if (v > max)
            {
                max = v;
            }
        }
    }

    max *= ext_pts[0]->dim;
    return max < 1 ? max : 1;
}
