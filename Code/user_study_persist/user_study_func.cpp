#include "user_study_func.h"

using namespace std;

string vline = "--------------------------------------------------------";

void print_table_title(ostream &os){
    char buffer[1024];
    sprintf(buffer, "|%10s|%10s|%10s|%10s|%10s|", " ", "Price(USD)", "Year", "Power(PS)", "Used KM");
    cout << vline << endl;
    cout << buffer << endl;
}


string format_opt(const point_t *p, const int opt){
    char *buffer = new char[1024];
    string option = "Option " + to_string(opt);
    sprintf(buffer, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|", option.c_str(), p->coord[0], p->coord[1], 
                p->coord[2], p->coord[3]);
    return string(buffer);
}


string format_opt_scale(const point_t *p, const int opt, double scale){
    char *buffer = new char[1024];
    string option = "Option " + to_string(opt);
    sprintf(buffer, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|", option.c_str(), p->coord[0] * scale, p->coord[1] * scale, 
                p->coord[2] * scale, p->coord[3] * scale);
    return string(buffer);
}


void print_opt(ostream &os, point_set_t* P, const int idx, const int opt){
    cout << vline << endl;
    cout << format_opt(P->points[idx], opt) << endl;
}


void print_opt(ostream &os, const point_t *p, const int opt){
    cout << vline << endl;
    cout << format_opt(p, opt) << endl;
}


void print_opt_scale(ostream &os, point_set_t* P, const int idx, const int opt, double scale){
    cout << vline << endl;
    cout << format_opt_scale(P->points[idx], opt, scale) << endl;
}


void print_opt_scale(ostream &os, const point_t *p, const int opt, double scale){
    cout << vline << endl;
    cout << format_opt_scale(p, opt, scale) << endl;
}


int inconsistency_asking(point_set_t* P, int p_idx, int q_idx){
    int option = 0;
    cout << endl;
    cout << "This is the special round:" << endl;
    cout << "Please choose the car that you *dislike*:" << endl;
    print_table_title(cout);
    print_opt(cout, P, p_idx, 1);
    print_opt(cout, P, q_idx, 2);
    cout << vline << endl;
    while (option != 1 && option != 2){
        char buf[BUF_SIZE];
        cout << endl << "Your choice (choose the one that you *dislike*) (1 or 2): ";
        cin >> buf;
        option = atoi(buf);
    }
    return option;
}


int show_to_user(point_set_t* P, int p_idx, int q_idx)
{
    int option = 0;
    cout << endl;
    if(inconsistency.size()==0){
        int opt = inconsistency_asking(P, p_idx, q_idx);
        inconsistency.push_back( (opt == 1) ? make_pair(p_idx, q_idx) : make_pair(q_idx, p_idx));
        // cout << "Inconsistency added; " << p_idx << "\t" << q_idx << endl;
        return opt;
    }
    else{
        int ic1 = inconsistency[0].first, ic2 = inconsistency[0].second;
        if(ic1 == p_idx && ic2 == q_idx){ 
            // cout << "Auto answer;" << p_idx << "\t" << q_idx << endl;
            return 1;
        }
        else if(ic1 == q_idx && ic2 == p_idx){
            // cout << "Auto answer;" << q_idx << "\t" << p_idx << endl;
            return 2;
        }
        else{
            cout << "Please choose the car you favor more:" << endl;
            print_table_title(cout);
            print_opt(cout, P, p_idx, 1);
            print_opt(cout, P, q_idx, 2);
            cout << vline << endl;
            while (option != 1 && option != 2){
                char buf[BUF_SIZE];
                cout << endl << "Your choice (1 or 2): ";
                cin >> buf;
                option = atoi(buf);
            }
            return option;
        }
    }
}


int inconsistency_asking(const point_t* p1, const point_t* p2){
    int option = 0;
    cout << endl;
    cout << "This is the special round:" << endl;
    cout << "Please choose the car that you *dislike*:" << endl;
    print_table_title(cout);
    print_opt(cout, p1, 1);
    print_opt(cout, p2, 2);
    cout << vline << endl;
    while (option != 1 && option != 2){
        char buf[BUF_SIZE];
        cout << endl << "Your choice (choose the one that you *dislike*) (1 or 2): ";
        cin >> buf;
        option = atoi(buf);
    }
    return option;
}


