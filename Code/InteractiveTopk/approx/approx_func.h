#include "../Others/used_function.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <cmath>
#include <map>
#ifndef APPROX_FUNC
#define APPROX_FUNC

# define OFFSET_THRESHOLD 0.000001

struct Enclosure
{
    Enclosure() = default;
    Enclosure(int t): level(t){};

	int level;
    std::vector<point_t *> ext_pts;
    halfspace_set_t *half_set;
    std::map<point_t *, frag_t *> frag_set;
};



/** @brief          Put points in p_set in to p_filename 
*/
void record_point_set(const std::vector<point_t *> &p_set, const string &p_filename);

halfspace_set_t * compute_convh_hyperplanes(const std::vector<point_t *> &p_set);

#endif