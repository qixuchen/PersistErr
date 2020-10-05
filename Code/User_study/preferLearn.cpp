#include "preferLearn.h"
#include "Others/pruning.h"
#include "Others/operation.h"
#include "Others/read_write.h"
#include <sys/time.h>
#include <math.h>
#include <stack>

#define Lnum 50
#define pi 3.1415

//@brief Used to find the estimated utility vector by max-min
//@param V          All the hyperplanes which bounds the possible utility range
//@return           The estimated utility vector
point_t *find_estimate(std::vector<point_t *> V)
{
    if (V.size() == 0)
    {
        return NULL;
    }
    else if (V.size() == 1)
    {
        return copy(V[0]);
    }

    int dim = V[0]->dim;

    double zero = 0.00000001;

    //Use QuadProg++ to solve the quadratic optimization problem
    Matrix<double> G, CE, CI;
    Vector<double> g0, ce0, ci0, x;
    int n, m, p;
    char ch;
    // Matrix G and g0 determine the objective function.
    // min 1/2 x^T*G*x + g0*x
    n = dim;
    G.resize(n, n);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (i == j)
            {
                G[i][j] = 1;
            }
            else
            {
                G[i][j] = 0;
            }
        }
    }
    /*
    std::cout << "G:" << std::endl;;
    for(int i = 0; i < n; i++)
    {
    for(int j = 0; j < n; j++)
    {
    std::cout << G[i][j] << " ";
    }
    std::cout << std::endl;;
    }
    */

    g0.resize(n);
    for (int i = 0; i < n; i++)
    {
        g0[i] = 0;
    }

    /*
    std::cout << "g0:" << std::endl;;
    for(int i = 0; i < n; i++)
    {
    std::cout << g0[i] << " ";
    }
    std::cout << std::endl;
    */

    // CE and ce0 determine the equality constraints
    // CE*x = ce0
    m = 0;
    CE.resize(n, m);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            CE[i][j] = 0;
        }
    }
    /*
    std::cout << "CE:" << std::endl;;
    for(int i = 0; i < n; i++)
    {
    for(int j = 0; j < m; j++)
    {
    std::cout << CE[i][j] << " ";
    }
    std::cout << std::endl;;
    }
    */

    ce0.resize(m);
    for (int j = 0; j < m; j++)
    {
        ce0[j] = 0;
    }
    /*
    std::cout << "ce0:" << std::endl;;
    for(int i = 0; i < m; i++)
    {
    std::cout << ce0[i] << " ";
    }
    std::cout << std::endl;
    */

    // CI and ci0 determine the inequality constraints
    // CI*x >= ci0
    p = V.size();
    CI.resize(n, p);
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < p; j++)
        {
            CI[i][j] = V[j]->coord[i];
        }
    }

    ci0.resize(p);
    for (int j = 0; j < p; j++)
    {
        ci0[j] = -1;
    }

    x.resize(n);

    //invoke solve_quadprog() in QuadProg++ to solve the problem for us
    // result is stored in x
    solve_quadprog(G, g0, CE, ce0, CI, ci0, x);

    //transform x into our struct
    point_t *estimate = alloc_point(dim);
    for (int i = 0; i < dim; i++)
    {
        estimate->coord[i] = x[i];
    }

    /*
    for(int i = dim; i < n; i++)
    {
    if(isZero(x[i]))
    printf("%lf ", x[i]);
    }
    printf("\n");
    */

    //printf("f: %f \n", dist);
    //print_point(result);
    //printf("p to p dist: %f \n", pow(calc_dist(result, query),2));
    return estimate;
}


