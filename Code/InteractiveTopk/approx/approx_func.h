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


/** @brief      Small checking function, sum of point coordinates
 */    
double coord_sum(point_t * p);


int check_ext_pt_validity(std::vector<point_t *> &aggre_ext_pts);


/** @brief      Compute the number of fragments in a Region r
 */
vector<point_t *> extract_frags(const Enclosure &e);


/** @brief      Find the points still not pruned
 */
std::set<point_t *> enclosure_compute_considered_set(const std::vector<Enclosure> &enclosures);


/** @brief          Put points in p_set in to p_filename 
*/
void record_point_set(const std::vector<point_t *> &p_set, const string &p_filename);


/**
 * @brief      Given a halfspace hs, get the other halfspace
*/
halfspace_t *get_hs_counterpart(halfspace_t *hs);


halfspace_set_t * compute_convh_hyperplanes(const std::vector<point_t *> &p_set);

#endif