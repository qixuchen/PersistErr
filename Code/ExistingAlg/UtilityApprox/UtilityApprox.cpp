#include "UtilityApprox.h"
#include <sys/time.h>
/**
 * @brief Finds the point q in p[0...N-1] which has the largest utility based on u
 * @param P         point set
 * @param u         the utility vector
 * @return the id of the best point
 */
int findbestpoint(point_set_t *P, point_t *u)
{
    int best = 0, i, j;
    double bestValue = 0, sum;

    for (i = 0; i < P->numberOfPoints; ++i)
    {
        sum = dot_prod(P->points[i], u);
        if (sum > bestValue)
        {
            best = i;
            bestValue = sum;
        }
    }
    return best;
}


/**
 * @brief Calculated the regret ratio of utility range
 * @param L the lower bound of each dimension
 * @param U the upper bound of each dimension
 * @param D the dimension
 * @return the regret ratio
 */
double rr_bound(double *L, double *U, int D)
{
    double bound = 0;

    for (int i = 0; i < D; i++)
    {
        bound += U[i] - L[i];
    }
    return bound;
}


/**
 * @brief Set the value of the extreme points of the rectangle
 * @param place      Set high/low value for the points
 * @param high       The largest value for each dimension
 * @param low        The smallest value for each dimension
 * @param ext_pts    All the extreme points
 * @param dim        The number of dimensions
 * @param layer      The number of dimensions we have set for the points
 */
void set_ext_on_R(bool *place, double *high, double *low, std::vector<point_t *> &ext_pts, int dim, int layer)
{
    if (layer + 1 == dim)
    {
        point_t *p1 = alloc_point(dim);
        point_t *p2 = alloc_point(dim);
        for (int i = 0; i < dim - 1; i++)
        {
            if (place[i] == true)
            {
                p1->coord[i] = high[i];
                p2->coord[i] = high[i];
            }
            else
            {
                p1->coord[i] = low[i];
                p2->coord[i] = low[i];
            }
        }
        p1->coord[dim - 1] = high[dim - 1];
        p2->coord[dim - 1] = low[dim - 1];
        ext_pts.push_back(p1);
        ext_pts.push_back(p2);
    }
    else
    {
        place[layer] = true;
        set_ext_on_R(place, high, low, ext_pts, dim, layer + 1);
        place[layer] = false;
        set_ext_on_R(place, high, low, ext_pts, dim, layer + 1);
    }
}


/**
 * @brief Danupon's Fake-Points algorithm
 * @param P the dataset
 * @param u the real utility vector
 * @param s the number of points shown in each question
 * @param epsilon the threshold of regret ratio
 * @param maxRound the upper bound of number of questions
 */