int show_to_user(const point_t* p1, const point_t* p2)
{
    int option = 0;
    cout << endl;
    if(inconsistency.size()==0){
        int opt = inconsistency_asking(p1, p2);
        inconsistency.push_back( (opt == 1) ? make_pair(p1->id, p2->id) : make_pair(p2->id, p1->id) );
        // cout << "Inconsistency added; " << p1->id << "\t" << p2->id << endl;
        return opt;
    }
    else{
        int ic1 = inconsistency[0].first, ic2 = inconsistency[0].second;
        if(ic1 == p1->id && ic2 == p2->id){
            // cout << "Auto answer;" << p1->id << "\t" << p2->id << endl;
            return 1;
        }
        else if(ic1 == p2->id && ic2 == p1->id){
            // cout << "Auto answer;" << p2->id << "\t" << p1->id << endl;
            return 2;
        }
        else{
            cout << "Please choose the car you favor more:" << endl;
            print_table_title(cout);
            print_opt(cout, p1, 1);
            print_opt(cout, p2, 2);
            cout << vline << endl;
            while (option != 1 && option != 2){
                char buf[BUF_SIZE];
                cout << endl << "Your choice (1 or 2): ";
                cin >> buf;
                option = atoi(buf);
            }
            return option;
        }
    }
}


void print_result_list(point_set_t* P, const vector<point_t *> &point_return){
    cout << endl;
    cout << "These are the cars recommended by this algorithm:" << endl << endl;
    print_table_title(cout);
    for(int i=0; i < point_return.size(); i++){
        print_opt(cout, P, point_return[i]->id, i+1);
    }
    cout<< vline <<endl;
}


void print_result_list(point_set_t* P, const vector<int> &ids){
    cout << endl;
    cout << "These are the cars recommended by this algorithm:" << endl << endl;
    print_table_title(cout);
    for(int i=0; i < ids.size(); i++){
        print_opt(cout, P, ids[i], i+1);
    }
    cout<< vline <<endl;
}


int alg_top1_select(const vector<point_t *> &point_return){
    cout << endl << "Please select your favorite one among the recommended cars." << endl;
    int option = 0, opt_range = point_return.size();
    while (option <= 0 || option > opt_range){
        char buf[BUF_SIZE];
        cout << endl << "Your choice (1 to " << opt_range << "): ";
        cin >> buf;
        option = atoi(buf);
    }
    return option-1;

}


/**    @brief write the result of an algorithm to a file in directory logs
 * 
 *             format:
 *                      # of used questions
 *                      id of the best point
 *                      average processing time
 *                      # of points recommended by algorithm (return size)
 *                      id of recommended point 1
 *                      id of recommended point 2
 *                      id of recommended point 3
 *                      ......
 * 
*/
void write_results_to_file(const int alg_id, const vector<point_t *> &point_return, const int best_idx){
    ofstream ofs;
    ofs.open("../logs/" + file_names[alg_id], ofstream::out);
    ofs << question_asked_list[alg_id] << endl;
    ofs << proc_time_list[alg_id] << endl;
    ofs << point_return[best_idx]->id << endl;
    ofs << point_return.size()<<endl;
    for(auto p: point_return){
        ofs << p->id << endl;
    }
    ofs.close();
}


/**    @brief write the confidence region each returned point belongs to
 */
void write_cf_info(const int alg_id, const vector<int> &cr_belong){
    ofstream ofs;
    ofs.open(string("../logs/") + "conf_reg" + file_names[alg_id], ofstream::out);
    for(auto cr: cr_belong){
        ofs << cr << endl;
    }
    ofs.close();
}


int show_to_user_scale(point_set_t* P, int p_idx, int q_idx)
{
    double scale = (double)(90 + (rand()%10)) / 100; //generate a random number from 0.91-0.99
    int option = 0;
    cout << endl;
    cout << "Please choose the car you favor more:" << endl;
    print_table_title(cout);
    print_opt_scale(cout, P, p_idx, 1, scale);
    print_opt_scale(cout, P, q_idx, 2, scale);
    cout << vline << endl;
    while (option != 1 && option != 2){
        char buf[BUF_SIZE];
        cout << endl << "Your choice (1 or 2): ";
        cin >> buf;
        option = atoi(buf);
    }
    return option;
}

int show_to_user_scale(const point_t* p1, const point_t* p2)
{
    double scale = (double)(90 + (rand()%10)) / 100; //generate a random number from 0.91-0.99
    int option = 0;
    cout << endl;
    cout << "Please choose the car you favor more:" << endl;
    print_table_title(cout);
    print_opt_scale(cout, p1, 1, scale);
    print_opt_scale(cout, p2, 2, scale);
    cout << vline << endl;
    while (option != 1 && option != 2){
        char buf[BUF_SIZE];
        cout << endl << "Your choice (1 or 2): ";
        cin >> buf;
        option = atoi(buf);
    }
    return option;
}


