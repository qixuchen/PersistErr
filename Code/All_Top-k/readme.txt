Readme (Interactive Search for One of the Top-k)
=========================
This package contains the source codes, which return all the top-k points
1. HD-PI (AllTopK) (sampling and accurate)
2. RH (AllTopK)

This package also provides the synthetic and real datasets.

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
Sample input (input/4d.txt) are provided. 
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
-----------------------------------------------------------
|                  Ground Truth |        -- |          -- |
|                         Point |   top - 1 |    PtID-1   |
|                         Point |     ...   |     ...     |
|                         Point |   top - k |    PtID-k   |
-----------------------------------------------------------
|                            RH | Q-count-1 |          -- |
|                         Point |        -- |  PtID-(k+1) |
|                         Point |     ...   |     ...     |
|                         Point |   top - k |  PtID-(k+k) |
-----------------------------------------------------------
|                HD-PI_sampling | Q-count-2 |          -- |
|                         Point |        -- |  PtID-(2k+1)|
|                         Point |     ...   |     ...     |
|                         Point |   top - k |  PtID-(2k+k)|
-----------------------------------------------------------
|                HD-PI_accurate | Q-count-3 |         --  |
|                         Point |        -- | PtID-(3k+1) |
|                         Point |     ...   |     ...     |
|                         Point |   top - k | PtID-(3k+k) |
-----------------------------------------------------------
where PtID-1, ..., PtID-k are the point ID of the user's top-k points (ground truth),
PtID-(k+1), ..., PtID-(k+k) are the point ID of the user's top-k points returned by algorithm RH,
PtID-(2k+1), ..., PtID-(2k+k) are the point ID of the user's top-k points returned by the sampling version of algorithm HD-PI,
PtID-(3k+1), ..., PtID-(3k+k) are the point ID of the user's top-k points returned by the accurate version of algorithm HD-PI,
Q-count-1 is the number of questions asked by RH,
Q-count-2 is the number of questions asked by HD-PI_sampling,
Q-count-3 is the number of questions asked by HD-PI_accurate.
For example, you might see:
---------------------------------------------------------
|                  Ground Truth |        -- |        -- |
|                         Point |   top - 1 |     52897 |
|                         Point |   top - 2 |     33170 |
|                         Point |   top - 3 |     35934 |
|                         Point |   top - 4 |     25979 |
|                         Point |   top - 5 |     90582 |
|                         Point |   top - 6 |     13908 |
|                         Point |   top - 7 |     46348 |
|                         Point |   top - 8 |     98191 |
|                         Point |   top - 9 |     56703 |
|                         Point |   top -10 |     14442 |
|                         Point |   top -11 |     35156 |
|                         Point |   top -12 |     26750 |
|                         Point |   top -13 |     65897 |
|                         Point |   top -14 |     52271 |
|                         Point |   top -15 |     65443 |
|                         Point |   top -16 |     28471 |
|                         Point |   top -17 |      8337 |
|                         Point |   top -18 |     13255 |
|                         Point |   top -19 |     60060 |
|                         Point |   top -20 |     80197 |
---------------------------------------------------------
|                            RH |        59 |        -- |
|                         Point |         1 |     52897 |
|                         Point |         2 |     33170 |
|                         Point |         3 |     35934 |
|                         Point |         4 |     25979 |
|                         Point |         5 |     90582 |
|                         Point |         6 |     13908 |
|                         Point |         7 |     46348 |
|                         Point |         8 |     98191 |
|                         Point |         9 |     56703 |
|                         Point |        10 |     14442 |
|                         Point |        11 |     26750 |
|                         Point |        12 |     35156 |
|                         Point |        13 |     65897 |
|                         Point |        14 |     52271 |
|                         Point |        15 |     65443 |
|                         Point |        16 |     28471 |
|                         Point |        17 |      8337 |
|                         Point |        18 |     13255 |
|                         Point |        19 |     60060 |
|                         Point |        20 |     80197 |
---------------------------------------------------------
|                 HDPI-sampling |        30 |        -- |
|                         Point |         1 |     52897 |
|                         Point |         2 |     33170 |
|                         Point |         3 |     35934 |
|                         Point |         4 |     25979 |
|                         Point |         5 |     13908 |
|                         Point |         6 |     90582 |
|                         Point |         7 |     46348 |
|                         Point |         8 |     98191 |
|                         Point |         9 |     14442 |
|                         Point |        10 |     56703 |
|                         Point |        11 |     35156 |
|                         Point |        12 |     26750 |
|                         Point |        13 |     65897 |
|                         Point |        14 |     52271 |
|                         Point |        15 |     28471 |
|                         Point |        16 |     65443 |
|                         Point |        17 |      8337 |
|                         Point |        18 |     13255 |
|                         Point |        19 |     60060 |
|                         Point |        20 |     80197 |
---------------------------------------------------------
|                 HDPI-accurate |        29 |        -- |
|                         Point |         1 |     52897 |
|                         Point |         2 |     33170 |
|                         Point |         3 |     35934 |
|                         Point |         4 |     25979 |
|                         Point |         5 |     13908 |
|                         Point |         6 |     90582 |
|                         Point |         7 |     46348 |
|                         Point |         8 |     98191 |
|                         Point |         9 |     56703 |
|                         Point |        10 |     14442 |
|                         Point |        11 |     35156 |
|                         Point |        12 |     26750 |
|                         Point |        13 |     65897 |
|                         Point |        14 |     52271 |
|                         Point |        15 |     65443 |
|                         Point |        16 |     28471 |
|                         Point |        17 |      8337 |
|                         Point |        18 |     13255 |
|                         Point |        19 |     60060 |
|                         Point |        20 |     80197 |
---------------------------------------------------------