//@brief Used to normalize the hyperplane so that ||H||=1
//@param hyper      The hyperplane
void hyperplane_nomarlize(hyperplane_t *hyper)
{
    int dim = hyper->normal->dim;
    double sum = 0;
    for (int i = 0; i < dim; i++)
    {
        sum += hyper->normal->coord[i] * hyper->normal->coord[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < dim; i++)
    {
        hyper->normal->coord[i] /= sum;
    }
}


//@brief Used to normalized the point so that ||P||=1
//@param            The point
void point_nomarlize(point_t *p)
{
    int dim = p->dim;
    double sum = 0;
    for (int i = 0; i < dim; i++)
    {
        sum += p->coord[i] * p->coord[i];
    }
    sum = sqrt(sum);
    for (int i = 0; i < dim; i++)
    {
        p->coord[i] /= sum;
    }
}


//@brief Used to calculate the cos() of angle of two vectors
//@param h1         The vector
//@param h2         The vector
//@return           The cos() of the angle
double cosine0(point_t *h1, point_t *h2)
{
    int dim = h1->dim;
    double sum = 0, s_h1 = 0, s_h2 = 0;
    for (int i = 0; i < dim; i++)
    {
        sum += h1->coord[i] * h2->coord[i];
        s_h1 += h1->coord[i] * h1->coord[i];
        s_h2 += h2->coord[i] * h2->coord[i];
    }
    s_h1 = sqrt(s_h1);
    s_h2 = sqrt(s_h2);
    return sum / (s_h1 * s_h2);
}


//@brief Used to calculate the orthogonality of two vectors. 1 - |cos()|
//@param h1         The vector
//@param h2         The vector
//@return           The orthogonality
double orthogonality(point_t *h1, point_t *h2)
{
    double value = cosine0(h1, h2);
    if (value >= 0)
    {
        return 1 - value;
    }
    else
    {
        return 1 + value;
    }
}


//@brief Used to calculate the largest orthogonality of the spherical cap w.r.t a vector
//@param n          The vector
//@param node       The spherical cap
//@return           The largest orthogonality
double upper_orthog(point_t *n, s_node_t *node)
{
    double alpha0 = cosine0(n, node->center); //cos(a)
    alpha0 = acos(alpha0); //angle
    double theta0 = acos(node->angle);
    if ((alpha0 - theta0) < pi / 2 && (alpha0 + theta0) > pi / 2)
    {
        return 1;
    }
    else
    {
        double v1 = fabs(cos(alpha0 + theta0));
        double v2 = fabs(cos(alpha0 - theta0));
        if (v1 < v2)
        {
            return 1 - v1;
        }
        else
        {
            return 1 - v2;
        }
    }
}


//@brief Used to calculate the smallest orthogonality of the spherical cap w.r.t a vector
//@param n          The vector
//@param node       The spherical cap
//@return           The smallest orthogonality
double lower_orthog(point_t *n, s_node_t *node)
{
    double alpha0 = cosine0(n, node->center); //cos(a)
    alpha0 = acos(alpha0); //angle
    double theta0 = acos(node->angle);
    if (alpha0 < theta0 || (alpha0 + theta0) > pi)
    {
        return 0;
    }
    else
    {
        double v1 = fabs(cos(alpha0 + theta0));
        double v2 = fabs(cos(alpha0 - theta0));
        if (v1 > v2)
        {
            return 1 - v1;
        }
        else
        {
            return 1 - v2;
        }
    }
}


//@brief Used to classified the hyperplanes into L clusters by k-means
//         Note that by re-assign hyperplane into clusters, the number of clusters may <L finally
//@param hyper      All the hyperplanes
//@param clu        All the clusters obtained
void k_means_cosine(std::vector<hyperplane_t *> hyper, std::vector<cluster_t *> &clu)
{
    int M = hyper.size();
    int dim = hyper[0]->normal->dim;
    int seg = M / Lnum;
    for (int i = 0; i < Lnum; i++)
    {
        cluster_t *c = alloc_cluster();
        c->center = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            c->center->coord[j] = hyper[i * seg]->normal->coord[j];
        }
        clu.push_back(c);
    }
    double shift = 9999;
    while (shift >= 0.1)
    {
        //initial
        shift = 0;
        for (int i = 0; i < Lnum; i++)
        {
            clu[i]->h_set.clear();
        }
        //scan each hyperplane, insert into a cluster
        for (int i = 0; i < M; i++)
        {
            //Find the cluster for each hyperplane
            double cos_sim = -2;
            int index = -1;
            for (int j = 0; j < Lnum; j++)
            {
                double cs = cosine0(hyper[i]->normal, clu[j]->center);
                if (cs > cos_sim)
                {
                    cos_sim = cs;
                    index = j;
                }
            }
            clu[index]->h_set.push_back(hyper[i]);
        }
        //renew the center of each cluster
        for (int i = 0; i < Lnum; i++)
        {
            for (int j = 0; j < dim; j++)
            {
                double value = 0;
                for (int a = 0; a < clu[i]->h_set.size(); a++)
                {
                    value += clu[i]->h_set[a]->normal->coord[j];
                }
                value /= clu[i]->h_set.size();
                shift = shift + value - clu[i]->center->coord[j];
                clu[i]->center->coord[j] = value;
            }
        }
    }
    //delete the cluster with 0 members
    int index = 0;
    for (int i = 0; i < Lnum; i++)
    {
        if (clu[index]->h_set.size() == 0)
        {
            clu.erase(clu.begin() + index);
        }
        else
        {
            index++;
        }
    }
}


