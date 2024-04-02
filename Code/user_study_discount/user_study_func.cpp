#include "user_study_func.h"

using namespace std;

string vline = "-------------------------------------------------------------";

string vline_v2 = "------------------------------------------------------------------------";

void enter_recover(){
    string recover;
    cout << "Press Enter to Continue.";
    getline(cin, recover);
    if(recover.size() == 1 && recover[0] == 'r'){
        cout << endl << "Entering recover mode " << endl;
        int jump_id = 0; 
        while (jump_id < 1 || jump_id > 6){
            string buf;
            cout << "Enter the algorithm id you want to jump to:";
            getline(cin, buf);
            if(is_number(buf)){
                jump_id = stoi(buf);
            }
            handle_recover(jump_id);
        }
        jump = jump_id;
    }
    cout << endl;
}

void handle_recover(int jump_id){
    for(int i=0; i < jump_id - 1; i++){
        ifstream ifs;
        ifs.open("../logs/" + file_names[i], ifstream::in);
        string ques, proc_time, best_pid, return_size, recom_id; 
        getline(ifs, ques);
        question_asked_list[i] = stoi(ques);
        getline(ifs, proc_time);
        proc_time_list[i] = stod(proc_time);
        getline(ifs, best_pid);
        best_pid_list[i] = stoi(best_pid);
        getline(ifs, return_size);
        int size = stoi(return_size);
        return_size_list[i] = size;
        for(int j = 0; j < size; j++){
            getline(ifs, recom_id);
            recommendation_list[i].push_back(stoi(recom_id));
        }
    }
}

void enter_to_continue(){
    cout << "Press Enter to Continue.";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cout << endl;
    cout << "-----------------------------------------------------------------------------------------------" << endl;
}

void print_vline(int display_mode){
    if(display_mode == 1){
        cout << vline_v2 << endl;
    }
    else{
        cout << vline << endl;
    }
}

bool is_number(const string& s){
    if(s.size() == 0) return false;
    for (char const &ch : s) {
        if (std::isdigit(ch) == 0)
            return false;
    }
    return true;
}

void pre_check(){
    cout << "---------------------------------Welcome to our user study----------------------------------------" << endl;
    string wid, pnumber, ams;
    cout << endl << "Please tell me your worker ID: ";
    getline(cin, wid);
    cout << endl << "Please tell me what is your participant number (enter 1 - 30): ";
    getline(cin, pnumber);
    cout << endl << "Please tell me which country is Amsterdam in: " << endl;
    cout << "1. Germany" << endl;
    cout << "2. Norway" << endl;
    cout << "3. France" << endl;
    cout << "4. Netherlands" << endl;
    cout << "5. Iceland" << endl;
    cout << endl << "Your choice (enter 1 - 5):";
    getline(cin, ams);

    ofstream ofs;
    ofs.open("../logs/misc/workid.txt", ofstream::out);
    ofs << wid << endl;
    ofs.close();

    ofs.open("../logs/misc/pnumber.txt", ofstream::out);
    ofs << pnumber << endl;
    ofs.close();

    ofs.open("../logs/misc/ams.txt", ofstream::out);
    ofs << ams << endl;
    ofs.close();

    cout << endl << endl;
}

void intro(){
    // pre_check();
    cout << endl;
    cout << "-------------------------Welcome to the recommending Airbnb system--------------------------------" << endl;
    cout << "Imagine that you are planning a trip to a new conuntry and want to rent an Airbnb. There are " << endl
           << "thousands of Airbnbs on the website." << endl << endl;
    cout << "Since renting a good Airbnb can greatly affect the happiness of your journey, you will want to" << endl
           << "find your favorite Airbnb." << endl << endl;
    cout << "In our research project, we want to study several recommendation algorithms and see whether they" << endl
           << "can help you find your favorite Airbnb under several different settings." << endl << endl;
    // enter_to_continue();
    enter_recover();
    cout << endl;
}