void print_alg_start(const int alg_num){
    cout << vline << endl;
    cout << "                  Algorithm " << alg_num+1 << " begins" << endl;
    cout << vline << endl;
}


void print_alg_end(const int alg_num){
    cout << vline << endl;
    cout << "                  Algorithm " << alg_num+1 << " ends" << endl;
    cout << vline << endl << endl << endl << endl;
}


void print_result(FILE* wPtr, const char *alg_name, const int num_of_question, const point* p, int alg_num){
    // print result to user
    printf("\n=========== Recommendation of algorithm %d =================\n", alg_num);
    printf("\n--------------------------------------------------------\n");
    printf("%s %10d \n", "No. of questions asked:", num_of_question);
    printf("--------------------------------------------------------\n");
    printf("Recommended car:\n");
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "Power(PS)", "Used KM");
    printf("---------------------------------------------------------\n");
    printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", p->coord[0],p->coord[1],
                p->coord[2], p->coord[3]);
    printf("---------------------------------------------------------\n");


    // print result to file 
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%30s |%10d |\n", alg_name, num_of_question);
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "Recommended car:\n");
    fprintf(wPtr, "--------------------------------------------------------\n");
    fprintf(wPtr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "Power(PS)", "Used KM");
    fprintf(wPtr,"---------------------------------------------------------\n");
    fprintf(wPtr,"|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", p->coord[0], p->coord[1],
               p->coord[2], p->coord[3]);
    fprintf(wPtr,"---------------------------------------------------------\n");

};



/**
 * @brief Display cars in the final list
 * 
 * @param final_list 
 */
void display_final_list(point_set_t* P, std::vector<int> final_list){
    for(int i = 0; i <final_list.size(); i++){
        cout << "Recommended car No. " << i+1 << ":" << endl;
        print_table_title(cout);
        print_opt(cout, P, final_list[i], i+1);
        cout << vline << endl;
    }
}


/**
 * @brief Ask the user to decide the favorite car in all returned cars
 * 
 * @param l_size Size of the final list
 * @return int 
 */
int ask_favorite_item(int l_size){
    int favorite = 0;
    printf("Please give a number from 1 to %d to indicate which car\n"
            "you like the most in the recommended cars listed above \n"
            "(e.g., 2 means car No. 2 is your favorite car)", l_size);
    while(favorite <1 || favorite > l_size){
        char buf[BUF_SIZE];
        cout << endl << "Your choice (1 - " << l_size << "): ";
        cin >> buf;
        favorite = atoi(buf);
    }
    return favorite-1;
}


int confirm_favorite_item(point_set_t* P, std::vector<int> final_list, int possible_best_idx){
    std::vector<int> compete_points;
    for(int i = 0; i < final_list.size(); i++){
        if(i == possible_best_idx) continue;
        int ans = show_to_user(P, final_list[possible_best_idx], final_list[i]);
        if(ans != 1){
            compete_points.push_back(i);
        }
    }
    compete_points.push_back(possible_best_idx);
    if(compete_points.size() <= 1) return possible_best_idx;
    else{
        int cur_best_idx = 0;
        for(int i = 1; i < compete_points.size(); i++){
            int ans = show_to_user(P, final_list[compete_points[cur_best_idx]], final_list[compete_points[i]]);
            if(ans != 1) cur_best_idx = i;
        }
        return compete_points[cur_best_idx];
    }
}

std::set<int> find_dissatisfactory_lists(point_set_t *P, int best_pid){
    std::set<int> dissat_lists;
    for(int i = 0; i < TOT_ALG_COUNT; i++){
        std::vector<int> &vec = recommendation_list[i];
        if(std::find(vec.begin(), vec.end(), best_pid) == vec.end()){
            dissat_lists.insert(i);
        }
    }
    return dissat_lists;
}


int ask_dissat_score(point_set_t *P,  vector<int> &ids){
    print_result_list(P, ids);
    int ans = 0;
    cout << "Please give a dissatisfaction score (1 - 10) on this result list." << endl;
    while (ans < 1 || ans > 10){
        char buf[BUF_SIZE];
        cout << endl << "Your answer (1 - 10): ";
        cin >> buf;
        ans = atoi(buf);
    }
    return ans;
}

