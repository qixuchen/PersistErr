#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "Others/qhull_build.h"
#include "lp.h"
#include <vector>
#include <ctime>
#include <math.h>
#include <iostream>
#include <string>

#ifndef USER_STUDY_FUNC
#define USER_STUDY_FUNC

#define BUF_SIZE 1024

int show_to_user(point_set_t* P, int p_idx, int q_idx);

int show_to_user(const point_t* p1, const point_t* p2);

void print_result_list(point_set_t* P, const vector<point_t *> &point_return);

int show_to_user_scale(point_set_t* P, int p_idx, int q_idx);

int show_to_user_scale(const point_t* p1, const point_t* p2);

void print_result(FILE* wPtr, const char *alg_name, const int num_of_question, const point* p, int alg_num);

void display_final_list(const point_set_t* P, const std::vector<int> final_list);

int ask_favorite_item(int l_size);

int ask_dissat_score(const point_set_t* P, int car_id);

int ask_satisfactory_level(const point_set_t* P, const int num_questions_asked, const int p_result_id, const char *alg_name, FILE *w_ptr);

void record_to_file(FILE *wPtr, int *records, int r_size);




#endif