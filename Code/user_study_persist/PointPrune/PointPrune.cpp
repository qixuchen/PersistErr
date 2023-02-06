#include "PointPrune.h"
#include <sys/time.h>


/**
 * @brief Find the best halfspace that will be used for checking. the best halfspace prunes the largest number of items/partitions
 * 
 * @param choose_item_set       contain all the hyperplanes and their relation to the partitions
 * @param selected_halfspaces   contain all the halfspaces indicated by the user
 * @return                      return the index of the item that can possibly prunes the largest number of partitions
 */
int find_best_hyperplane(std::vector<choose_item*> choose_item_set, std::vector<halfspace_t *> &selected_halfspaces, 
                        point_t *&best_p1, point_t *&best_p2, double &ratio){
    
    
    int hs_size = selected_halfspaces.size();
    int item_size = choose_item_set.size();
    choose_item* best_item = NULL;
    int best_size = -1, best_item_index=-1;
    int select_idx = 0; //used to delete the best from selected_halfspaces


    if(hs_size<1){
        printf("%s\n", "Error: No halfspace candidates");
    }
    for(int i=0; i<hs_size; i++){
        hyperplane_t *h = alloc_hyperplane(selected_halfspaces[i]->normal, selected_halfspaces[i]->offset);
        choose_item* cur_item=NULL;
        int cur_size, j=0;
        while(j<item_size){
            if(is_same_hyperplane(h,choose_item_set[j]->hyper)){
                cur_item = choose_item_set[j];
                break;
            }
            j++;
        }
        if(cur_item==NULL){
            printf("%s\n", "Error: Cannot find the corresponding hyperplane");
        }
        if(h->normal->coord[0] == cur_item->hyper->normal->coord[0]){ //their normal points to the same direction
            cur_size = cur_item->positive_side.size();
        }
        else{
            cur_size = cur_item->negative_side.size();
        }
        if(cur_size>best_size){
            select_idx = i;
            best_size=cur_size;
            best_item=cur_item;
            best_item_index=j;
        }
        release_hyperplane(h);
    }
    if(best_size==-1){
        printf("%s\n", "Error: Cannot find the best item");
    }

    double num_partition = best_item->positive_side.size()+best_item->negative_side.size()
                            + best_item->intersect_case.size();
    ratio = ((double) best_size)/num_partition;

    halfspace_t *h_erase = selected_halfspaces[select_idx];
    best_p1=selected_halfspaces[select_idx]->point1;
    best_p2=selected_halfspaces[select_idx]->point2;
    selected_halfspaces.erase(selected_halfspaces.begin()+select_idx);
    release_halfspace(h_erase);
    return best_item_index;
}



