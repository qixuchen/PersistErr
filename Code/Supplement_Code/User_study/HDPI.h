#include "Others/used_function.h"

void HDPI_sampling(FILE *wPtr, std::vector<point_t *> p_set, point_set_t *original_P, int k, int interval, point_t* u, double *numQ, vector<point_t*> *resPoint);

void HDPI_accurate(FILE *wPtr, std::vector<point_t *> p_set, point_set_t *original_P, int k, int interval, point_t* u, double *numQ, vector<point_t*> *resPoint);
