This Directory contains the code for user study on the follwing algorithms.

The following algorithms are included in the user study:

	(1). *SE-score* (ours)
  
	(2). *SS-score* (ours)

    (3). *Verify-Point*
   
    (4). *HD-PI*
  
	(5). *Pref-Learn*
  
	(6). *Active-Ranking*

The real datasets `airbnb` used in the user study can be found in another directory `input`.
Link to the original dataset can be found in our paper.

Make sure there is a folder called `output` and a folder called `logs` under the directory `user_study`.
They will be used for storing some intermediate results and the performance of each algorithm.

## Dependencies

The code requires `glpk` and `lp-solve` to run. The following shows how to install these packages if you are using Linux.

### Install glpk

Run the following command to install `glpk`.

    sudo apt-get install libglpk-dev

Then update the following lines in file `CMakeLists.txt`:

    set(INC_DIR /usr/local/Cellar/glpk/4.65/include)
    set(LINK_DIR /usr/local/Cellar/glpk/4.65/lib)

Update the path `/usr/local/Cellar/glpk/4.65` to the path where you installed the `glpk` package.

### Install lp_solve

Run the following command to install `lp_solve`.

    sudo apt-get install lp-solve

Then update the following line in file `CMakeLists.txt`:

    find_library(LP_SOLVE NAMES liblpsolve55.so PATHS /usr/lib/lp_solve)

Update the path `/usr/lib/lp_solve` to the path where you installed the `lp_solve` package.

## Usage Steps

### Compilation
Under directory `user_study`, run the following: 

	mkdir logs
    mkdir output
    mkdir build
    cd build
    cmake ..
    make
    
	
### Execution
Under directory `user_study/build`, run

	./user_study 

### Input
The datasets `airbnb` can be found under directory `input`.
It has the following format:

    num_of_record dim
    airbnb_1
    airbnb_2
    airbnb_3
    ...
where `num_of_record` is the total number of record in the dataset, and `dim` is the dimensionality (4 in `airbnb`).

Each airbnb consist of 4 numbers representing attributes `daily price`, `cleanliness rating`, `location rating` and `number of reviews per month`. 
	
### Output
The output is the content of the user study. The instruction in the user study is self-explanatory.

There are in total 6 algorithms in the user study.

Their performance is recorded under directory `user_study/logs`.

`1.txt` to `6.txt` each records the performance of each algorithm. See the following table for their correspondance:

id | algorithm name |
-----|-----|
alg 1 | *Verify-Point*|
alg 2 | *HD-PI* |
alg 3 | *SE-score* |
alg 4 | *SS-score* |
alg 5 | *Active-Ranking* |
alg 6 | *Pref_Learn* |

Each file stores the following content.

    num_of_ques
    proc_time
    ret_size
    pid_final_recom
    pid_ret1
    pid_ret2
    ...

where `num_of_ques` is the number of questions used by the algorithm, `proc_time` is the average processing time to decide the next question, `ret_size` is the number of returned points, `pid_final_recom` is the pid of the final recommendation (see our paper for the defination of final recommendation). `pid_ret1`, `pid_ret2`, ... are the pid of the 1st returned points, 2nd returned points and so on.

`summary.txt` summarizes the statistics collected in user study. It has the following format.

    pid_best dissat_best
    num_of_ques proc_time hit dissat
    num_of_ques proc_time hit dissat
    num_of_ques proc_time hit dissat
    ...

where the first row stores the pid of the best point and the dissatisfaction score of the best point.

Starting from the second row, each row stores the statistics of an algorithm, from alg 1 to alg 6. For each algorithm, its number of questions used, average processing time, hit and dissatisfaction score are recorded (see our paper for the defination of hit and dissatisfaction score).