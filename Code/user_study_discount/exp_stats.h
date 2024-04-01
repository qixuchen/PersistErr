#ifndef EXP_STATS
#define EXP_STATS

#include <chrono>
#include <string>
#include <vector>

#define TOT_ALG_COUNT 3

extern double best_score;
extern int correct_count;
extern double question_num;
extern double return_size;
extern int jump;
extern int dissat_score_best;
extern int question_asked_list[TOT_ALG_COUNT];
extern int best_pid_list[TOT_ALG_COUNT];
extern int return_size_list[TOT_ALG_COUNT];
extern double proc_time_list[TOT_ALG_COUNT];
extern int hit_list[TOT_ALG_COUNT];
extern int dissat_score_list[TOT_ALG_COUNT];
extern std::vector<std::string> file_names;
extern std::vector<std::vector<int>> recommendation_list;
extern std::vector<int> user_choices;
extern std::vector<std::pair<int, int>> displayed_questions;
extern int higher;

// #define EXACT_ALG 0
// #define SAMPLING_ALG 1
// #define OPTIMAL_ALG 2
// #define HDPI_ALG 3
// #define POINTPRUNE_ALG 4
// #define ACTIVERANK_ALG 5
// #define PREFLEARN_ALG 6

void reset_timer();
void start_timer();
void stop_timer();
void incre_total_time_microsec();
double avg_time();
double avg_time(const int round);
void reset_stats();

#endif