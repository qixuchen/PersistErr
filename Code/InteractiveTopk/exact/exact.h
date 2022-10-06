#include "../Others/used_function.h"
#include <map>
#include <math.h>

#ifndef EXACT_ALG
#define EXACT_ALG



struct Region
{
	Region() = default;
	Region(int t): level(t){};

	int level;
	std::map<point_t *, frag_t *> frag_set;
};

#endif