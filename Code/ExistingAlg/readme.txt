Readme (Baselines in Interactive Top-k Points Searching)
=========================
This package contains all source codes for 
1. Active-Ranking
2. Preference-Learning
3. Median (only work on 2-dimensonal datasets)
4. Median-Adapt (only work on 2-dimensonal datasets)
5. Hull (only work on 2-dimensonal datasets)
6. Hull-Adapt (only work on 2-dimensonal datasets)
7. UH-Simplex
8. UH-Simplex-Adapt
9. UH-Random
10. UH-Random-Adapt
11. UtilityApprox

This package also provides the real datasets.
Make sure there is a folder called "input/" and a folder called "output/" under the working directory.
They will be used for storing the input/output files and some intermediate results.

Usage Step
==========
a. Compilation
	mkdir build
	cd build
	cmake ..
	make

	You will need to install the GLPK package (for solving LPs) at first.
	See GLPK webpage <http://www.gnu.org/software/glpk/glpk.html>.
	Then update the path in CMakeLists.txt
          set(INC_DIR /usr/local/Cellar/glpk/4.65/include)
          set(LINK_DIR /usr/local/Cellar/glpk/4.65/lib)
	Update the path "/usr/local/Cellar/glpk/4.65" to the path you install the GLPK package
	
b. Execution
	./ExistingAlg

c. Input
	The input file contains the dataset (whose format will be described in Appendix A.)
	
d. Output
	The output will be shown on the console (whose format will be described in Appendix B.)

Example
=======
Sample input (input/2d.txt) are provided. Try:
./ExistingAlg

Appendix A. Format of Input File
------------------------------------
The format of the first line is: n d
n - number of points in the data set, integer
d - dimensionality, integer
The format of the following n lines is
<dim 1> <dim 2> ... <dim d>.
Each line corresponds to a point.
	
Appendix B. Format of Console Output
------------------------------------
The format of the output is
---------------------------------------------------------
|                  Ground Truth |        -- |        -- |
|                         Point |   top - 1 |   PtID-1  |
|                         Point |     ...   |   ...     |
|                         Point |   top - k |   PtID-k  |
---------------------------------------------------------
|                Active-Ranking | Q-count-1 |        -- |
|                         Point |     	 -- | PtID-(k+1)|
---------------------------------------------------------
|           Preference_Learning | Q-count-2 |        -- |
|                         Point |     	 -- | PtID-(k+2)|
---------------------------------------------------------
|                        Median | Q-count-3 |        -- |
|                         Point |     	 -- | PtID-(k+3)|
---------------------------------------------------------
|                  Median-Adapt | Q-count-4 |        -- |
|                         Point |     	 -- | PtID-(k+4)|
---------------------------------------------------------
|                          Hull | Q-count-5 |        -- |
|                         Point |     	 -- | PtID-(k+5)|
---------------------------------------------------------
|                    Hull-Adapt | Q-count-6 |        -- |
|                         Point |     	 -- | PtID-(k+6)|
---------------------------------------------------------
|                    UH-SIMPLEX | Q-count-7 |        -- |
|                         Point |     	 -- | PtID-(k+7)|
---------------------------------------------------------
|              UH-SIMPLEX-Adapt | Q-count-8 |        -- |
|                         Point |     	 -- | PtID-(k+8)|
---------------------------------------------------------
|                     UH-RANDOM | Q-count-9 |        -- |
|                         Point |     	 -- | PtID-(k+9)|
---------------------------------------------------------
|               UH-RANDOM-Adapt | Q-count-10|        -- |
|                         Point |     	 -- |PtID-(k+10)|
---------------------------------------------------------
|                 UtilityApprox | Q-count-11|        -- |
|                         Point |     	 -- |PtID-(k+11)|
---------------------------------------------------------
where PtID-1, ..., PtID-k are the point ID of the user's top-k points (ground truth),
PtID-(k+1) is the point ID of one of the user's top-k points returned by algorithm Active-Ranking,
PtID-(k+2) is the point ID of one of the user's top-k points returned by algorithm Preference_Learning,
PtID-(k+3) is the point ID of one of the user's top-k points returned by algorithm Median,
PtID-(k+4) is the point ID of one of the user's top-k points returned by algorithm Median-Adapt,
PtID-(k+5) is the point ID of one of the user's top-k points returned by algorithm Hull,
PtID-(k+6) is the point ID of one of the user's top-k points returned by algorithm Hull-Adapt,
PtID-(k+7) is the point ID of one of the user's top-k points returned by algorithm UH-SIMPLEX,
PtID-(k+8) is the point ID of one of the user's top-k points returned by algorithm UH-SIMPLEX-Adapt,
PtID-(k+9) is the point ID of one of the user's top-k points returned by algorithm UH-RANDOM,
PtID-(k+10) is the point ID of one of the user's top-k points returned by algorithm UH-RANDOM-Adapt,
PtID-(k+11) is the point ID of one of the user's top-k points returned by algorithm UtilityApprox,
Q-count-1 is the number of questions asked in Active-Ranking,
Q-count-2 is the number of questions asked in Preference_Learning,
Q-count-3 is the number of questions asked in Median,
Q-count-4 is the number of questions asked in Median-Adapt,
Q-count-5 is the number of questions asked in Hull,
Q-count-6 is the number of questions asked in Hull-Adapt, 
Q-count-7 is the number of questions asked in UH-SIMPLEX, 
Q-count-8 is the number of questions asked in UH-Simplex-Adapt, 
Q-count-9 is the number of questions asked in UH-RANDOM, 
Q-count-10 is the number of questions asked in UH-RANDOM-Adapt and 
Q-count-11 is the number of questions asked in UtilityApprox
For example, you might see:
---------------------------------------------------------
|                  Ground Truth |        -- |        -- |
|                         Point |   top - 1 |     64545 |
---------------------------------------------------------
|                Active-Ranking |         9 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|           Preference_Learning |         8 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                        Median |         3 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                  Median-Adapt |         4 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                          Hull |         3 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                    Hull-Adapt |         3 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                    UH-SIMPLEX |         3 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|              UH-SIMPLEX-Adapt |         4 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                     UH-RANDOM |         3 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|               UH-RANDOM-Adapt |         3 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                 UtilityApprox |        31 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
