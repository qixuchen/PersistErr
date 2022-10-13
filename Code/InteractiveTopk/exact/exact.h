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


std::vector<point_t *> extract_frags(const Region &r);

bool check_correctness(const std::set<point_t *> &considered_points, point_t *u, double best_score);


std::set<point_t *> compute_considered_set(const std::vector<Region> &regions);


double compute_hy_priority(const item *item_ptr, const std::vector<std::vector<point_t*>> &points_in_region);


int find_best_hyperplane(const std::vector<item *> &choose_item_set, 
                            std::map<int, hyperplane_t *> &hyperplane_candidates, std::vector<Region> &regions);


void construct_hy_candidates(std::map<int, hyperplane_t *> &hyperplane_candidates, const std::vector<item *> &choose_item_set);


void construct_fragment_in_initialized_R(const std::vector<halfspace_set_t *> &half_set_set, std::map<point_t *, frag_t *> &frag_set);


void construct_halfspace_set(std::vector<point_t *> &p_set, std::vector<point_t *> &choose_item_points,
                             std::vector<halfspace_set_t *> &half_set_set, std::set<int> &considered_half_set);


int build_choose_item_table(std::vector<halfspace_set_t *> half_set_set, std::vector<point_t *> p_set,
                            std::vector<item *> &choose_item_set);


void construct_hy_candidates(std::map<int, hyperplane_t *> &hyperplane_candidates, const std::vector<item *> &choose_item_set);


void rt_recurrence(std::vector<Region> &regions, halfspace_t* hs_ptr);


int Exact(std::vector<point_t *> p_set, point_t *u, int k);

#endif