#include "../Others/used_function.h"
#include "../exp_stats.h"
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

double compute_hy_priority(const item *item_ptr, const std::vector<std::vector<point_t*>> &points_in_region);

void construct_halfspace_set(std::vector<point_t *> &p_set, std::vector<point_t *> &choose_item_points,
                             std::vector<halfspace_set_t *> &half_set_set, std::set<int> &considered_half_set);

int build_choose_item_table(std::vector<halfspace_set_t *> half_set_set, std::vector<point_t *> p_set,
                            std::vector<item *> &choose_item_set);
 
void construct_hy_candidates(std::map<int, hyperplane_t *> &hyperplane_candidates, const std::vector<item *> &choose_item_set);

int Exact(std::vector<point_t *> p_set, point_t *u, int k);

#endif