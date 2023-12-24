#include "user_study_func.h"

using namespace std;

string vline = "--------------------------------------------------------";


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
    cout << "-------------------------Welcome to the recommending airbnb system--------------------------------" << endl;
    cout << "Imagine that you are planning a trip to a new conuntry and want to rent an airbnb. There are " << endl
           << "thousands of airbnbs on the website." << endl << endl;
    cout << "Since renting a good airbnb can greatly affect the happiness of your journey, You will want to find" << endl
           << "your favorite airbnb." << endl << endl;
    cout << "In our research project, we want to study several recommendation algorithms and see whether they" << endl
           << "can help you find your favorite airbnb if some **incorrect** input is given by the user." << endl << endl;
    cout << "This survey consists of 2 parts. Based on your answers in Part 1, there is a small chance that " << endl
           << "Part 2 is skipped." << endl << endl;
    // enter_to_continue();
    enter_recover();
    cout << endl;
}

void part1_info(){
    cout << "-----------------------------------------------------------------------------------------------" << endl ;
    cout << "-----------------------------------------------------------------------------------------------" << endl << endl;
    cout << "                                    Beginning of Section 2" << endl << endl;
    cout << "-----------------------------------------------------------------------------------------------" << endl ;
    cout << "-----------------------------------------------------------------------------------------------" << endl << endl;
    enter_to_continue();

    cout << "In Section 2, we want to evaluate algorithm's performance with a limited number of questions." << endl << endl;
    cout << "The flow of Sections 2 is similar to Section 1. However, each algorithm will ask you at most " << endl 
        << "10 questions before making its recommendations. " << endl << endl;
    enter_to_continue();

    cout << "===============================================================================================" << endl << endl;
    cout << "                                     Beginning of Part 1" << endl << endl;
    cout << "===============================================================================================" << endl << endl;
    cout << "In Part 1, there are 6 algorithms and each will ask you some questions." << endl << endl;
    cout << "In each question, you will be presented 2 options of airbnbs and you need to pick the one that" << endl
            << "you favor more." << endl << endl;

    cout << "By the end of each algorithm, a list of airbnbs will be recommended by this algorithm." << endl;
    cout << "Your task is to select 1 airbnb from the recommendation list that you think is the best." << endl << endl;
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


void part2_info(){
    cout << "===============================================================================================" << endl << endl;
    cout << "                                     Beginning of Part 2" << endl << endl;
    cout << "===============================================================================================" << endl << endl;

    cout << "In Part 2, there are two tasks." << endl << endl;

    cout << "Based on your answer to the first task, you may not need to do the second task." << endl << endl;

    cout << "The first task is to decide your favorite airbnb among the airbnbs you selected at the end of each" << endl
            << "algorithm in Part 1." << endl << endl;

    enter_to_continue();
    cout << endl;
}


void part2_skip(){
    cout << "According to your answer in Part 1, you do not need to take Part 2." << endl << endl;
    cout << "Part 2 is skipped." << endl << endl;
}

void part2_second_task_skip(){
    cout << endl << endl;
    cout << "According to your answer in task 1, you do not need to take task 2." << endl << endl;
    cout << "Task 2 is skipped." << endl << endl;
}


void part2_end(){
    cout << "===============================================================================================" << endl << endl;
    cout << "                                        End of Part 2" << endl << endl;
    cout << "===============================================================================================" << endl << endl << endl << endl;
}


void end(){
    cout << "-----------------------------------------------------------------------------------------------" << endl ;
    cout << "-----------------------------------------------------------------------------------------------" << endl << endl;
    cout << "                                       End of Section 2" << endl << endl;
    cout << "-----------------------------------------------------------------------------------------------" << endl ;
    cout << "-----------------------------------------------------------------------------------------------" << endl << endl;
    cout << "Section completed 2 / 4." << endl << endl;
    enter_to_continue();
    // cout << "Your completion code is: C5EPB200" << endl;
}


void print_table_title(ostream &os){
    char buffer[1024];
    sprintf(buffer, "|%10s|%10s|%10s|%10s|%10s|", " ", "Price(USD)", "Clean", "Location", "Review PM");
    cout << vline << endl;
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


void print_opt(ostream &os, point_set_t* P, const int idx, const int opt){
    cout << vline << endl;
    cout << format_opt(P->points[idx], opt) << endl;
}


void print_opt(ostream &os, const point_t *p, const int opt){
    cout << vline << endl;
    cout << format_opt(p, opt) << endl;
}


int show_to_user(point_set_t* P, int p_idx, int q_idx)
{
    int option = 0;
    cout << endl;
    cout << "Please choose the airbnb you favor more:" << endl;
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


int show_to_user(const point_t* p1, const point_t* p2)
{
    int option = 0;
    cout << endl; 
    cout << "Please choose the airbnb you favor more:" << endl;
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
    cout << "These are the airbnbs recommended by this algorithm:" << endl;
    print_table_title(cout);
    for(int i=0; i < point_return.size(); i++){
        print_opt(cout, P, point_return[i]->id, i+1);
    }
    cout<< vline <<endl;
}


void print_result_list(point_set_t* P, const vector<int> &ids){
    cout << endl;
    cout << vline << endl << endl;
    // cout << "This algorithm has finished."  << endl;
    cout << "These are the airbnbs recommended by this algorithm:" << endl;
    print_table_title(cout);
    for(int i=0; i < ids.size(); i++){
        print_opt(cout, P, ids[i], i+1);
    }
    cout<< vline <<endl;
}

void print_result_list(point_set_t* P, const vector<int> &ids, int i){
    cout << endl;
    cout << vline << endl << endl;
    // cout << "This algorithm has finished."  << endl;
    cout << "These are the airbnbs recommended by Algorithm " << i << ":" << endl;
    print_table_title(cout);
    for(int i=0; i < ids.size(); i++){
        print_opt(cout, P, ids[i], i+1);
    }
    cout<< vline <<endl;
}

int alg_top1_select(const vector<point_t *> &point_return){
    cout << endl << "Please select your favorite one among the recommended airbnbs." << endl;
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
    ofs << "question asked: " << question_asked_list[alg_id] << endl;
    ofs << "processing time: " << proc_time_list[alg_id] << endl;
    ofs << "best point id: " << point_return[best_idx]->id << endl;
    ofs << "return size: " << point_return.size() << endl;
    ofs << "returned pids: " << endl;
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
 * @brief Display airbnbs in the final list
 * 
 * @param final_list 
 */
void display_final_list(point_set_t* P, std::vector<int> final_list){
    cout << vline << endl;
    cout << "Now, we need you to decide your favorite airbnb among the " << endl
            << "airbnbs you selected in Part 1." << endl << endl;
    cout << "The list of airbnbs selected by you are as follows." << endl << endl;
    for(int i = 0; i <final_list.size(); i++){
        cout << "Recommended airbnb No. " << i+1 << ":" << endl;
        print_table_title(cout);
        print_opt(cout, P, final_list[i], i+1);
        cout << vline << endl;
    }
}


/**
 * @brief Ask the user to decide the favorite airbnb in all returned airbnbs
 * 
 * @param l_size Size of the final list
 * @return int 
 */
int ask_favorite_item(int l_size){
    int favorite = 0;
    printf("Please give a number from 1 to %d to indicate which airbnb\n"
            "you like the most in the recommended airbnb listed above \n"
            "(e.g., 2 means airbnb No. 2 is your favorite airbnb)", l_size);
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
    cout << "The second task is to provide your opinion on the airbnb recommended to you." << endl;
    cout << "Please provide a **dissatisfaction score** for the favorite airbnb selected by you." << endl;
    cout << "The **dissatisfaction score** is an integer ranging from 0 to 10. It measures how *dissatisfied*" << endl
            << "you will feel if you get this airbnb." << endl;
    cout << "0 means the least dissatisfied, and 10 means the most dissatisfied." << endl << endl;
    enter_to_continue();
    cout << endl;
}

int ask_dissat_score_best(point_set_t *P,  int best_id){
    cout << vline << endl;
    cout << "This is the favorite airbnb you chose." << endl;
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
            << "recommend your favorite airbnb." << endl << endl;

    cout << "The **dissatisfaction score** is an integer ranging from " << dissat_score_best << " to 10. It measures how *dissatisfied*" << endl
            << "you will feel if you get a airbnb from this list instead of your favorite airbnb." << endl;
    
    cout << dissat_score_best <<" means the least dissatisfied, and 10 means the most dissatisfied." << endl << endl;

    enter_to_continue();
    cout << endl;

}



int ask_dissat_score(point_set_t *P,  vector<int> &ids, int dissat_score_best, int i){
    print_result_list(P, ids, i);
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
    ofs << "best point idx \t" << "best point dissat score" << endl;
    ofs << best_idx << "\t" << dissat_score_best << endl;
    ofs << "# quest \t" << "time \t" << "hit \t" << "dissat score \t" << endl;
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


vector<point_t *> sampleVector(const vector<point_t *>& originalVector, int sampleSize){
    vector<point_t *> sampledVector;
    
    // Create a random number generator
    random_device rd;
    mt19937 generator(rd());
    
    // Shuffle the original vector
    vector<point_t *> shuffledVector = originalVector;
    shuffle(shuffledVector.begin(), shuffledVector.end(), generator);
    
    // Take the first 'sampleSize' elements from the shuffled vector
    for (int i = 0; i < sampleSize; ++i) {
        sampledVector.push_back(shuffledVector[i]);
    }
    
    return sampledVector;
}