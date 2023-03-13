#include "exp_stats.h"

double best_score = 0;
double question_num = 0;
double return_size = 0;
int correct_count = 0;
double tot_time = 0;
int jump = 1;
int dissat_score_best = 0;
int question_asked_list[TOT_ALG_COUNT] = {0};
int best_pid_list[TOT_ALG_COUNT] = {0};
int return_size_list[TOT_ALG_COUNT] = {0};
double proc_time_list[TOT_ALG_COUNT] = {0};
int hit_list[TOT_ALG_COUNT] = {0};
int dissat_score_list[TOT_ALG_COUNT] = {0};
std::vector<std::string> file_names = {"1.txt", "2.txt", "3.txt", "4.txt", "5.txt", "6.txt"};
std::vector<std::vector<int>> recommendation_list(TOT_ALG_COUNT, std::vector<int>());
std::vector<int> user_choices;
std::vector<std::pair<int, int>> displayed_questions;
std::vector<std::pair<int,int>> inconsistency;

using namespace std::chrono;
steady_clock::time_point begin, end;

void reset_timer(){
    tot_time = 0;
}

void start_timer(){
    begin = steady_clock::now();
}

void stop_timer(){
    end = steady_clock::now();
    incre_total_time_microsec();
}

void incre_total_time_microsec(){
    tot_time += duration_cast<std::chrono::microseconds>(end - begin).count();
}

double avg_time(){
    return tot_time / question_num;
}

double avg_time(const int round){
    return tot_time / round;
}

void reset_stats(){
    best_score = 0;
    question_num = 0;
    return_size = 0;
    correct_count = 0;
    tot_time = 0;
    inconsistency.clear();
    user_choices.clear();
    displayed_questions.clear();
}