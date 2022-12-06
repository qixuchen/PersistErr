#include <chrono>

#ifndef EXP_STATS
#define EXP_STATS

extern double best_score;
extern int correct_count;
extern double question_num;
extern double return_size;

void reset_timer();
void start_timer();
void stop_timer();
void incre_total_time_microsec();
double avg_time();

#endif