void part1_info(){
    cout << "===============================================================================================" << endl << endl;
    cout << "                                     Beginning of Part 1" << endl << endl;
    cout << "===============================================================================================" << endl << endl;
    cout << "In Part 1, there are 3 algorithms and each will ask you to compare some Airbnbs." << endl << endl;
    
    cout << "Each Airbnb is described with the following 5 atrributes:" << endl
            << "    Labeled Price       The price per night **before** applying the discount" << endl
            << "                        Range from USD 20 - 1500" << endl
            << "    Cleanliness         Average rating on cleanliness of the Airbnb" << endl
            << "                        Range from 0 - 5" << endl
            << "    Location            Average rating on location convenience" << endl
            << "                        Range from 0 - 5" << endl
            << "    Review PM           Average number of reviews in each month" << endl 
            << "                        Range from 0 - 8.82" << endl 
            << "    Discount            The discount rate" << endl 
            << "                        Range from 0% - 60%" << endl << endl;

    cout << "Note that the price shown here is the \"labeled price\" before the discount rate is considered." << endl;
    cout << "The price that you actually need to pay for the rent is called the \"final price\"." << endl << endl;
    cout << "    final price = labeled price x (100% - discount rate)" << endl << endl;
    cout << "For example, given an option with a labelled price USD 300 and a discount rate 30%," << endl;
    cout << "The final price that you actually need to pay for it is 300 x (100% - 30%) = 210 USD." << endl << endl;

    enter_to_continue();
    cout << endl;

    cout << "In each question, you will be presented 2 Airbnb options." << endl;
    cout << "For example, they may look like follows." << endl << endl;

    cout << "------------------------------------------------------------------------" << endl;
    cout << "|          |  Labeled Price|     Clean|  Location| Review PM|  Discount|" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|  Option 1|            255|      4.83|      4.83|      4.00|       30%|" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|  Option 2|            260|      4.77|      4.93|      3.46|       30%|" << endl;
    cout << "------------------------------------------------------------------------" << endl << endl;

    cout << "Your task is to pick the option that you favor more. For example, if you think Airbnb 1" << endl
            << "is more preferred to Airbnb 2, You could pick option 1 by entering 1." << endl << endl;

    enter_to_continue();
    cout << endl;

    cout << "By the end of each algorithm, a list of Airbnbs will be recommended by this algorithm." << endl;
    cout << "For example, a recommendation list may look like follows:" << endl << endl;

    cout << "------------------------------------------------------------------------" << endl;
    cout << "|          |  Labeled Price|     Clean|  Location| Review PM|  Discount|" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|  Option 1|            281|      3.93|      4.70|      0.78|       40%|" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|  Option 2|             85|      4.93|      4.83|      3.41|        5%|" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|  Option 3|            101|      4.67|      4.80|      7.46|        5%|" << endl;
    cout << "------------------------------------------------------------------------" << endl << endl;

    cout << "You will need to select 1 Airbnb from the recommendation list that you think is the best." << endl;
    cout << "For example, enter 3 if you think option 3 is your favorite among the recommended Airbnbs." << endl << endl;
    enter_to_continue();
    cout << endl;
}


void part1_end(){
    cout << "===============================================================================================" << endl << endl;
    cout << "                                        End of Part 1" << endl << endl;
    cout << "===============================================================================================" << endl << endl;
    enter_to_continue();
    cout << endl << endl;
}

void part2_begin(){
    cout << "===============================================================================================" << endl << endl;
    cout << "                                     Beginning of Part 2" << endl << endl;
    cout << "===============================================================================================" << endl << endl;
}

void part2_info_v1_p1(){

    cout << "In Part 2, there are two tasks." << endl << endl;

    cout << "The first task is to decide your favorite Airbnb among the Airbnbs you selected" << endl
            << "at the end of each algorithm in Part 1." << endl << endl;
    
    cout << "For these options, we have transformed them to only show their \"final price\"." << endl;
    cout << "i.e., the price that you actually need to pay for it." << endl << endl;

    enter_to_continue();
    cout << endl;
}

void part2_info_v1_p2(){
    cout << "------------------------------------------------------------------------" << endl << endl;
    cout << "Next, there is another task for this part." << endl << endl;
    enter_to_continue();
    cout << endl;
}

