Readme (Interactive Top-k Points Searching)
=========================
This package contains all source codes for 
1. 2D-PI (only work on 2-dimensonal datasets)
2. HD-PI (sampling and accurate)
3. RH

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
	./InteractiveTopk

c. Input
	The input file contains the dataset (whose format will be described in Appendix A.)
	
d. Output
	The output will be shown on the console (whose format will be described in Appendix B.)

Example
=======
Sample input (input/2d.txt) are provided. 
Try: ./InteractiveTopk

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
|                            RH | Q-count-1 |        -- |
|                         Point |     	 -- | PtID-(k+1)|
---------------------------------------------------------
|                HD-PI_sampling | Q-count-2 |        -- |
|                         Point |     	 -- | PtID-(k+2)|
---------------------------------------------------------
|                HD-PI_accurate | Q-count-3 |       --  |
|                         Point |     	 -- | PtID-(k+3)|
---------------------------------------------------------
|                         2D-PI | Q-count-4 |        -- |
|                         Point |     	 -- | PtID-(k+4)|
---------------------------------------------------------
where PtID-1, ..., PtID-k are the point ID of the user's top-k points (ground truth),
PtID-(k+1) is the point ID of one of the user's top-k points returned by algorithm RH,
PtID-(k+2) is the point ID of one of the user's top-k points returned by the sampling version of algorithm HD-PI,
PtID-(k+3) is the point ID of one of the user's top-k points returned by the accurate version of algorithm HD-PI,
PtID-(k+4) is the point ID of one of the user's top-k points returned by algorithm 2D-PI,
Q-count-1 is the number of questions asked in RH,
Q-count-2 is the number of questions asked in HD-PI_sampling,
Q-count-3 is the number of questions asked in HD-PI_accurate and
Q-count-4 is the number of questions asked in 2D-PI.
For example, you might see:
---------------------------------------------------------
|                  Ground Truth |        -- |        -- |
|                         Point |   top - 1 |     64545 |
|                         Point |   top - 2 |     83062 |
|                         Point |   top - 3 |     19937 |
|                         Point |   top - 4 |     61131 |
|                         Point |   top - 5 |     41644 |
|                         Point |   top - 6 |     68472 |
|                         Point |   top - 7 |     19744 |
|                         Point |   top - 8 |     37936 |
|                         Point |   top - 9 |     59683 |
|                         Point |   top -10 |     74530 |
|                         Point |   top -11 |     35011 |
|                         Point |   top -12 |     25524 |
|                         Point |   top -13 |     93046 |
|                         Point |   top -14 |     32259 |
|                         Point |   top -15 |     82843 |
|                         Point |   top -16 |     72131 |
|                         Point |   top -17 |     34184 |
|                         Point |   top -18 |     71690 |
|                         Point |   top -19 |     35219 |
|                         Point |   top -20 |     23484 |
|                         Point |   top -21 |     28401 |
|                         Point |   top -22 |     11288 |
|                         Point |   top -23 |     18897 |
|                         Point |   top -24 |     70774 |
|                         Point |   top -25 |     69136 |
|                         Point |   top -26 |     51791 |
|                         Point |   top -27 |     44364 |
|                         Point |   top -28 |     83611 |
|                         Point |   top -29 |     67036 |
|                         Point |   top -30 |     48685 |
|                         Point |   top -31 |     59847 |
|                         Point |   top -32 |     69579 |
|                         Point |   top -33 |     47581 |
|                         Point |   top -34 |     32106 |
|                         Point |   top -35 |     42504 |
|                         Point |   top -36 |     17545 |
|                         Point |   top -37 |     86964 |
|                         Point |   top -38 |     68959 |
|                         Point |   top -39 |     69804 |
|                         Point |   top -40 |     24335 |
|                         Point |   top -41 |     32124 |
|                         Point |   top -42 |     66793 |
|                         Point |   top -43 |     62487 |
|                         Point |   top -44 |     59439 |
|                         Point |   top -45 |     74439 |
|                         Point |   top -46 |     99158 |
|                         Point |   top -47 |     49870 |
|                         Point |   top -48 |     81699 |
|                         Point |   top -49 |     87337 |
|                         Point |   top -50 |     39492 |
---------------------------------------------------------
|                            RH |         3 |        -- |
|                         Point |        -- |     74530 |
---------------------------------------------------------
|                HD-PI_sampling |         2 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                HD-PI_accurate |         2 |        -- |
|                         Point |        -- |     64545 |
---------------------------------------------------------
|                         2D-PI |         1 |        -- |
|                         Point |        -- |     74530 |
---------------------------------------------------------