//@brief Used to build the sphereical cap
//       1. Find the representative vector  2. Set the cos()
//@param            The spherical node
void cap_construction(s_node_t *node)
{
    std::vector<point_t *> V;
    point_t *pt;
    int M = node->hyper.size();
    int dim = node->hyper[0]->normal->dim;
    for (int i = 0; i < M; i++)
    {
        pt = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            pt->coord[j] = node->hyper[i]->normal->coord[j];
        }
        V.push_back(pt);
    }
    node->center = find_estimate(V);
    point_nomarlize(node->center);
    /*
    printf("center  ");
    for(int j=0; j < dim; j++)
    {
        printf("%lf  ", node->center->coord[j]);
    }
    printf("\n");
    for(int i=0; i < M; i++)
    {
        printf("point%d ", i);
        for(int j=0; j < dim; j++)
        {
            printf("%lf ", node->hyper[i]->normal->coord[j]);
        }
        printf("p1 %d p2 %d\n", node->hyper[i]->point1->id, node->hyper[i]->point2->id);
    }
    */
    node->angle = cosine0(node->center, node->hyper[0]->normal);
    for (int i = 1; i < M; i++)
    {
        double angle = cosine0(node->center, node->hyper[i]->normal);
        if (angle < node->angle)
        {
            node->angle = angle;
        }
    }
}


//@brief Used to build the spherical tree
//@param hyper      All the hyperplanes
//@param node       The node of the tree. For user, only set the root node to call this function
void build_spherical_tree(std::vector<hyperplane_t *> hyper, s_node_t *node)
{
    int M = hyper.size();
    int dim = hyper[0]->normal->dim;
    //build leaf
    if (M <= 50)
    {
        for (int i = 0; i < M; i++)
        {
            node->hyper.push_back(hyper[i]);
        }
        node->is_leaf = true;
        cap_construction(node);
    }
    else//build internal node
    {
        std::vector<cluster_t *> clu;
        //obtain all the clusters
        k_means_cosine(hyper, clu);
        //build a node for each cluster and set the center by spherical cap construction
        int clu_size = clu.size();
        s_node *s_n;
        if (clu_size == 1)
        {
            cluster_t *c = alloc_cluster();
            int countt = clu[0]->h_set.size();
            for (int j = 0; j < countt / 2; j++)
            {
                c->h_set.push_back(clu[0]->h_set[0]);
                clu[0]->h_set.erase(clu[0]->h_set.begin());
            }
            clu.push_back(c);
            clu_size = clu.size();
        }
        for (int i = 0; i < clu_size; i++)
        {
            s_n = alloc_s_node(dim);
            build_spherical_tree(clu[i]->h_set, s_n);
            node->child.push_back(s_n);
        }
        for (int i = 0; i < M; i++)
        {
            node->hyper.push_back(hyper[i]);
        }
        node->is_leaf = false;
        cap_construction(node);
    }
}


//brief Used to prune the impossible spherical caps for searching
//@param q          The searching utility vector
//@param S          The spherical caps for searching
//@param Q          The spherical caps refined
void spherical_cap_pruning(point_t *q, std::vector<s_node_t *> S, std::vector<s_node_t *> &Q)
{
    int M = S.size();
    Q.push_back(S[0]);
    double maxLB = lower_orthog(q, S[0]);
    for (int i = 1; i < M; i++)
    {
        double UB = upper_orthog(q, S[i]);
        double LB = lower_orthog(q, S[i]);
        if (UB > maxLB)
        {
            //deal with Q
            int index = 0, q_size = Q.size();
            for (int j = 0; j < q_size; j++)
            {
                double UB0 = upper_orthog(q, Q[index]);
                if (UB0 < LB)
                {
                    Q.erase(Q.begin() + index);
                }
                else
                {
                    index++;
                }
            }
            //deal with maxLB
            if (LB > maxLB)
            {
                maxLB = LB;
            }
            //insert into Q
            q_size = Q.size();
            int a = 0;
            for (a = 0; a < q_size; a++)
            {
                double UB0 = upper_orthog(q, Q[a]);
                if (UB < UB0)
                {
                    break;
                }
            }
            Q.insert(Q.begin() + a, S[i]);
        }
    }
}