void part2_info_v2(){

    cout << "In Part 2, there is one task." << endl << endl;
    enter_to_continue();
    cout << endl;
}

void part2_second_task(){
    cout << "We notice that you selected the following option as the best choice in Algorithm 2." << endl << endl;
}



void part2_end(){
    cout << "===============================================================================================" << endl << endl;
    cout << "                                        End of Part 2" << endl << endl;
    cout << "===============================================================================================" << endl << endl << endl << endl;
}


void end(){
    cout << "===============================================================================================" << endl << endl;
    cout << "                                     End of User Study" << endl << endl;
    cout << "===============================================================================================" << endl << endl;
    cout << "Thank you for taking the user study." << endl << endl;
    // cout << "Your completion code is: C5EPB200" << endl;
}


void alg3_info(){
    cout << "In this algorithm, we will do something different." << endl << endl;
    
    cout << "In previous algorithms, we displayed \"labeled price\" and \"discount rate\"." << endl
            <<"For example: " << endl << endl;
    
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|          |  Labeled Price|     Clean|  Location| Review PM|  Discount|" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|  Option 1|            250|      4.83|      4.83|      4.00|       20%|" << endl;
    cout << "------------------------------------------------------------------------" << endl;
    cout << "|  Option 2|            300|      4.77|      4.93|      3.46|       30%|" << endl;
    cout << "------------------------------------------------------------------------" << endl << endl;

    enter_to_continue();
    cout << endl;

    cout << "In this algorithm, we will not display \"labeled price\" and \"discount rate\"." << endl;
    cout << "Instead, we will display the \"final price\", which is the price that you actually need to pay." << endl << endl;
    cout << "    final price = labeled price x (100% - discount rate)" << endl << endl;
    cout << "For example, the above two options will be transformed to the following." << endl << endl;

    cout << "-------------------------------------------------------------" << endl;
    cout << "|          |    Final Price|     Clean|  Location| Review PM|" << endl;
    cout << "-------------------------------------------------------------" << endl;
    cout << "|  Option 1|            200|      4.83|      4.83|      4.00|" << endl;
    cout << "-------------------------------------------------------------" << endl;
    cout << "|  Option 2|            210|      4.77|      4.93|      3.46|" << endl;
    cout << "-------------------------------------------------------------" << endl << endl;

    cout << "Your task remains the same. For each question, please pick the option that you prefer." << endl << endl;
    enter_to_continue();
    cout << endl;
}


void print_table_title(ostream &os){
    char buffer[1024];
    sprintf(buffer, "|%10s|%10s|%10s|%10s|%10s|", " ", "Price(USD)", "Clean", "Location", "Review PM");
    cout << vline << endl;
    cout << buffer << endl;
}

void print_table_title(ostream &os, int display_mode){
    char buffer[1024];
    if(display_mode == 1){
        sprintf(buffer, "|%10s|%15s|%10s|%10s|%10s|%10s|", " ", "Labeled Price", "Clean", "Location", "Review PM", "Discount");
    }
    else{
        sprintf(buffer, "|%10s|%15s|%10s|%10s|%10s|", " ", "Final Price", "Clean", "Location", "Review PM");
    }
    print_vline(display_mode);
    cout << buffer << endl;
}


string format_opt(const point_t *p, const int opt){
    char *buffer = new char[1024];
    string option = "Option " + to_string(opt);
    // sprintf(buffer, "|%10s|%10.0f|%10.0f|%10.0f|%10.0f|", option.c_str(), p->coord[0], p->coord[1], 
    //             p->coord[2], p->coord[3]);
    sprintf(buffer, "|%10s|%10.0f|%10.2f|%10.2f|%10.2f|", option.c_str(), p->coord[0], p->coord[1], 
                p->coord[2], p->coord[3]);
    return string(buffer);
}

