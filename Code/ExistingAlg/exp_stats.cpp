#include "exp_stats.h"
#include <vector>
#include <algorithm>
#include <iostream>

double best_score = 0;
double question_num = 0;
double return_size = 0;
int correct_count = 0;
double tot_time = 0;
std::vector<double> t_vec;

using namespace std::chrono;
steady_clock::time_point begin, end;

void reset_timer(){
    tot_time = 0;
    t_vec.clear();
}

void start_timer(){
    begin = steady_clock::now();
}

void stop_timer(){
    end = steady_clock::now();
    incre_total_time_microsec();
    t_vec.push_back(duration_cast<std::chrono::microseconds>(end - begin).count());
}

void incre_total_time_microsec(){
    tot_time += duration_cast<std::chrono::microseconds>(end - begin).count();
}

double avg_time(){
    return tot_time / question_num;
}

double total_time(){
    return tot_time;
}

void compute_time_stats(){
    std::sort(t_vec.begin(), t_vec.end());
    int size = t_vec.size();
    std::cout << "P95 time: " << t_vec[(int)(size*0.95)] << std::endl;
    std::cout << "Median time: " << t_vec[(int)(size*0.5)] << std::endl;
}