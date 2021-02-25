#define Beta 0.01
#define Precision 0.0000015
#define RandRate 0.5

#include "data_utility.h"
#include "data_struct.h"
#include "read_write.h"
#include "pruning.h"
#include "../Qhull/qhull_build.h"
#include "../lp.h"
#include <vector>
#include <ctime>
#include <math.h>


int show_to_user(point_set_t* P, int p_idx, int q_idx);

bool is_same_point(point_t* p1, point_t* p2);

bool is_same_exist(point_t *p, vector<point_t*> &p_set);

void point_reload(point_set_t* original_set, std::vector<point_t*> &p_set);

point_set_t* point_reload(std::vector<point_t *> p_set);

void point_reload_with_random(point_set_t* original_set, std::vector<point_t*> &p_set);

void point_random(std::vector<point_t *> &p_set);

void point_reload_with_random_delete_same(point_set_t* original_set, std::vector<point_t*> &p_set);

point_set_t * point_reload_delete_same(std::vector<point_t *> p_set);

void delete_same(std::vector<point_t *> p_set, std::vector<point_t *> &p0);

void sort_point(std::vector<point_t*> p_set, std::vector<point_t*> &return_point, point_t* u);

double calculate_distance(hyperplane_t* hyper, point_t* p);

void calculate_length(halfspace_set_t *R, double *len, double *position);

double calculate_absolute_distance(hyperplane_t *hyper, point_t *p);

void find_top_k(point_t* u, point_set_t* p_set, std::vector<point_t*> &top, int k);

void find_top_k(point_t* u, std::vector<point_t*> p_set, std::vector<point_t*> &top, int k);

void find_top1_sampling(std::vector<point_t*> p_set, std::vector<point_t*> &top_set, point_t* u, double segment, int level, int used_seg);

void find_top1_sampling(std::vector<point_t *> p_set, std::vector<point_t *> &top_set, point_t *u, double segment, int level, int used_seg, double *posi, double *len);

void find_top1_accurate(std::vector<point_t*> p_set, std::vector<point_t*> &top_set);

void find_top1_qhull(std::vector<point_t *> p_set, std::vector<point_t*> &top);

void find_top1(std::vector<point_t *> p_set, std::vector<point_t *> &top);

void skyband(point_set_t* p_set, std::vector<point_t*> &return_point, int k);

void skyband(std::vector<point_t*> p_set, std::vector<point_t*> &return_point, int k, int id_num);

void skyband(std::vector<point_t*> p_set, std::vector<point_t*> &return_point, int k);

void skyline_c(std::vector<point_t *> p_set, std::vector<point_t *> &return_point);

void set_in_center(halfspace_set_t* half_set);

halfspace_set_t* R_initial(int dim);

int check_situation(hyperplane_t* hyper, halfspace_set_t* half_set);

int check_situation(hyperplane_t* hyper, vector<point_t *> ext_pts);

int print_choose_item_situation(std::vector<choose_item*> choose_item_set, int i);

int check_situation_accelerate(hyperplane_t* hyper, halfspace_set_t* half_set, int method);

int check_situation_accelerate(hyperplane_t *hyper, halfspace_set_t *half_set, int method, double &check_nnum, double &check_nnum_yes);

int check_situation_positive(hyperplane_t* hyper, halfspace_set_t* half_set);

int check_situation_positive(hyperplane_t *hyper, vector<point_t *> ext_pts);

bool find_possible_topk(std::vector<point_t*> p_set, halfspace_set_t* half_set, int k, std::vector<point_t*> &top_current);

point_t* check_possible_topk(std::vector<point_t*> p_set, halfspace_set_t* half_set, int k, std::vector<point_t*> &top_current);

bool check_possible_s_topk(std::vector<point_t *> &p_set, halfspace_set_t *half_set, int k, int s, std::vector<point_t *> &top_current);

void print_halfspace_ext_R(halfspace_set_t *R);

void print_interaction_result(FILE *wPtr, char *name, int k, int s, int numOfQuestion, vector<point_t*> &top_current, point_set_t *original_P);

void algorithm_comparison(FILE *wPtr, point_set_t *P, double *num, vector<point*> *res, int k);