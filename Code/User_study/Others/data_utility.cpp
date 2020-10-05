//#include "stdafx.h"

#include "data_utility.h"


/*
 *	Allocate memory for a point in dim-dimensional space
 */
point_t *alloc_point(int dim)
{
    point_t *point_v;

    point_v = (point_t *) malloc(sizeof(point_t));
    memset(point_v, 0, sizeof(point_t));

    point_v->dim = dim;

    point_v->coord = (COORD_TYPE *) malloc(dim * sizeof(COORD_TYPE));
    memset(point_v->coord, 0, dim * sizeof(COORD_TYPE));

    point_v->id = -1;

    return point_v;
}

/*
 *	Allocate memory for a point in dim-dimensional space with id
 */
point_t *alloc_point(int dim, int id)
{
    point_t *point_v = alloc_point(dim);

    point_v->id = id;

    return point_v;
}


utility_t *alloc_utility()
{
    utility_t *utility_v;

    utility_v = (utility_t *) malloc(sizeof(utility_t));
    memset(utility_v, 0, sizeof(utility_t));

    return utility_v;
}


/*
 *	Generate a random point in dim-dimensional space
 */
point_t *rand_point(int dim)
{
    point_t *point_v = alloc_point(dim);

    for (int i = 0; i < dim; i++)
        point_v->coord[i] = rand() * 1.0 / RAND_MAX;

    //print_point(point_v);

    return point_v;
}

/*
 *	Release memory for a point
 */
void release_point(point_t *&point_v)
{
    if (point_v == NULL)
    {
        return;
    }

    if (point_v->coord != NULL)
    {
        free(point_v->coord);
        point_v->coord = NULL;
    }

    free(point_v);
    point_v = NULL;
}

/*
 *	Allocate memory for a hyperplane in dim-dimensional space
 */
hyperplane_t *alloc_hyperplane(point_t *normal, double offset)
{
    hyperplane_t *hyperplane_v;

    hyperplane_v = (hyperplane_t *) malloc(sizeof(hyperplane_t));
    memset(hyperplane_v, 0, sizeof(hyperplane_t));

    hyperplane_v->normal = normal;
    hyperplane_v->offset = offset;

    return hyperplane_v;
}

/*
 *	Allocate memory for a hyperplane in dim-dimensional space
 */
hyperplane_t *alloc_hyperplane(point_t *p_1, point_t *p_2, double offset)
{
    hyperplane_t *hyperplane_v;

    hyperplane_v = (hyperplane_t *) malloc(sizeof(hyperplane_t));
    memset(hyperplane_v, 0, sizeof(hyperplane_t));

    hyperplane_v->normal = alloc_point(p_1->dim);

    for (int i = 0; i < p_1->dim; i++)
    {
        hyperplane_v->normal->coord[i] = p_1->coord[i] - p_2->coord[i];
        //std::cout<<halfspace_v->normal->coord[i]<<" ";
    }

    hyperplane_v->point1 = p_1;
    hyperplane_v->point2 = p_2;
    hyperplane_v->offset = offset;

    return hyperplane_v;
}

/*
 *	Release memory for a point in dim-dimensional space
 */
void release_hyperplane(hyperplane_t *&hyperplane_v)
{
    if (hyperplane_v == NULL)
    {
        return;
    }
    if (hyperplane_v->normal != NULL)
    {
        release_point(hyperplane_v->normal);
    }
    free(hyperplane_v);
    hyperplane_v = NULL;
}


point_count_t *alloc_point_count()
{
    point_count_t *point_count_v;

    point_count_v = (point_count_t *) malloc(sizeof(point_count_t));
    memset(point_count_v, 0, sizeof(point_count_t));

    return point_count_v;
}

/*
 *	Allocate memory for a halfspace in dim-dimensional space
 	The coordinate of the halfspace is p1-p2
 */
halfspace_t *alloc_halfspace(point_t *p_1, point_t *p_2, double offset, bool direction)
{
    halfspace_t *halfspace_v;

    halfspace_v = (halfspace_t *) malloc(sizeof(halfspace_t));
    memset(halfspace_v, 0, sizeof(halfspace_t));

    halfspace_v->normal = alloc_point(p_1->dim);
    for (int i = 0; i < p_1->dim; i++)
    {
        halfspace_v->normal->coord[i] = p_1->coord[i] - p_2->coord[i];
        //std::cout<<halfspace_v->normal->coord[i]<<" ";
    }

    halfspace_v->point1 = p_1;
    halfspace_v->point2 = p_2;
    halfspace_v->direction = direction;
    halfspace_v->offset = offset;

    return halfspace_v;
}

