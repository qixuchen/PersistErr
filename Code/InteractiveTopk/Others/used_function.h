#define Beta 0.01
#define Precision 0.0000015
#define RandRate 0.5

#include "../Others/data_utility.h"
#include "../Others/data_struct.h"
#include "read_write.h"
#include "../Others/pruning.h"
#include "qhull_build.h"
#include "../lp.h"
#include <vector>
#include <ctime>
#include <math.h>

bool is_same_point(point_t* p1, point_t* p2);

//@brief Reload the points to the data structure vector
// 		 Does not change the original dataset
//@param original_set	The original dataset
//@param p_set 			The returned dataset where the points are inserted randomly
void point_reload(point_set_t* original_set, std::vector<point_t*> &p_set);

/**
 * @brief Reload the points to the data structure point_set
 *        Does not change the original dataset
 * @param original_set	The original dataset
 * @param p_set 		The returned dataset
 */
point_set_t* point_reload(std::vector<point_t *> p_set);

//@brief Reload the points to the data structure vector Randomly
//		 Define "RandRate" to control how many point reinserted
// 		 Does not change the original dataset
//@param original_set	The original dataset
//@param p_set 			The returned dataset where the points are inserted randomly
void point_reload_with_random(point_set_t* original_set, std::vector<point_t*> &p_set);

void point_random(std::vector<point_t *> &p_set);

void hyperplane_random(std::vector<hyperplane_t *> &h_set);


bool exist_same_hyperplane(std::vector<hyperplane_t*> h_set, hyperplane* h);


//@brief Reload the points to the data structure vector Randomly 
//		 Delete the overlapped points, remain only one
//		 Define "RandRate" to control how many point reinserted
// 		 Does not change the original dataset
//@param original_set	The original dataset
//@param p_set 			The returned dataset where the points are inserted randomly
void point_reload_with_random_delete_same(point_set_t* original_set, std::vector<point_t*> &p_set);


point_set_t * point_reload_delete_same(std::vector<point_t *> p_set);

void delete_same(std::vector<point_t *> p_set, std::vector<point_t *> &p0);

//@brief Sort points based on their utility w.r.t. linear function u
// 		 Does not change the original dataset
//@param p_set 			The original dataset
//@param return_point	The returned dataset containing all the sorted points
//@param u 				The linear function
void sort_point(std::vector<point_t*> p_set, std::vector<point_t*> &return_point, point_t* u);

//@brief Calculate the distance between a point and a hyperplane
//@param hyper 	The hyperplane
//@param p 		The point
//@return The distance(Due to the property of C++, the distance can not be accurate enough)
double calculate_distance(hyperplane_t* hyper, point_t* p);

//@brief Find the top-k point w.r.t linear function u
//@param u 			The linear function
//@param p_set 		The original dataset
//@param top 		The returned top-k point
//@param k 			The threhold of top-k
void find_top_k(point_t* u, point_set_t* p_set, std::vector<point_t*> &top, int k);

//@brief Find the top-k point w.r.t linear function u
//@param u 			The linear function
//@param p_set 		The original dataset(in the form of vector)
//@param top 		The returned top-k point
//@param k 			The threhold of top-k
void find_top_k(point_t* u, std::vector<point_t*> p_set, std::vector<point_t*> &top, int k);

//@brief Use sampling to find all the points which is able to be top-1 at some utility vector
//@param p_set 		The point set containing all the points
//@param top_set	The returned point set containing all the possible top-1 point
//@param u 			The utility vector. For user, point_t* u = alloc_point(dim)
//@param level		The number of dimensions we have set. For user, only need to set level=0
//@param used_seg	The range which has been assigned to the u[i]. For user, set rest_seg=0
void find_top1_sampling(std::vector<point_t*> p_set, std::vector<point_t*> &top_set, point_t* u, int level, int used_seg);

void find_top1(std::vector<point_t *> p_set, std::vector<point_t *> &top);

//@brief Find all the points which are not dominated by >=k points
//param p_set 			The original dataset
//param return_point 	The returned points which are not dominated by >=k points
//param k 				The threhold
void skyband(point_set_t* p_set, std::vector<point_t*> &return_point, int k);

//@brief Find all the points which are not dominated by >=k points
//param p_set 			The original dataset(in the form of vector)
//param return_point 	The returned points which are not dominated by >=k points
//param k 				The threhold
void skyband(std::vector<point_t*> p_set, std::vector<point_t*> &return_point, int k);

void skyline_c(std::vector<point_t *> p_set, std::vector<point_t *> &return_point);

/*
 * @brief Initial utility range R
 * @param dim   Dimension
 * @return      The utility range(utility space)
 */
halfspace_set_t* R_initial(int dim);

/*
 * @brief Print the information of the chosen choose_item
 * @param choose_item_set 	The choose_item table
 * @param i 					The index of the chosen choose_item
 */
int print_choose_item_situation(std::vector<choose_item*> choose_item_set, int i);

/*
 * @brief Check the relation between the hyperplane and the half_set
 *		 Use bounding sphere/bounding rectangle to accelerate
 *		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
 * @param hyper 		The hyperplane
 * @param half_set	The half_set/Intersection of the halfspace
 * @param method 	0: use bounding sphere
 *					1: use bounding rectangle
 * @return The relation	1: half_set on the positive side of the hyperplane
 *						-1: half_set on the negative side of the hyperplane
 *						0: half_set intersects with the hyperplane
 *						-2: Error for check situation
 */
int check_situation_accelerate(hyperplane_t* hyper, halfspace_set_t* half_set, int method);

/*
 * @brief Check whether the half_set is on the positive side of the hyperplane
 *        Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
 * @param hyper 		The hyperplane
 * @param half_set	The half_set/Intersection of the halfspace
 * @return The relation	1: half_set on the positive side of the hyperplane
 *						0: half_set on the negative side of the hyperplane/intersects with the hyperplane
 *                      -2: Error for check situation
 */
int check_situation_positive(hyperplane_t* hyper, halfspace_set_t* half_set);

/*
 * @brief Find points which could be the top-k points for any utility vector in half_set
 *        Not accurate. It needs to be used with function check_possible_top_k()
 * @param p_set			The dataset
 * @param half_set		The half_set/intersection of the halfspace
 * @param top_current 	THe possible top-k points
 * @return              If there are possible top-k points, return true.
 *                      Otherwise return false
 */
bool find_possible_topk(std::vector<point_t*> p_set, halfspace_set_t* half_set, int k, std::vector<point_t*> &top_current);


/*
 * @brief Used to check whether there is a top-k points w.r.t any utility vector in half_set
 * @param p_set			The dataset containing all the points
 * @param half_set 		The half_set/intersection of the halfspace
 * @param k 			top-k
 * @param top_current 	The dataset containing all the possible top-k point found by function find_top_k_point_by_ext()
 * @return              If there is a top-k point w.r.t any utility vector in half_set, return it
 *                      Otherwise, return false
 */
point_t* check_possible_topk(std::vector<point_t*> p_set, halfspace_set_t* half_set, int k, std::vector<point_t*> &top_current);


/*
 * @brief Print the information of halfspaces and extreme points of R
 * @param R     The utility range R
 */
void print_halfspace_ext_R(halfspace_set_t *R);