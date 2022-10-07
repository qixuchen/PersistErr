#include "../Others/used_function.h"
#include <map>
#include <math.h>

#ifndef EXACT_ALG
#define EXACT_ALG

extern double best_score;
extern int correct_count;
extern double question_num;
extern double return_size;
struct Region
{
	Region() = default;
	Region(int t): level(t){};

	int level;
	std::map<point_t *, frag_t *> frag_set;
};


int Exact(std::vector<point_t *> p_set, point_t *u, int k);

#endif