#include "exp_stats.h"

double best_score = 0;
double question_num = 0;
double return_size = 0;
int correct_count = 0;
double tot_time = 0;


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