int PointPrune_v2(std::vector<point_t *> p_set, point_set_t *P0, int checknum, int w, double *max, double *min)
{
    start_timer();
    int k = 1;
    int round = 0;
    //p_set_1 contains the points which are not dominated by >=1 points
    //p_set_k contains the points which are not dominated by >=k points
    //p_top_1 contains the points which are the convex points
    std::vector<point_t *> p_set_1, top;
    //onion(p_set, p_set_1, 1);
    int dim = p_set[0]->dim;
    find_top1(p_set, top);
    skyline_c(top, p_set_1);

    //half_set_set          contains all the partitions(intersection of halfspaces)
    //considered_half_set   contains all the possible partitions considered
    //choose_item_points    contains all the points used to construct hyperplanes(questions) (set C in the paper)
    //choose_item_set       contains all the hyperplanes(questions) which can be asked user
    //R_half_set            The utility range R
    std::vector<halfspace_set_t *> half_set_set, half_set_set_cp;
    std::vector<int> considered_half_set, considered_half_set_cp;   //[i] shows the index in half_set_set
    std::vector<point_t *> choose_item_points, choose_item_points_cp;
    std::vector<choose_item *> choose_item_set,  choose_item_set_cp;
    std::vector<halfspace_t*> selected_halfspaces;
    halfspace_set_t *R_half_set_cp = R_initial(dim);
    halfspace_set_t *R_half_set = R_initial(dim);
    get_extreme_pts_refine_halfspaces_alg1(R_half_set_cp);
    get_extreme_pts_refine_halfspaces_alg1(R_half_set);
    HDPI_partfunc::construct_halfspace_set_with_copy(p_set_1, choose_item_points, choose_item_points_cp, half_set_set, 
                                    half_set_set_cp, considered_half_set, considered_half_set_cp);

    HDPI_partfunc::build_choose_item_table(half_set_set, choose_item_points, choose_item_set);
    for(int i =0 ; i<choose_item_set.size();i++){
        choose_item * c_i=deepcopy_choose_item(choose_item_set[i]);
        choose_item_set_cp.push_back(c_i);
    }

    point_t* point_result = NULL, *true_point_result = NULL;
    halfspace_t *hy=NULL, *hy_cp=NULL;
    bool encounter_err = false, end_premature=false;

    //while(true_point_result==NULL)
    while(considered_half_set_cp.size() > w)
    {
        point_result = NULL;
        //index: the index of the chosen hyperplane(question)
        //p1:    the first point
        //p2:    the second point
        int index = HDPI_partfunc::choose_best_item(choose_item_set);
        // double v1 = dot_prod(u, choose_item_set[index]->hyper->point1);
        // double v2 = dot_prod(u, choose_item_set[index]->hyper->point2);
        point_t* p1 = choose_item_set[index]->hyper->point1;
        point_t* p2 = choose_item_set[index]->hyper->point2;
        point_t* user_choice = (show_to_user(P0->points[p1->id],P0->points[p2->id])==1) ? p1 : p2;
        round++;
        //start of phase 1
        //==========================================================================================================================================
        // while (point_result==NULL)
        while (considered_half_set.size() > w)
        {
            if (user_choice==p1)
            {
                hy = alloc_halfspace(p2, p1, 0, true);
                index = HDPI_partfunc::modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, true);
            }
            else
            {
                hy = alloc_halfspace(p1, p2, 0, true);
                index = HDPI_partfunc::modify_choose_item_table(choose_item_set, half_set_set, considered_half_set, index, false);
            }
            p1 = choose_item_set[index]->hyper->point1;
            p2 = choose_item_set[index]->hyper->point2;
            user_choice = (show_to_user(P0->points[p1->id],P0->points[p2->id])==1) ? p1 : p2;
            round++;

            //Find whether there exist point which is the topk point w.r.t any u in R
            R_half_set->halfspaces.push_back(hy);
            halfspace_t *hy2 = alloc_halfspace(hy->point1, hy->point2, hy->offset, hy->direction);
            selected_halfspaces.push_back(hy2);

            get_extreme_pts_refine_halfspaces_alg1(R_half_set);
            std::vector<point_t *> top_current;
            point_result = NULL;
            bool check_result = find_possible_topk(p_set, R_half_set, k, top_current);
            if (check_result)
            {
                point_result = check_possible_topk(p_set, R_half_set, k, top_current);
            }
            else if(considered_half_set.size() == 1){
                point_result=half_set_set[considered_half_set[0]]->represent_point[0];
            }
        }

        //=================================================================================================================================
        //End of phase 1


        //start of phase 2
        //==========================================================================================================================================
        encounter_err = false, end_premature=false;
        // while(true_point_result==NULL && selected_halfspaces.size()>0)
        while(considered_half_set_cp.size() > w && selected_halfspaces.size()>0)
        {
            // IMPORTANT: The order of point recorded in choose_item does not imply user preference
            // The user preference is indicated in selected_halfspaces: p2 > p1
            point_t *best_p1=NULL, *best_p2=NULL;
            double ratio=0;
            int best_index = find_best_hyperplane(choose_item_set_cp,selected_halfspaces, best_p1, best_p2, ratio);
            p1 = choose_item_set_cp[best_index]->hyper->point1;
            p2 = choose_item_set_cp[best_index]->hyper->point2;
    
            if(best_p1==p1){
                //user_choice = checking(u,p2,p1,theta,checknum);
                user_choice = checking_varyk(p2, p1, checknum, max, min)==1 ? p2 : p1;
                round += checknum;
            }
            else{
                //user_choice = checking(u,p1,p2,theta,checknum);
                user_choice = checking_varyk(p1, p2, checknum, max, min)==1 ? p1 : p2;
                round += checknum;
            }
            //printf("ratio %10f\n",ratio);
            if(user_choice!=best_p2){
                encounter_err = true;
            }
            if(encounter_err==true && ratio<0.15){
                //printf("enc err\n");
                end_premature=true;
            }

            if (user_choice==p1)
            {
                hy_cp = alloc_halfspace(p2, p1, 0, true);
                best_index = HDPI_partfunc::modify_choose_item_table(choose_item_set_cp, half_set_set_cp, considered_half_set_cp, best_index, true);
            }
            else
            {
                hy_cp = alloc_halfspace(p1, p2, 0, true);
                HDPI_partfunc::modify_choose_item_table(choose_item_set_cp, half_set_set_cp, considered_half_set_cp, best_index, false);
            }

            R_half_set_cp->halfspaces.push_back(hy_cp);
            get_extreme_pts_refine_halfspaces_alg1(R_half_set_cp);

            // IMPORTANT: Remove the halfspaces that are no longer helpful (lies on one side of R)
            int m=0;
            while(m<selected_halfspaces.size()){
                hyperplane_t *h = alloc_hyperplane(selected_halfspaces[m]->normal, selected_halfspaces[m]->offset);
                if(check_situation_accelerate(h,R_half_set_cp,0)!=0){
                    selected_halfspaces.erase(selected_halfspaces.begin()+m);
                }
                else{
                    m++;
                }
            }
            std::vector<point_t *> top_current;
            true_point_result = NULL;
            bool check_result = find_possible_topk(p_set, R_half_set_cp, k, top_current);
            if (check_result)
            {
                true_point_result = check_possible_topk(p_set, R_half_set_cp, k, top_current);
            }
            else if(considered_half_set_cp.size() == 1){
                true_point_result=half_set_set_cp[considered_half_set_cp[0]]->represent_point[0];
            }
            if(end_premature){
                break;
            }
        }
        if(true_point_result!=NULL){
            break;
        }

        //  re-initialize all data structures used in the inner loop with the record of the outer loop
        selected_halfspaces.clear();

        release_halfspace_set(R_half_set);
        R_half_set = deepcopy_halfspace_set(R_half_set_cp);

        choose_item_points.clear();
        for(int i=0; i<choose_item_points_cp.size();i++){
            choose_item_points.push_back(choose_item_points_cp[i]);
        }

        considered_half_set.clear();
        for(int i=0; i<considered_half_set_cp.size();i++){
            considered_half_set.push_back(considered_half_set_cp[i]);
        }
        half_set_set.clear();

        for(int i=0; i<half_set_set_cp.size();i++){
            halfspace_set_t *hs = deepcopy_halfspace_set(half_set_set_cp[i]);
            half_set_set.push_back(hs);
        }

        while(choose_item_set.size()>0){
            release_choose_item(choose_item_set[0]);
            choose_item_set.erase(choose_item_set.begin());
        }
        choose_item_set.clear();
        for(int i =0 ; i<choose_item_set_cp.size();i++){
            choose_item * c_i=deepcopy_choose_item(choose_item_set_cp[i]);
            choose_item_set.push_back(c_i);
        }
        //=================================================================================================================================
        //End of phase 2
    }
    stop_timer();

    //get the returned points
    vector<point_t *> points_return; 
    for(auto p : considered_half_set_cp){
        points_return.push_back(half_set_set_cp[p]->represent_point[0]);
    }
    print_result_list(P0, points_return);
    question_num += round;
    return_size += points_return.size();
    return 0;
}






