#include "../Others/used_function.h"
#include "../exp_stats.h"
#ifndef HDPI_PARTITION_FUNC
#define HDPI_PARTITION_FUNC

namespace HDPI_partfunc{

    /**
     * @brief Build all the partition(intersection of the halfspace), each partition corresponds to a top-1 point
     * @param p_set 				The set containing all the points which are possible to be the top-1 point
     *                              for some utility vectors
     * @param choose_item_points		The set containing points used to build choose_item
     * @param half_set_set			The returned partitions
     * @param considered_half_set	The set recorded all the partitions in the form of index
     */
    void construct_halfspace_set(std::vector<point_t *> &p_set, std::vector<point_t *> &choose_item_points,
                                std::vector<halfspace_set_t *> &half_set_set, std::vector<int> &considered_half_set);



    /**
     * @brief Used in PointPrune, Build all the partition(intersection of the halfspace), each partition corresponds to a top-1 point and store a copy
     * @param p_set 				The set containing all the points which are possible to be the top-1 point
     *                              for some utility vectors
     * @param choose_item_points		The set containing points used to build choose_item
     * @param half_set_set			The returned partitions
     * @param considered_half_set	The set recorded all the partitions in the form of index
     */
    void construct_halfspace_set_with_copy(std::vector<point_t *> &p_set, std::vector<point_t *> &choose_item_points, 
                                std::vector<point_t *> &choose_item_points_p, std::vector<halfspace_set_t *> &half_set_set, 
                                std::vector<halfspace_set_t *> &half_set_set_p, std::vector<int> &considered_half_set, 
                                std::vector<int> &considered_half_set_p);



    /**
     * @brief Build the choose_item table used for selecting the hyperplane(question) asked user
     * @param half_set_set 		All the partitions
     * @param p_set 			All the points which used to build choose_item
     * @param choose_item_set 	The returned choose_item
     * @return The index of the choose_item(hyperplane) which divide the half_set most evenly
     */
    int build_choose_item_table(std::vector<halfspace_set_t *> half_set_set, std::vector<point_t *> p_set,
                                std::vector<choose_item *> &choose_item_set);


    /**
     * @brief Based on the answer from the user, modify the choose_item table
     * @param choose_item_set 		The choose_item table which will be modified
     * @param half_set_set 			All the partitions
     * @param considered_half_set 	Based on the user's answer, the partitions still considered
     * @param H_num					The choose_item used for asking question
     * @param direction 			The user's answer(which side)
     *                              -true on the positive side
     *                              -false on the negative side
     * @return The choose_item(hyperplane) which divide the half_set most evenly
     */
    int modify_choose_item_table(std::vector<choose_item *> &choose_item_set,
                                std::vector<halfspace_set_t *> half_set_set, std::vector<int> &considered_half_set,
                                int H_num, bool direction);



    int choose_best_item(std::vector<choose_item*> choose_item_set);

}

#endif

