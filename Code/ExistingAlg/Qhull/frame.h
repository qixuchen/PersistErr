#ifndef FRAME_H
#define FRAME_H

#include "../Others/data_struct.h"
#include "../Others/data_utility.h"
#include "../Others/lp.h"
#include "../Others/operation.h"

// frame compuatation (all at once)
void frameConeFastLP(vector<point_t*> rays, vector<int>& idxs);

// parital frame compuatation (only s)
void partialConeFastLP(vector<point_t*> rays, vector<int>& B, int s);

// frame compuatation (all at once) - naive implementation
void frameConeLP(std::vector<point_t*> rays, std::vector<int>& idxs);

#endif