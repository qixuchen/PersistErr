//#include "stdafx.h"

#include "data_utility.h"
#include "pruning.h"


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


/*
 *	Allocate memory for a point in dim-dimensional space with id
 */
point_t *alloc_point(const point_t * p)
{
    if(p == NULL) return NULL;
    point_t *point_v = alloc_point(p->dim);
    point_v->id = p->id;
    for(int i = 0; i < p->dim; i++){
        point_v->coord[i] = p->coord[i];
    }
    return point_v;
}


/**
 * @brief       Deep copy a point.
 *
 * @param p     The point to be copied from.
 * @return point_t*
 */
point_t *deepcopy_point(const point_t *p){
    point_t *point_v=NULL;
    if(p==NULL){
        return point_v;
    }
    point_v = alloc_point(p->dim);
    point_v->id = p->id;
    point_v->pid = p->pid;
    point_v->value = p->value;
    if(p->coord != NULL){
        for(int i=0; i< p->dim; i++){
            point_v->coord[i]=p->coord[i];
        }
    }
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

void print_point_set(std::vector<point_t*> p_set)
{
    int size = p_set.size(), dim = p_set[0]->dim;
    for(int i=0; i < size; ++i)
    {
        printf("%d  %d  ", p_set[i]->id, p_set[i]->dim);
        for(int j=0; j < dim; ++j)
        {
            printf("%lf  ", p_set[i]->coord[j]);
        }
        printf("%lf  \n", p_set[i]->value);
    }
}


/*
 *	Allocate memory for a hyperplane in dim-dimensional space
 */
hyperplane_t *alloc_hyperplane(point_t *normal, double offset)
{
    hyperplane_t *hyperplane_v;

    hyperplane_v = (hyperplane_t *) malloc(sizeof(hyperplane_t));
    memset(hyperplane_v, 0, sizeof(hyperplane_t));

    hyperplane_v->normal = alloc_point(normal->dim);

    for (int i = 0; i < normal->dim; i++)
    {
        hyperplane_v->normal->coord[i] = normal->coord[i];
    }
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


/**
 * @brief       deep copy the hyperplane (until point level (exclude point))
 *
 * @param h     the hyperplane to copy from
 * @return      hyperplane_t*
 */
hyperplane_t *deepcopy_hyperplane(const hyperplane_t *h){
    hyperplane_t *h_v =NULL;
    if(h==NULL){
        return h_v;
    }
    h_v = alloc_hyperplane(h->normal, h->offset);
    if(h->point1 != NULL){
        h_v->point1 = h->point1;
    }
    if(h->point2 != NULL){
        h_v->point2 = h->point2;
    }
    return h_v;
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
 */
halfspace_t *alloc_halfspace(int dim)
{
    halfspace_t *halfspace_v;
    halfspace_v = (halfspace_t *) malloc(sizeof(halfspace_t));
    memset(halfspace_v, 0, sizeof(halfspace_t));

    halfspace_v->normal = alloc_point(dim);
    for (int i = 0; i < dim; i++) halfspace_v->normal->coord[i] = 0;
    halfspace_v->direction = true;
    halfspace_v->offset = 0;
    return halfspace_v;
}


/*
 *	Allocate memory for a halfspace in dim-dimensional space
 */
halfspace_t *alloc_halfspace(point_t *normal, double offset)
{
    halfspace_t *halfspace_v;
    halfspace_v = (halfspace_t *) malloc(sizeof(halfspace_t));
    memset(halfspace_v, 0, sizeof(halfspace_t));

    halfspace_v->normal = alloc_point(normal->dim);
    for (int i = 0; i < normal->dim; i++) halfspace_v->normal->coord[i] = normal->coord[i];
    halfspace_v->offset = offset;
    return halfspace_v;
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


/**
 * @brief       deep copy the halfspace (until point level (exclude point))
 *
 * @param h     the halfspace to copy from
 * @return      halfspace_t*
 */
halfspace_t *deepcopy_halfspace(const halfspace_t *h){
    halfspace_t *h_v=NULL;
    if(h==NULL){
        return h_v;
    }
    h_v = alloc_halfspace(h->point1, h->point2, h->offset, h->direction);
    return h_v;
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
    cluster_v = NULL;
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


/**
 * @brief Initial utility range
 * @param hset  The halfspace
 * @return      The new copied halfspace
 */
halfspace_set_t* alloc_halfspace_set(halfspace_set_t *hset)
{
    int dim = hset->halfspaces[0]->normal->dim;
    halfspace_set_t *halfspace_set_v;
    halfspace_set_v = new halfspace_set_t;

    for(int i=0; i < hset->halfspaces.size();++i)
    {
        halfspace_t *h = alloc_halfspace(hset->halfspaces[i]->point1, hset->halfspaces[i]->point2, 0, true);
        halfspace_set_v->halfspaces.push_back(h);
    }
    halfspace_set_v->in_center = alloc_point(dim);
    halfspace_set_v->out_center = alloc_point(dim);
    halfspace_set_v->check_point = alloc_point(dim);

    //set extreme points
    get_extreme_pts_refine_halfspaces_alg1(halfspace_set_v);



    return halfspace_set_v;
}


halfspace_set_t* alloc_halfspace_set_normal_only(halfspace_set_t *hset)
{
    int dim = hset->halfspaces[0]->normal->dim;
    halfspace_set_t *halfspace_set_v;
    halfspace_set_v = new halfspace_set_t;

    for(int i=0; i < hset->halfspaces.size();++i)
    {
        halfspace_t *h = alloc_halfspace(hset->halfspaces[i]->normal, hset->halfspaces[i]->offset);
        halfspace_set_v->halfspaces.push_back(h);
    }
    halfspace_set_v->in_center = alloc_point(dim);
    halfspace_set_v->out_center = alloc_point(dim);
    halfspace_set_v->check_point = alloc_point(dim);

    //set extreme points
    get_extreme_pts_refine_halfspaces_alg1(halfspace_set_v);



    return halfspace_set_v;
}


/**
 * @brief       Deep copy the hyperplane set (until point level (exclude point))
 *
 * @param hss   the hyperplane set to copy from
 * @return      hyperplane_set_t*
 */
halfspace_set_t *deepcopy_halfspace_set(const halfspace_set_t * hss){ //LEAK
    halfspace_set_t * hss_v;
    if(hss==NULL){
        return hss_v;
    }
    hss_v = (halfspace_set_t *) malloc(sizeof(halfspace_set_t));
    memset(hss_v, 0, sizeof(halfspace_set_t));
    hss_v->in_center = deepcopy_point(hss->in_center);
    hss_v->out_center = deepcopy_point(hss->out_center);
    hss_v->check_point = deepcopy_point(hss->check_point);
    int size = hss->ext_pts.size();
    for (int i = 0; i < size; i++)
    {
        point * p = deepcopy_point(hss->ext_pts[i]);
        hss_v->ext_pts.push_back(p);
    }
    size = hss->halfspaces.size();
    for (int i = 0; i < size; i++)
    {
        halfspace_t * halfspace = deepcopy_halfspace(hss->halfspaces[i]);
        hss_v->halfspaces.push_back(halfspace);
    }
    size = hss->rec.size();
    for (int i = 0; i < size; i++)
    {
        hss_v->rec.push_back(hss->rec[i]);
    }
    size = hss->represent_point.size();
    for (int i = 0; i < size; i++)
    {
        hss_v->represent_point.push_back(hss->represent_point[i]);
    }
    return hss_v;
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
    //halfspace_set_v->halfspaces.shrink_to_fit();
    halfspace_set_v = NULL;
}


/**  @brief dynamically allocate space for choose_item 
 * 
 * 
 */
choose_item *alloc_choose_item(){
    choose_item *item_ptr = (choose_item *) malloc(sizeof(choose_item));;
    memset(item_ptr, 0, sizeof(choose_item));
    return item_ptr;
}


/**
 * @brief                Deep copy a choose_item object
 * 
 * @param it            The choose_item object to copy from
 * @return choose_item* 
 */
choose_item *deepcopy_choose_item(const choose_item *it){
    choose_item *it_v=NULL;
    if(it == NULL){
        return it_v;
    }
    it_v=(choose_item *) malloc(sizeof(choose_item));
    memset(it_v, 0, sizeof(choose_item));
    if(it->hyper != NULL){
        it_v->hyper=deepcopy_hyperplane(it->hyper);
    }

    int size = it->positive_side.size();
    for(int i=0; i<size; i++){
        it_v->positive_side.push_back(it->positive_side[i]);
    }

    size = it->negative_side.size();
    for(int i=0; i<size; i++){
        it_v->negative_side.push_back(it->negative_side[i]);
    }

    size = it->intersect_case.size();
    for(int i=0; i<size; i++){
        it_v->intersect_case.push_back(it->intersect_case[i]);
    }

    return it_v;

}


/**  @brief destory the dynamically allocated choose_item 
 * 
 * 
 */
void release_choose_item(choose_item *item_ptr){
    if (item_ptr == 0) return;

    if(item_ptr->hyper != 0){
        release_hyperplane(item_ptr->hyper);
    }
    free(item_ptr);
    item_ptr = NULL;
}


/**  @brief dynamically allocate space for item 
 */
item *alloc_item(){
    return new item;
}


/**  @brief destory the dynamically allocated choose_item 
 */
void release_item(item *item_ptr){
    if (item_ptr == 0) return;

    if(item_ptr->hyper != 0){
        release_hyperplane(item_ptr->hyper);
    }
    delete item_ptr;
    item_ptr = NULL;
}


/**  @brief dynamically allocate space for partition_fragement 
 */
frag_t *alloc_fragment(){
    return new frag_t;
}

/**  @brief destory the dynamically allocated partition_fragement
 */
void release_fragment(frag_t *frag_ptr){
    if (frag_ptr == 0) return;
    free(frag_ptr);
    frag_ptr = NULL;
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