void write_summary(){
    ofstream ofs;
    ofs.open("../logs/summary", ofstream::out);
    for(int i = 0; i < TOT_ALG_COUNT; i++){
        ofs << question_asked_list[i] << "\t";
        ofs << proc_time_list[i] << "\t";
        ofs << dissat_score_list[i] << "\t" << endl;
    }
    ofs.close();
}


/**
 * @brief Aske the dissatisfactory level of a car
 * 
 * @param P 
 * @param car 
 * @return int 
 */
int ask_dissat_score(const point_set_t* P, int car_id){
    printf("\n");
    printf("Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate how \n"
            " **dissatisfied** you feel for the following car recommended to you. \n"
            "(1 means the least dissatisfied and 10 means the most dissatisfied): \n");
    printf("--------------------------------------------------------\n");
    printf("Recommended car:\n");
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "Power(PS)", "Used KM");
    printf("---------------------------------------------------------\n");
    printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P->points[car_id]->coord[0], P->points[car_id]->coord[1],
                P->points[car_id]->coord[2], P->points[car_id]->coord[3]);
    printf("---------------------------------------------------------");
    int dissat = 0;
    while(dissat < 1 || dissat > 10)
    {
        char buf[BUF_SIZE];
        printf("\nYou answer (from 1 to 10): ");
        fgets(buf, BUF_SIZE, stdin);
        dissat = atoi(buf);
    }
    printf("\n\n");
    return dissat;
}


/**
 * @brief Record the satisfactory level of the user. The satisfacory level ranges from 1-10
 * 
 * @param P                             point set
 * @param num_questions_asked           Total # questions asked by the algorithm
 * @param p_result_id                   Id of the top-1 point selected by alg
 * @param alg_name                      name of the alg
 * @param w_ptr                         file pointer
 * @return int 
 */
int ask_satisfactory_level(const point_set_t* P, const int num_questions_asked, const int p_result_id, const char *alg_name, FILE *w_ptr){

    printf("\n--------------------------------------------------------\n");
    printf("%s %10d \n", "No. of questions asked:", num_questions_asked);
    printf("--------------------------------------------------------\n");
    printf("Recommended cars:\n");
    printf("--------------------------------------------------------\n");
    printf("|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "Power(PS)", "Used KM");
    printf("---------------------------------------------------------\n");
    printf("|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P->points[p_result_id]->coord[0], P->points[p_result_id]->coord[1],
                P->points[p_result_id]->coord[2], P->points[p_result_id]->coord[3]);
    printf("---------------------------------------------------------\n");
    printf("Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
            "how bored you feel when you are asked with %d questions for this round \n"
            "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
            "that you feel the most bored and 1 denotes that you feel the least bored.)\n"
            "You answer: ", num_questions_asked);
    int sat = 0;
    while(sat < 1 || sat > 10)
    {
        char buf[BUF_SIZE];
        printf("\nYou answer (from 1 to 10): ");
        fgets(buf, BUF_SIZE, stdin);
        sat = atoi(buf);
    }
    printf("\n\n");

    fprintf(w_ptr, "--------------------------------------------------------\n");
    fprintf(w_ptr, "|%30s |%10d |\n", alg_name, num_questions_asked);
    fprintf(w_ptr, "--------------------------------------------------------\n");
    fprintf(w_ptr, "Recommended cars:\n");
    fprintf(w_ptr, "--------------------------------------------------------\n");
    fprintf(w_ptr, "|%10s|%10s|%10s|%10s|%10s|\n", " ", "Price(USD)", "Year", "Power(PS)", "Used KM");
    fprintf(w_ptr, "---------------------------------------------------------\n");
    fprintf(w_ptr, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|\n", "Car", P->points[p_result_id]->coord[0], P->points[p_result_id]->coord[1],
                P->points[p_result_id]->coord[2], P->points[p_result_id]->coord[3]);
    fprintf(w_ptr, "---------------------------------------------------------\n");
    fprintf(w_ptr, "Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate \n"
                    "how bored you feel when you are asked with %d questions for this round \n"
                    "in order to obtain one of your 20 most favorite cars (Note: 10 denotes\n"
                    "that you feel the most bored and 1 denotes that you feel the least bored.) %d\n\n\n", num_questions_asked, sat);
    
    return 0;
}



void record_to_file(FILE *wPtr, int *records, int r_size){
    for(int i = 0; i < r_size; i++){
        fprintf(wPtr, "%d ", records[i]);
    }
    fprintf(wPtr, "\n");
}

