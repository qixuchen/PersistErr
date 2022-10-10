#include "approx_func.h"

using std::vector; 
using std::string;

/** @brief      Small checking function, sum of point coordinates
 */    
double coord_sum(point_t * p){
    if(p == 0) return 0;
    int dim = p->dim;
    double sum = 0;
    for(int i = 0; i < dim; i++){
        sum += p->coord[i];
    }
    return sum;
}


int check_ext_pt_validity(std::vector<point_t *> &aggre_ext_pts){
    for(int i = 0; i < aggre_ext_pts.size(); i++){
        if(coord_sum(aggre_ext_pts[i]) != 1) return i;
    }
    return -1;
}


/** @brief      Compute the number of fragments in a Region r
 */
vector<point_t *> extract_frags(const Enclosure &e){
    vector<point_t *> results;
    if(e.frag_set.size() != 0){ // debug purpose
        for(auto f: e.frag_set) results.push_back(f.first);
    }
    return results;
}


/** @brief      Find the points still not pruned
 */
std::set<point_t *> enclosure_compute_considered_set(const std::vector<Enclosure> &enclosures){
    int k = enclosures.size() - 1;
    std::set<point_t *> considered_points;
    for(int i=0; i <= k; i++){
        std::vector<point_t *> point_set = extract_frags(enclosures[i]);
        for(auto p : point_set) considered_points.insert(p);
    }
    return considered_points;
}


/** 
 * @brief          Put points in p_set in to p_filename 
*/
void record_point_set(const std::vector<point_t *> &p_set, const string &p_filename){
    ofstream p_file(p_filename);
    if(!p_file.is_open()){
        cout << __func__ << "Error when opening" << p_filename << endl;
    }
    int dim = p_set[0]->dim, size = p_set.size();
    p_file << dim << endl;
    p_file << size + 1 << endl;  // + 1 since we will add origin
    //add origin
    for (int j = 0; j < dim; j++)
    {
        p_file << 0 << "   ";
    }
    p_file << endl;
    for(int i = 0; i < size; i++){
        for (int j = 0; j < dim; j++)
        {
            p_file << p_set[i]->coord[j] << "   ";
        }
        p_file << endl;
    }
    p_file.close();
}

/**
 * @brief       Read the resulting halfspaces from h_filename, store them in a halfspace_set
*/
halfspace_set_t * read_convh_hyperplanes(const string &h_filename){
    ifstream h_file(h_filename);
    if(!h_file.is_open()) cout << __func__ << "Error when opening" << h_filename << endl;
    int dim = 0, size = 0;
    h_file >> dim >> size;
    dim -= 1;   // the last dimension is the offset

    halfspace_set_t *hss_ptr = alloc_halfspace_set(dim); // LEAK
    for(int i = 0; i < size; i++){
        halfspace_t *hs = alloc_halfspace(dim); // LEAK
        for(int j = 0; j < dim; j++) h_file >> hs->normal->coord[j];
        h_file >> hs->offset;
        if(abs(hs->offset) < OFFSET_THRESHOLD){ // only interested in those pass through the origin
            hss_ptr->halfspaces.push_back(hs);
        }
        else{
            release_halfspace(hs);
        }
    }
    h_file.close();
    return hss_ptr;
}

/**
 * @brief       Compute the convex hull of p_set and return hss_ptr pointing to the resulting halfspace_set
 *              1, put points in p_set in to output/point_set.txt
 *              2, call qconvex using std::system
 *              3, read the resulting halfspaces from output/conv_hyper.txt, store them in a halfspace_set
*/
halfspace_set_t * compute_convh_hyperplanes(const std::vector<point_t *> &p_set){
    if(p_set.size() == 0){
        cout << __func__ << ": p_set has no elements" << endl;
        return 0;
    }
    string p_filename = "../output/point_set.txt";
    string h_filename = "../output/conv_hyper.txt";
    record_point_set(p_set, p_filename);

    // call qhull to compute the convex hull hyperplanes and write into output/conv_hyper.txt
    string conv_command = "qconvex n QV0 < " + p_filename + " > " + h_filename;
    std::system(conv_command.c_str());

    return read_convh_hyperplanes(h_filename);
}


/**
 * @brief      Given a halfspace hs, get the other halfspace
*/
halfspace_t *get_hs_counterpart(halfspace_t *hs){
    return alloc_halfspace(hs->point2, hs->point1, hs->offset, hs->direction);
}


bool check_enclosure_emptiness(halfspace_set_t * half_set, halfspace_t *h){
    if(half_set == 0) return true;
    double max_dist = -1;
    for(auto p : half_set->ext_pts){
        if(dot_prod(p, h->normal) > max_dist) max_dist = dot_prod(p, h->normal);
    }
    return (max_dist > 0.005) ? false : true; // this threshold is set since all ext points
                                            // coordinates sums to 10,000
}