//@brief Used to find the hyperplane asking question through spherical tree based on estimated u
//@param node       The root of spherical tree
//@param q          The estimated u
//@param best       The best hyperplane found so far. For user, set best=NULL to call this function
//@return   The hyperplane used to ask user question
hyperplane_t *orthogonal_search(s_node_t *node, point_t *q, hyperplane_t *best)
{
    if (node->is_leaf)
    {
        int M = node->hyper.size();
        double ortho_value;
        if (best == NULL)
        {
            ortho_value = 0;
        }
        else
        {
            ortho_value = orthogonality(best->normal, q);
        }
        for (int i = 0; i < M; i++)
        {
            double v = orthogonality(node->hyper[i]->normal, q);
            if (v > ortho_value)
            {
                ortho_value = v;
                best = node->hyper[i];
            }
        }
        return best;
    }

    std::vector<s_node_t *> Q;
    spherical_cap_pruning(q, node->child, Q);
    std::stack<s_node_t *> S;
    for (int i = 0; i < Q.size(); i++)
    {
        S.push(Q[i]);
    }
    Q.clear();
    while (!S.empty())
    {
        s_node_t *t = S.top();
        double UB_t = upper_orthog(q, t);
        if (best == NULL || UB_t > orthogonality(best->normal, q))
        {
            best = orthogonal_search(t, q, best);
        }
        S.pop();
    }
    return best;
}