int utilityapprox(point_set_t *P, point_t *u, int s, double epsilon, int maxRound)
{
    int D = P->points[0]->dim;
    double delta, gamma, beta, regret = 1.0, sum;
    //double bestUtility, currUtility;
    int i, j, l, qIndex, alpha, t;
    int istar = 0, VERBOSE = 0;
    double *U = new double[D];
    double *L = new double[D];
    double *chi = new double[s + 1];
    double Qcount = 0;

    point_t *v = alloc_point(D);
    point_set_t *q_set = alloc_point_set(s);

    // Compute the utility of the best solution
    //bestUtility = dot_prod(P->points[findbestpoint(P, u)], u);
    //if (VERBOSE)
    //printf("best point index: %d\n", findbestpoint(D, N, p, prefs));

    // determine the attribute with highest rating in prefs
    for (i = 1; i < D; ++i)
        if (u->coord[i] > u->coord[istar])
        {
            istar = i;
        }

    //if (VERBOSE)
    //printf("istar = %d\n", istar);

    // increment number of rounds according to how many were needed
    // to determine highest rating above
    if ((D - 1) % (s - 1) == 0)
    {
        Qcount = (D - 1) / (s - 1);
    }
    else
    {
        Qcount = (D - 1) / (s - 1) + 1;
    }

    // initialize U and L
    for (i = 0; i < D; ++i)
    {
        U[i] = 1.0;
        if (i == istar)
        {
            L[i] = 1.0;
        }
        else
        {
            L[i] = 0.0;
        }
    }

    // create memory for the K points that will be shown
    for (i = 0; i < s; ++i)
    {
        q_set->points[i] = alloc_point(D);
    }

    i = 0;
    t = 1;
    vector<int> C;
    while ((rr_bound(L, U, D) > epsilon && !isZero(rr_bound(L, U, D) - epsilon)) && Qcount < maxRound)
    {
        //printf("%d\n", C.size());
        //if (VERBOSE) printf("i = %d, rounds = %d, t = %d\n", i, rounds, t);
        if (i != istar)
        {
            // compute v
            for (j = 0; j < D; ++j)
                v->coord[j] = (U[j] + L[j]) / 2.0;

            // find (the index of) the point that is most preferred by v
            qIndex = findbestpoint(P, v);

            // calculate chi
            for (j = 0; j <= s; ++j)
                chi[j] = L[i] + j * (U[i] - L[i]) / s;

            // compute delta as in latest version
            sum = 0.0;
            for (j = 1; j < s; ++j)
                sum = sum + chi[j] - L[i];

            delta = 1.0 / (pow(s * 1.0, (t / (D - 1)) * 1.0) * sum);

            if (delta > 1e-5)
            {
                delta = 1e-5;
            }

            //if (VERBOSE) printf("delta = %lf\n", delta);

            // compute gamma
            gamma = P->points[qIndex]->coord[istar] / P->points[qIndex]->coord[i];

            // create the set of fake points
            for (l = 0; l < D; ++l)
                q_set->points[s - 1]->coord[l] = P->points[qIndex]->coord[l];

            for (j = s - 2; j >= 0; j -= 1)
            {
                for (l = 0; l < D; ++l)
                    if (l == istar)
                    {
                        q_set->points[j]->coord[l] = q_set->points[j + 1]->coord[l] +
                                                     chi[j + 1] * delta * gamma * q_set->points[j + 1]->coord[i];
                    }
                    else if (l == i)
                    {
                        q_set->points[j]->coord[l] =
                                q_set->points[j + 1]->coord[l] - delta * gamma * q_set->points[j + 1]->coord[i];
                    }
                    else
                    {
                        q_set->points[j]->coord[l] = P->points[qIndex]->coord[l];
                    }
            }
            // compute beta and rescale points by it
            sum = 0.0;
            for (j = 0; j < s; ++j)
                sum += chi[j + 1] - L[i];
            beta = 1.0 / (1.0 + sum * delta);  // multiply by delta at the end to try and avoid underflow
            for (j = 0; j < s; ++j)
                for (l = 0; l < D; ++l)
                    q_set->points[j]->coord[l] = q_set->points[j]->coord[l] * beta;

            // check which point the user would pick and update L and U accordingly
            alpha = findbestpoint(q_set, u);
            //if (VERBOSE) printf("alpha = %d\n", alpha);
            //if (VERBOSE) printf("old L[%d] = %lf, old U[%d] = %lf\n", i, L[i], i, U[i]);
            L[i] = chi[alpha];
            U[i] = chi[alpha + 1];
            /*if (VERBOSE) printf("new L[%d] = %lf, new U[%d] = %lf\n", i, L[i], i, U[i]);*/

            // compute utility and regret of current best
            //currUtility = dot_prod(P->points[findbestpoint(P, v)], u);
            //regret = 1.0 - currUtility / bestUtility;

            //if (VERBOSE) printf("curr best point index: %d\n", findbestpoint(D, N, p, v));
            //if (VERBOSE)
            //{
            //	printf("best and curr utilities: %lf, %lf\nregret at round %d is %lf\n", bestUtility, currUtility, rounds, regret);
            //	printf("u: ");
            //	for(j = 0; j < D; ++j)
            //		printf("%lf ",  u->coord[j]);
            //	printf("\nv: ");
            //	for(j = 0; j < D; ++j)
            //		printf("%lf ",  v->coord[j]);
            //	printf("\n\n");
            //}
            Qcount = Qcount + 1;
        }
        i = (i + 1) % D; // cycle through all the dimensions
        t += 1;
    }
    delete[] U;
    delete[] L;
    delete[] chi;

    point_t *result = P->points[findbestpoint(P, v)];
    release_point(v);
    release_point_set(q_set, true);

    printf("|%30s |%10.0lf |%10s |\n", "UtilityApprox", Qcount, "--");
    printf("|%30s |%10s |%10d |\n", "Point", "--", result->id);
    printf("---------------------------------------------------------\n");
    return Qcount;
}