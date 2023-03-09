#include "Others/data_utility.h"
#include "Others/data_struct.h"
#include "Others/read_write.h"
#include "Others/pruning.h"
#include "Others/qhull_build.h"
#include "lp.h"
#include "exp_stats.h"
#include <vector>
#include <ctime>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#ifndef USER_STUDY_FUNC
#define USER_STUDY_FUNC

#define BUF_SIZE 1024

bool is_number(const string& s);

void enter_to_continue();

void enter_recover();

void handle_recover(int jump_id);

void intro();

void part1_info();

void part1_end();

void part2_info();

void part2_skip();

void part2_second_task_skip();

void part2_end();

void end();

int show_to_user(point_set_t* P, int p_idx, int q_idx);

int show_to_user(const point_t* p1, const point_t* p2);

void print_result_list(point_set_t* P, const vector<point_t *> &point_return);

void print_result_list(point_set_t* P, const vector<int> &ids);

int alg_top1_select(const vector<point_t *> &point_return);

void write_results_to_file(const int alg_id, const vector<point_t *> &point_return, const int best_idx);

void record_ques_history(const int alg_id);

void write_cf_info(const int alg_id, const vector<int> &cr_belong);

void print_alg_start(const int alg_num);

void print_alg_end(const int alg_num);

void display_final_list(point_set_t* P, std::vector<int> final_list);

int ask_favorite_item(int l_size);

int confirm_favorite_item(point_set_t* P, std::vector<int> final_list, int possible_best_idx);

std::set<int> find_dissatisfactory_lists(point_set_t *P, int best_pid);

void dissat_info_best();

int ask_dissat_score_best(point_set_t *P,  int best_id);

void dissat_info(int dissat_score_best);

int ask_dissat_score(point_set_t *P,  vector<int> &ids, int dissat_score_best);

void record_to_file(FILE *wPtr, int *records, int r_size);

void write_summary();



#endif