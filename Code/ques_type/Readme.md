The Directory contains the source code for variants of *FC*, *SS-random* and *SS-score*. The following variants are provided:
	
	(1). Display *s* points in each round and ask the user to select the favorite point (instead of pairwise comparison)
	
	(2). Display *s* points in each round and ask the user to partition them into *superior* and *inferior* groups (instead of pairwise comparison)

Make sure there is a folder called `output` under the directory `ques_type`.
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
Under directory `ques_type`, run the following: 

    mkdir output
    mkdir build
    cd build
    cmake ..
    make
    
	
### Execution
Under directory `ques_type/build`, run

	./ques_type

### Input
The datasets tested in our paper can be found under another directory called `input`.
A dataset has the following format:

    num_of_record dim
    record_1
    record_2
    record_3
    ...
where `num_of_record` is the total number of record in the dataset, and `dim` is the dimensionality.

A toy 3-d dataset with 4 records:

    4 3
    1   0   0
    0   1   0
    0   0   1
    0.5 0.5 0
	
### Output
The output will be shown on the console. It runs an algorithm (e.g. an algorithm with name `Alg`) for a number of rounds (e.g., 100 rounds).  

At the end of the output, it displays the `accuracy`, `avg question num` and `avg time` of the tested algorithm `Alg`.