/*
 *	Release memory for a halfspace in dim-dimensional space
 */
void release_halfspace(halfspace_t *&halfspace_v)
{
    if (halfspace_v == NULL)
    {
        return;
    }
    if (halfspace_v->normal != NULL)
    {
        release_point(halfspace_v->normal);
    }
    free(halfspace_v);
    halfspace_v = NULL;
}

s_node_t *alloc_s_node(int dim)
{
    s_node_t *s_node_v;

    s_node_v = (s_node_t *) malloc(sizeof(s_node_t));
    memset(s_node_v, 0, sizeof(s_node_t));
    s_node_v->center = alloc_point(dim);

    return s_node_v;
}

cluster_t *alloc_cluster()
{
    cluster_t *cluster_v;

    cluster_v = (cluster_t *) malloc(sizeof(cluster_t));
    memset(cluster_v, 0, sizeof(cluster_t));

    cluster_v->center = NULL;
    return cluster_v;
}

void release_cluster(cluster_t *cluster_v)
{
    if (cluster_v == NULL)
    {
        return;
    }
    if (cluster_v->center != NULL)
    {
        release_point(cluster_v->center);
    }
    cluster_v->h_set.clear();
    free(cluster_v);
}

/*
 *	Allocate memory for numberOfPoints number of points
 */
point_set_t *alloc_point_set(int numberOfPoints)
{
    point_set_t *point_set_v;

    point_set_v = (point_set_t *) malloc(sizeof(point_set_t));
    memset(point_set_v, 0, sizeof(point_set_t));

    point_set_v->numberOfPoints = numberOfPoints;

    point_set_v->points = (point_t **) malloc(numberOfPoints * sizeof(point_t *));
    memset(point_set_v->points, 0, numberOfPoints * sizeof(point_t *));

    return point_set_v;
}

/*
 *	Release memory for numberOfPoints number of points
 */
void release_point_set(point_set_t *&point_set_v, bool clear)
{
    if (point_set_v == NULL)
    {
        return;
    }

    if (point_set_v->points != NULL)
    {
        if (clear)
        {
            for (int i = 0; i < point_set_v->numberOfPoints; i++)
                release_point(point_set_v->points[i]);
        }

        free(point_set_v->points);
        point_set_v->points = NULL;
    }

    free(point_set_v);
    point_set_v = NULL;
}


/*
 *	Allocate memory for halfspace_set
 */
halfspace_set_t *alloc_halfspace_set(int dim)
{
    halfspace_set_t *halfspace_set_v;

    halfspace_set_v = (halfspace_set_t *) malloc(sizeof(halfspace_set_t));
    memset(halfspace_set_v, 0, sizeof(halfspace_set_t));

    halfspace_set_v->in_center = alloc_point(dim);
    halfspace_set_v->out_center = alloc_point(dim);
    halfspace_set_v->check_point = alloc_point(dim);

    return halfspace_set_v;
}


/*
 *	Release memory for halfspace_set
 */
void release_halfspace_set(halfspace_set_t *&halfspace_set_v)
{
    if (halfspace_set_v == NULL)
    {
        return;
    }

    if (halfspace_set_v->in_center != NULL)
    {
        release_point(halfspace_set_v->in_center);
    }
    if (halfspace_set_v->out_center != NULL)
    {
        release_point(halfspace_set_v->out_center);
    }
    if (halfspace_set_v->check_point != NULL)
    {
        release_point(halfspace_set_v->check_point);
    }
    int size = halfspace_set_v->ext_pts.size();
    for (int i = 0; i < size; i++)
    {
        release_point(halfspace_set_v->ext_pts[i]);
    }
    size = halfspace_set_v->halfspaces.size();
    for (int i = 0; i < size; i++)
    {
        release_halfspace(halfspace_set_v->halfspaces[i]);
    }
    halfspace_set_v->halfspaces.clear();
    halfspace_set_v->halfspaces.shrink_to_fit();
    free(halfspace_set_v);
    halfspace_set_v = NULL;
}


/*
*	For degug purpose, print the coordinates for a given point
*/
void print_point(point_t *point_v)
{
    //printf("printing point:\n");
    //printf("%d ", point_v->id);
    for (int i = 0; i < point_v->dim; i++)
        printf("%f ", point_v->coord[i]);
    printf("\n");
}

/*
 *	For debug purose, print the contents of a given point set
 */
void print_point_set(point_set_t *point_set_v)
{
    for (int i = 0; i < point_set_v->numberOfPoints; i++)
        //printf("%i ", point_set_v->points[i]->id);
        print_point(point_set_v->points[i]);

    printf("\n");
}