string format_opt(const point_t *p, const int opt, int display_mode){
    char *buffer = new char[1024];
    string option;
    if(opt > 0){
        option = "Option " + to_string(opt);
    }
    else{ // do not display option numbering
        option = "";
    }
    if(display_mode==1){
        sprintf(buffer, "|%10s|%15d|%10.2f|%10.2f|%10.2f|%9d%%|", option.c_str(), static_cast<int>(p->coord[0]), p->coord[1], 
                p->coord[2], p->coord[3], int(p->coord[4]));
    }
    else{
        sprintf(buffer, "|%10s|%15d|%10.2f|%10.2f|%10.2f|", option.c_str(), static_cast<int>(ceil(p->coord[0] * (100 - p->coord[4]) / 100)), p->coord[1], 
                p->coord[2], p->coord[3]);
    }
    return string(buffer);
}


void print_opt(ostream &os, point_set_t* P, const int idx, const int opt){
    cout << vline << endl;
    cout << format_opt(P->points[idx], opt) << endl;
}

void print_opt(ostream &os, point_set_t* P, const int idx, const int opt, int display_mode){
    print_vline(display_mode);
    cout << format_opt(P->points[idx], opt, display_mode) << endl;
}

void print_opt(ostream &os, const point_t *p, const int opt){
    cout << vline << endl;
    cout << format_opt(p, opt) << endl;
}

int show_to_user(point_set_t* P, int p_idx, int q_idx)
{
    int option = 0;
    cout << endl;
    cout << "Please choose the Airbnb you favor more:" << endl;
    print_table_title(cout);
    print_opt(cout, P, p_idx, 1);
    print_opt(cout, P, q_idx, 2);
    cout << vline << endl;
    while (option != 1 && option != 2){
        string buf;
        cout << endl << "Your choice (choose the one you like) (enter 1 or 2): ";
        getline(cin, buf);
        if(is_number(buf)){
            option = stoi(buf);
        }
    }
    displayed_questions.push_back(make_pair(p_idx, q_idx));
    user_choices.push_back(option);
    return option;
}

int show_to_user(point_set_t* P, int p_idx, int q_idx, int display_mode)
{
    int option = 0;
    cout << endl;
    cout << "Please choose the Airbnb you favor more:" << endl;
    print_table_title(cout, display_mode);
    print_opt(cout, P, p_idx, 1, display_mode);
    print_opt(cout, P, q_idx, 2, display_mode);
    print_vline(display_mode);
    while (option != 1 && option != 2){
        string buf;
        cout << endl << "Your choice (choose the one you like) (enter 1 or 2): ";
        getline(cin, buf);
        if(is_number(buf)){
            option = stoi(buf);
        }
    }
    displayed_questions.push_back(make_pair(p_idx, q_idx));
    user_choices.push_back(option);
    return option;
}


int show_to_user(const point_t* p1, const point_t* p2)
{
    int option = 0;
    cout << endl; 
    cout << "Please choose the Airbnb you favor more:" << endl;
    print_table_title(cout);
    print_opt(cout, p1, 1);
    print_opt(cout, p2, 2);
    cout << vline << endl;
    while (option != 1 && option != 2){
        string buf;
        cout << endl << "Your choice (choose the one you like) (enter 1 or 2): ";
        getline(cin, buf);
        if(is_number(buf)){
            option = stoi(buf);
        }
    }  
    displayed_questions.push_back(make_pair(p1->id, p2->id));
    user_choices.push_back(option);
    return option;
}

void print_result_list(point_set_t* P, const vector<point_t *> &point_return){
    cout << endl;
    cout << vline << endl << endl;
    cout << "These are the Airbnbs recommended by this algorithm:" << endl;
    print_table_title(cout);
    for(int i=0; i < point_return.size(); i++){
        print_opt(cout, P, point_return[i]->id, i+1);
    }
    cout<< vline <<endl;
}

void print_result_list(point_set_t* P, const vector<point_t *> &point_return, int display_mode){
    cout << endl;
    print_vline(display_mode);
    cout << endl;
    cout << "These are the Airbnbs recommended by this algorithm:" << endl;
    print_table_title(cout, display_mode);
    for(int i=0; i < point_return.size(); i++){
        print_opt(cout, P, point_return[i]->id, i+1, display_mode);
    }
    print_vline(display_mode);
}