//@brief Find one of the top-k point by pairwise learning. Use the cos() of real u and estimated u as the accuracy.
//       The stop condition is that cos() should satisfy the given threshold
//@param original_set       The original dataset
//@param u                  The real utility vector
//@param k                  The threshold tok-k
point_t * Preference_Learning(FILE *wPtr, std::vector<point_t *> original_set, point_set_t *P0, int k, int &questions)
{
    int M, maxQcount = 100, testCount = 0, correctCount = 0;;
    //p_set: randomly choose 1000 points
    std::vector<point_t *> p_set;
    if (original_set.size() < 1000)
    {
        M = original_set.size();
        for (int i = 0; i < M; i++)
        {
            bool is_same = false;
            for (int j = 0; j < p_set.size(); j++)
            {
                if (is_same_point(p_set[j], original_set[i]))
                {
                    is_same = true;
                    break;
                }
            }
            if (!is_same)
            {
                p_set.push_back(original_set[i]);
            }
        }
        point_random(p_set);
    }
    else
    {
        int cco = 0;
        for (int i = 0; i < 1100; i++)
        {
            int ide = rand() % original_set.size();
            bool is_same = false;
            for (int j = 0; j < p_set.size(); j++)
            {
                if (is_same_point(p_set[j], original_set[ide]))
                {
                    is_same = true;
                    break;
                }
            }
            if (!is_same)
            {
                p_set.push_back(original_set[ide]);
                cco++;
                if (cco >= 1000)
                {
                    break;
                }
            }
        }
        point_random(p_set);
    }
    int dim = p_set[0]->dim;
    M = p_set.size();

    //the normal vectors
    std::vector<point_t *> V;
    for (int i = 0; i < dim; i++)
    {
        point_t *b = alloc_point(dim);
        for (int j = 0; j < dim; j++)
        {
            if (i == j)
            {
                b->coord[j] = 1;
            }
            else
            {
                b->coord[j] = 0;
            }
        }
        V.push_back(b);
    }

    //build a hyperplane for each pair of points
    std::vector<hyperplane_t *> h_set;
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            if (i != j && !is_same_point(p_set[i], p_set[j]))
            {
                hyperplane_t *h1 = alloc_hyperplane(p_set[i], p_set[j], 0);
                hyperplane_nomarlize(h1);
                h_set.push_back(h1);
                hyperplane_t *h2 = alloc_hyperplane(p_set[j], p_set[i], 0);
                hyperplane_nomarlize(h2);
                h_set.push_back(h2);
            }
        }
    }

    s_node_t *stree_root = alloc_s_node(dim);
    build_spherical_tree(h_set, stree_root);

    point_t *estimate_u;
    for (int i = 0; i < maxQcount; i++)
    {
        point_t *p;
        point_t *q;
        int p_idx, q_idx;
        estimate_u = find_estimate(V);
        point_nomarlize(estimate_u);
        //printf("|%lf|%lf|%lf|%lf|\n", estimate_u->coord[0], estimate_u->coord[1], estimate_u->coord[2], estimate_u->coord[3]);
        hyperplane_t *best = NULL;

        if (i % 2 == 1)
        {
            bool done = false;
            while (!done)
            {
                p_idx = rand() % M;
                q_idx = p_idx;
                while (p_idx == q_idx)
                {
                    q_idx = rand() % M;
                }
                if (!dominates(p_set[p_idx], p_set[q_idx]) && !dominates(p_set[q_idx], p_set[p_idx]))
                {
                    done = true;
                }
            }
            p = p_set[p_idx];
            q = p_set[q_idx];
        }
        else
        {
            best = orthogonal_search(stree_root, estimate_u, best);
            p = best->point1;
            q = best->point2;
        }


        int option = show_to_user(P0, p->id, q->id);
        if (i % 2 == 0) //training
        {
            point_t *pt = alloc_point(dim);
            if (option == 1)
            {
                for (int ia = 0; ia < dim; ia++)
                {
                    pt->coord[ia] = best->normal->coord[ia];
                }
            }
            else
            {
                for (int ia = 0; ia < dim; ia++)
                {
                    pt->coord[ia] = -best->normal->coord[ia];
                }
            }
            V.push_back(pt);
        }
        else // testing
        {
            testCount++;
            if ((option == 1 && dot_prod(estimate_u, p) >= dot_prod(estimate_u, q)) ||
                    (option == 2 && dot_prod(estimate_u, p) <= dot_prod(estimate_u, q)))
            {
                correctCount++;
            }
            double accuracy = ((double) correctCount) / testCount;
            printf("TotalQ:%d, TestQ: %d, Accuracy: %lf\n", i + 1, testCount, accuracy);
            if (testCount >= 5 && accuracy > 0.75)
            {
                break;
            }
        }
        release_point(estimate_u);
    }

    estimate_u = find_estimate(V);
    point_nomarlize(estimate_u);
    //Find the top-k point based on estimated utility vector
    std::vector<point_t *> top_current;
    find_top_k(estimate_u, original_set, top_current, k);
    release_point(estimate_u);
    while (h_set.size() > 0)
    {
        hyperplane_t *hh = h_set[h_set.size()-1];
        release_hyperplane(hh);
        h_set.pop_back();
    }

    printf("\n--------------------------------------------------------\n");
    printf("%s %10d \n", "No. of questions asked:", 2*testCount);
    printf("--------------------------------------------------------\n");
    printf("Recommended cars:\n");
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
    printf("---------------------------------------------------------\n");
    int i = rand()%k;
    printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[top_current[i]->id]->coord[0], P0->points[top_current[i]->id]->coord[1],
               P0->points[top_current[i]->id]->coord[2], P0->points[top_current[i]->id]->coord[3]);
    printf("---------------------------------------------------------\n");
    printf("Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
           "how bored you feel when you are asked with %d questions for this round \n"
           "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
           "that you feel the most bored and 1 denotes that you feel the least bored.)\n"
           "You answer: ", 2*testCount);
    int sat = 0;
    while(sat < 1 || sat > 10)
    {
        scanf("%d", &sat);
    }
    printf("\n\n");


    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%30s |%10d |\n", "Preference_learning", 2*testCount);
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "Recommended cars:\n");
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "PowerPS", "Used KM");
    fprintf(wPtr, "---------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P0->points[top_current[i]->id]->coord[0], P0->points[top_current[i]->id]->coord[1],
               P0->points[top_current[i]->id]->coord[2], P0->points[top_current[i]->id]->coord[3]);
    fprintf(wPtr, "---------------------------------------------------------\n");
    fprintf(wPtr, "Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                  "how bored you feel when you are asked with %d questions for this round \n"
                  "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                  "that you feel the most bored and 1 denotes that you feel the least bored.) %d\n\n\n", 2*testCount, sat);

    questions = 2*testCount;
    return top_current[i];
}