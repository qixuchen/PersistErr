#ifndef PREFERLEARN_H
#define PREFERLEARN_H

#include "../Others/data_struct.h"
#include "../Others/data_utility.h"
#include "../Others/operation.h"
#include "../Others/read_write.h"
#include "../Others/pruning.h"
#include "../Others/used_function.h"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include "../Others/QuadProg++.hh"
using namespace quadprogpp;

//@brief Used to find the estimated utility vector by max-min
//@param V          All the hyperplanes which bounds the possible utility range
//@return           The estimated utility vector
point_t* find_estimate(std::vector<point_t*> V);

//@brief Find one of the top-k point by pairwise learning. Use the cos() of real u and estimated u as the accuracy.
//       The stop condition is that cos() should satisfy the given threshold
//@param original_set       The original dataset
//@param u                  The real utility vector
//@param k                  The threshold tok-k
int Preference_Learning_accuracy(std::vector<point_t*> original_set, point_t* u, int k);


#endif