void print_result_list(point_set_t* P, const vector<int> &ids){
    cout << endl;
    cout << vline << endl << endl;
    // cout << "This algorithm has finished."  << endl;
    cout << "These are the Airbnbs recommended by this algorithm:" << endl;
    print_table_title(cout);
    for(int i=0; i < ids.size(); i++){
        print_opt(cout, P, ids[i], i+1);
    }
    cout<< vline <<endl;
}


int alg_top1_select(const vector<point_t *> &point_return){
    cout << endl << "Please select your favorite one among the recommended Airbnbs." << endl;
    int option = 0, opt_range = point_return.size();
    while (option <= 0 || option > opt_range){
        string buf;
        cout << endl << "Your choice (choose your favorite) (enter 1 to " << opt_range << "): ";
        getline(cin, buf);
        if(is_number(buf)){
            option = stoi(buf);
        }
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


/**    @brief write the question asked by an algorithm and user's choices to a file in directory logs
 * 
 *             format:
 *                      quest 1 point 1
 *                      quest 1 point 2
 *                      user choice (1 or 2)
 * 
 *                      quest 2 point 1
 *                      quest 2 point 2
 *                      user choice (1 or 2)
 * 
*/
void record_ques_history(const int alg_id){
    ofstream ofs;
    ofs.open("../logs/history/" + to_string(alg_id+1) + ".txt", ofstream::out);
    int size = user_choices.size();
    for(int i = 0; i < size; i++){
        ofs << displayed_questions[i].first << endl;
        ofs << displayed_questions[i].second << endl;
        ofs << user_choices[i] << endl << endl;
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


/**
 * @brief Display Airbnbs in the final list
 * 
 * @param final_list 
 */
void display_final_list(point_set_t* P, std::vector<int> final_list){
    int display_mode = 2;
    print_vline(display_mode);
    cout << "The list of Airbnbs selected by you in Part 1 are as follows." << endl << endl;
    for(int i = 0; i <final_list.size(); i++){
        cout << "Recommended Airbnb No. " << i+1 << ":" << endl;
        print_table_title(cout, display_mode);
        print_opt(cout, P, final_list[i], i+1, display_mode);
        print_vline(display_mode);
    }
}


/**
 * @brief Ask the user to decide the favorite Airbnb in all returned Airbnbs
 * 
 * @param l_size Size of the final list
 * @return int 
 */
int ask_favorite_item(int l_size){
    int favorite = 0;
    printf("Please give a number from 1 to %d to indicate which Airbnb\n"
            "you like the most in the recommended Airbnb listed above. \n"
            "(e.g., 2 means Airbnb No. 2 is your favorite Airbnb).", l_size);
    while(favorite <1 || favorite > l_size){
        string buf;
        cout << endl << "Your choice (choose your favorite) (enter 1 to " << l_size << "): ";
        getline(cin, buf);
        if(is_number(buf)){
            favorite = stoi(buf);
        }
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

int higher_than_expect(point_set_t* P, int pid, int idx){
    int display_mode = 1;

    cout << "-------------------------------------------------------------" << endl;
    printf("|%15s|%10s|%10s|%10s|%10s|\n", "Labeled Price", "Clean", "Location", "Review PM", "Discount");
    cout << "-------------------------------------------------------------" << endl;
    printf("|%15d|%10.2f|%10.2f|%10.2f|%9d%%|\n", static_cast<int>(P->points[pid]->coord[0]), P->points[pid]->coord[1], 
                P->points[pid]->coord[2], P->points[pid]->coord[3], int(P->points[pid]->coord[4]));
    cout << "-------------------------------------------------------------" << endl;
    // print_table_title(cout, display_mode);
    // print_opt(cout, P, pid, idx + 1, display_mode);
    // print_vline(display_mode);

    point_t * best_point = P->points[pid];
    cout << endl;
    printf("The calculated \"final price\" for this rent is:   %d USD\n\n", static_cast<int>(ceil(best_point->coord[0] * (100 - best_point->coord[4]) / 100)));
    // printf("which is about:  %6d RMB\n\n", int(best_point->coord[0] * (100 - best_point->coord[4]) / 100 * 7.23));
    printf("Note that this Airbnb has a high discount rate as %d%%.\n", static_cast<int>(best_point->coord[4]));
    cout << "You may expect that the final price of this Airbnb is very low. But, after you see the calculated" << endl
            << "final price, do you think the real final price of this Airbnb is *higher* than your expectation?" << endl
            << "Probably, due to your focus on the high discount rate?" << endl << endl;
    int option = 0;
    while (option != 1 && option != 2){
        string buf;
        cout << endl << "Your answer (answer 1 for Yes, 2 for No): ";
        getline(cin, buf);
        if(is_number(buf)){
            option = stoi(buf);
        }
    }
    if(option == 1){
        return 1;
    }
    else{
        return 0;
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


void dissat_info_best(){
    cout << "------------------------------------------------------------------------------------------------" << endl << endl;
    cout << "The second task is to provide your opinion on the Airbnb recommended to you." << endl;
    cout << "Please provide a **dissatisfaction score** for the favorite Airbnb selected by you." << endl;
    cout << "The **dissatisfaction score** is an integer ranging from 0 to 10. It measures how *dissatisfied*" << endl
            << "you will feel if you get this Airbnb." << endl;
    cout << "0 means the least dissatisfied, and 10 means the most dissatisfied." << endl << endl;
    enter_to_continue();
    cout << endl;
}

int ask_dissat_score_best(point_set_t *P,  int best_id){
    cout << vline << endl;
    cout << "This is the favorite Airbnb you chose." << endl;
    cout << endl;
    print_table_title(cout);
    print_opt(cout, P, best_id, 1);
    cout << vline << endl;
    int ans = -1;
    cout << "Please give a dissatisfaction score (0 - 10) on this result list." << endl;
    while (ans < 0 || ans > 10){
        string buf;
        cout << endl << "Your answer (enter 0 - 10): ";
        getline(cin, buf);
        if(is_number(buf)){
            ans = stoi(buf);
        }
    }
    return ans;
}


void dissat_info(int dissat_score_best){
    cout << "------------------------------------------------------------------------------------------------" << endl << endl;
    cout << "Next, please also provide a **dissatisfaction score** to each recommendation list that does not" << endl
            << "recommend your favorite Airbnb." << endl << endl;

    cout << "The **dissatisfaction score** is an integer ranging from " << dissat_score_best << " to 10. It measures how *dissatisfied*" << endl
            << "you will feel if you get a Airbnb from this list instead of your favorite Airbnb." << endl;
    
    cout << dissat_score_best <<" means the least dissatisfied, and 10 means the most dissatisfied." << endl << endl;

    enter_to_continue();
    cout << endl;

}

int ask_dissat_score(point_set_t *P,  vector<int> &ids, int dissat_score_best){
    print_result_list(P, ids);
    int ans = -1;
    cout << "Please give a dissatisfaction score (" << dissat_score_best << " - 10) on this result list." << endl;
    while (ans < dissat_score_best || ans > 10){
        string buf;
        cout << endl << "Your answer (enter " << dissat_score_best << " - 10): ";
        getline(cin, buf);
        if(is_number(buf)){
            ans = stoi(buf);
        }
    }
    return ans;
}

void write_summary(int best_idx){
    ofstream ofs;
    ofs.open("../logs/summary.txt", ofstream::out);
    ofs << best_idx << "\t" << higher << endl;
    for(int i = 0; i < TOT_ALG_COUNT; i++){
        ofs << question_asked_list[i] << "\t";
        ofs << proc_time_list[i] << "\t";
        ofs << hit_list[i] << "\t";
        ofs << dissat_score_list[i] << "\t" << endl;
    }

    ofs.close();
}


void record_to_file(FILE *wPtr, int *records, int r_size){
    for(int i = 0; i < r_size; i++){
        fprintf(wPtr, "%d ", records[i]);
    }
    fprintf(wPtr, "\n");
}

void log_lock(){
    system("chmod -R 444 ../logs/*.txt");
}