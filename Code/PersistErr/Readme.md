# Robust Best Point Selection under Unreliable User Feedback


This Directory contains the source code of the proposed algorithms.

(1). *FC* (can be found in directory `optimal`)

(2). *SS-random* and *SS-score* (can be found in directory `sampling`)
  
Make sure there is a folder called `output` under the directory `PersistErr`.
They will be used for storing some intermediate results.

## Dependencies

The code requires `glpk` and `lp-solve` to run. The following shows how to install these packages if you are using Linux.

### Install glpk

Run the following command to install `glpk`.

    sudo apt-get install libglpk-dev

Then update the following lines in file `CMakeLists.txt`:

    set(INC_DIR /usr/local/Cellar/glpk/4.65/include)
    set(LINK_DIR /usr/local/Cellar/glpk/4.65/lib)

Replace the path `/usr/local/Cellar/glpk/4.65` to the path where you installed the `glpk` package.

### Install lp_solve

Run the following command to install `lp_solve`.

    sudo apt-get install lp-solve

Then update the following line in file `CMakeLists.txt`:

    find_library(LP_SOLVE NAMES liblpsolve55.so PATHS /usr/lib/lp_solve)

Replace the path `/usr/lib/lp_solve` to the path where you installed the `lp_solve` package.

## Usage Steps

### Compilation
Under directory `PersistErr`, run the following: 

    mkdir output
    mkdir build
    cd build
    cmake ..
    make
    
	
### Execution
Under directory `PersistErr/build`, run

    ./PersistErr NUM_REPEAT ALG_NAME INPUT THETA

where `NUM_REPEAT` is the number of times to repeat, `ALG_NAME` is the name of the algorithm, `INPUT` is the input dataset, and `THETA` is the error rate.

You may also simply run

	./PersistErr

which uses the default set of parameters.

### Input
The datasets tested in our paper can be found under another directory called `input`.
A dataset has the following format:

    num_of_record dim
    record_1
    record_2
    record_3
    ...
where `num_of_record` is the total number of record in the dataset, and `dim` is the dimensionality.

For example, here is a toy 3-d dataset with 4 records:

    4 3
    1   0   0
    0   1   0
    0   0   1
    0.5 0.5 0
	
### Output
The output will be shown on the console. It runs an algorithm (e.g. an algorithm with name `Alg`) for a number of rounds (e.g., 100 rounds).   

The first two lines of the output shows the dataset and algorithm names. At the end of the output, it displays the `correct count`, `avg question num` and `avg time` of the tested algorithm `Alg`. The following is a sample output.

    ../../input/4d100k.txt
    sampling
    round 0
    round 1
    ...
    ...
    round 98
    round 99
    correct count: 98
    avg question num: 19.35
    avg return size: 4.72
    avg time: 7257.64
