# PersistErr


This package contains the proposed algorithms under `Code/InteractiveTopk`: 
  
1. Algorithm *Exact* (can be found in directory exact)
  
2. Algorithm *Approx* (can be found in directory approx)
       
The baseline algorithms can also be found in this directory.

The code requires `glpk` and `lp-solve` to run.

If you are using ubuntu, you can run the following command to install `glpk` and `lp-solve`.

```
sudo apt-get install libglpk-dev
sudo apt-get install lp-solve
```

To compile the code, clone it to your local machine and direct to `Code/Experiments`, `Code/RI_user_study` or `Code/Var_Experiments`, then run the following.

```
mkdir output
mkdir build
cd build
cmake ..
make
```
