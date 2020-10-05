#define Beta 0.01
#define Precision 0.0000015
#define RandRate 0.5

#include "data_utility.h"
#include "data_struct.h"
#include "read_write.h"
#include "pruning.h"
#include "../Qhull/qhull_build.h"
#include "lp.h"
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

//@brief Use LP to check each point, whether it is able to be top-1 at some utility vector
//@param p_set 		The point set containing all the points
//@param top_set	The returned point set containing all the possible top-1 point
void find_top1_accurate(std::vector<point_t*> p_set, std::vector<point_t*> &top_set);

void find_top1_qhull(std::vector<point_t *> p_set, std::vector<point_t*> &top);

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


void onion_onion(std::vector<point_t *> p_set, std::vector<point_t *> &top_set, int k);


void skyline_c(std::vector<point_t *> p_set, std::vector<point_t *> &return_point);

//@brief Set the in_center of the half_set as the average value of the extreme points
//		 In order to make sure sum(p[i])=1, set p[dim-1]=1-p[0]-p[1]-...-p[dim-2]
//@param half_set	The half_set/Intersection of the halfspace
void set_in_center(halfspace_set_t* half_set);

halfspace_set_t* R_initial(int dim);

//@brief check if there exist points which are the top-k point for any utility vector in the considered half_set
//@param half_set_set			It stores the information of all the half_set
//@param considered_half_set	The half_set we consider
//@param p 						The returned points
//@return If there exist the satisfied points, return true. Otherwise return false;
bool common_top_k_halfset(std::vector<halfspace_set_t*> half_set_set, std::vector<int> considered_half_set, std::vector<point_t*> &p);

//@brief Check the relation between the hyperplane and the half_set
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param half_set	The half_set/Intersection of the halfspace
//@return The relation	1: half_set on the positive side of the hyperplane
//						-1: half_set on the negative side of the hyperplane
//						0: half_set intersects with the hyperplane
//						-2: Error for check situation
int check_situation(hyperplane_t* hyper, halfspace_set_t* half_set);


//@brief Check the relation between the hyperplane and the half_set
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param half_set	The half_set/Intersection of the halfspace
//@return The relation	1: half_set on the positive side of the hyperplane
//						-1: half_set on the negative side of the hyperplane
//						0: half_set intersects with the hyperplane
//						-2: Error for check situation
int check_situation(hyperplane_t* hyper, vector<point_t *> ext_pts);

//@brief Print the information of the chosen choose_item
//@param choose_item_set 	The choose_item table
//@param i 					The index of the chosen choose_item
int print_choose_item_situation(std::vector<choose_item*> choose_item_set, int i);

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
int check_situation_accelerate(hyperplane_t* hyper, halfspace_set_t* half_set, int method);

int check_situation_accelerate(hyperplane_t *hyper, halfspace_set_t *half_set, int method, double &check_nnum, double &check_nnum_yes);


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
int check_situation_accelerate_LP(hyperplane_t *hyper, halfspace_set_t *half_set, int method);

//@brief Check whether the half_set is on the positive side of the hyperplane
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param half_set	The half_set/Intersection of the halfspace
//@return The relation	1: half_set on the positive side of the hyperplane
//						0: half_set on the negative side of the hyperplane/intersects with the hyperplane
//						-2: Error for check situation
int check_situation_positive(hyperplane_t* hyper, halfspace_set_t* half_set);


//@brief Check whether the half_set is on the positive side of the hyperplane
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//@param hyper 		The hyperplane
//@param ext_pts	All the extreme points
//@return The relation	1: half_set on the positive side of the hyperplane
//						0: half_set on the negative side of the hyperplane/intersects with the hyperplane
//						-2: Error for check situation
int check_situation_positive(hyperplane_t *hyper, vector<point_t *> ext_pts);


//@brief Used to find all the points which are the top-k point for any utility vector in half_set
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//		 Find common top-k points w.r.t extreme points(utility vector)
//		 Not accurate. It needs to be used with function check_possible_top_k()
//@param p_set			The datasets
//@param half_set		The half_set/Intersection of the halfspace
//@param top_current 	THe returned possible top-k point
//@return If there is possible top-k point, return true. Otherwise return false
bool find_top_k_point_by_ext(std::vector<point_t*> p_set, halfspace_set_t* half_set, int k, std::vector<point_t*> &top_current);

//@brief Used to find all the points which are the top-k point for any utility vector in half_set
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//		 Find common top-k points w.r.t extreme points(utility vector)
//		 Not accurate. It needs to be used with function check_possible_top_k()
//@param p_set			The datasets
//@param ext_pts		The extreme points of R
//@param top_current 	THe returned possible top-k point
//@return If there is possible top-k point, return true. Otherwise return false
bool find_top_k_point_by_ext(std::vector<point_t *> p_set, std::vector<point_t *> ext_pts, int k,
                             std::vector<point_t *> &top_current);



//@brief Used to find all the points which are the top-k point for any utility vector in half_set
//		 Since the extreme points of the half_set can not be accurate enough, we set "Precision" to solve the error
//		 Find common top-k points w.r.t extreme points(utility vector). Overlook the error.
//		 Not accurate. It needs to be used with function check_possible_top_k()
//@param p_set			The datasets
//@param half_set		The half_set/Intersection of the halfspace
//@param top_current 	THe returned possible top-k point
//@return If there is possible top-k point, return true. Otherwise return false
bool find_top_k_point_by_ext_no_acc(std::vector<point_t *> p_set, halfspace_set_t *half_set, int k, std::vector<point_t *> &top_current);


//@brief Used to check whether the points are the top-k points w.r.t any utility vector in half_set
//@param p_set			The dataset containing all the points
//@param half_set 		The half_set/Intersection of the halfspace
//@param k 				top-k
//@param top_current 	The datasets containing all the possible top-k point found by function find_top_k_point_by_ext()
//						The returned datasets containing all the points which are the top-k points w.r.t any utility vector in half_set
//@return If there are points which are the top-k points w.r.t any utility vector in half_set, return true
//		  Otherwise, return false.
bool check_possible_top_k(std::vector<point_t*> p_set, halfspace_set_t* half_set, int k, std::vector<point_t*> &top_current);


//@brief Used to check whether the points are the top-k points w.r.t any utility vector in half_set
//@param p_set			The dataset containing all the points
//@param ext_pts 		The extreme points of R
//@param k 				top-k
//@param top_current 	The datasets containing all the possible top-k point found by function find_top_k_point_by_ext()
//						The returned datasets containing all the points which are the top-k points w.r.t any utility vector in half_set
//@return If there are points which are the top-k points w.r.t any utility vector in half_set, return true
//		  Otherwise, return false.
bool check_possible_top_k(std::vector<point_t *> p_set, std::vector<point_t *> ext_pts, int k,
                          std::vector<point_t *> &top_current);

//@brief Used to prune points which are not able to be the top-k based on utility space R
//@param p_set 			The dataset containing all the points
//@param R 				The utility range
//@param k              The threshold top-k
void find_possible_top_k(std::vector<point_t*> &p_set, halfspace_set_t* R, int k);

void find_possible_top_k(vector<int> &C_idx, point_set_t *p_set, vector<point_t *> ext_pts, int k);
/**
 * @brief Used to prune points which are not able to be the top-k based on the ext_pts
 * @param p_set 			The dataset containing all the points
 * @param ext_pts 		    All the extreme points
 * @param k                 The threshold top-k
 */
void find_possible_top_k(std::vector<point_t *> &p_set, vector<point_t *> ext_pts, int k);

void find_possible_top_k(std::vector<point_t *> &p_set, vector<point_t *> ext_pts, int k, bool &interecting);

bool order_known(std::vector<point_t *> p_set, vector<point_t *> ext_pts);



/*
 * @brief Print the information of halfspaces and extreme points of R
 * @param R     The utility range R
 */
void print_halfspace_ext_R(halfspace_set_t *R);