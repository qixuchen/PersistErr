Readme (Recommending Car System)
=========================
This package contains all source codes for user study on
1. UH-Simplex
2. Preference-Learning
3. RH
4. UH-Random
5. Active-Ranking
6. HDPI_sampling
7. HDPI_accurate

This package also provides the used car dataset from
https://www.kaggle.com/orgesleka/used-cars-database.

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
	Update "/usr/local/Cellar/glpk/4.65" to the path where you install the GLPK package
	
b. Execution
	./User_study

c. Input
	The used car dataset is shown in input/car.txt (whose format will be described in Appendix A.)
	
d. Output
	1. The console output will be shown on the console (whose format will be described in Appendix B.)
	2. The user log could be found in output/result.txt (whose format will be described in Appendix C.)

Example
=======
Try: ./User_study
Sample user log is provided: output/result.txt

Appendix A. Format of Input File
------------------------------------
For input/car.txt (the used car dataset with four attributes: price, year, power and used KM):
	The format of the first line is: n d
	n - number of cars in the dataset, integer
	d - number of attributes, integer (which is 4)
	The format of the following n lines is
	<price> <year> <power> <used KM>.
	Each line corresponds to a car
	
Appendix B. Format of Console Output
------------------------------------
The console output consists of three components: (a) interaction, (b) evaluation and (c) comparison
The recommending car system interacts with you for different rounds.

a. Interaction
	At each round, you will be presented consecutive questions correspond to a method in
	our system (two cars and you are asked to choose the one you favor more).
	For example, you might see:
	Please choose the car you favor more:
	--------------------------------------------------------
	|          |Price(USD)|      Year|   PowerPS|   Used KM|
	--------------------------------------------------------
	|  Option 1|     28800|      2013|       432|     20000|
	--------------------------------------------------------
	|  Option 2|     15400|      2015|       150|     20000|
	--------------------------------------------------------
	Your choice: 
	
b. Evaluation
	At the end of each round, it will ask you to give a score to indicate how bored you
	feel when you are asked with XX questions for this round in order to obtain one of 
	your 20 most favorite cars. 
	For example, you might see:
	--------------------------------------------------------
	No. of questions asked:         10 
	--------------------------------------------------------
	Recommended cars:
	--------------------------------------------------------
	|          |Price(USD)|      Year|   PowerPS|   Used KM|
	---------------------------------------------------------
	|       Car|     28800|      2013|       432|     20000|
	---------------------------------------------------------
	Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate 
	how bored you feel when you are asked with 10 questions for this round 
	in order to obtain one of your 20 most favorite cars (Note: 10 denotes
	that you feel the most bored and 1 denotes that you feel the least bored.)
	You answer:

c. Comparison
	After all the rounds. it will ask you to compare different rounds. You will be presented 
	two rounds (including number of questions and recommended car) each time and you need to 
	choose the round you favor more.
	For example, you might see:
	=========================== Round 2 ===========================
	No. of questions asked: 8 
	Recommended cars:
	--------------------------------------------------------
	|          |Price(USD)|      Year|   PowerPS|   Used KM|
	---------------------------------------------------------
	|       Car|     28800|      2013|       432|     20000|
	---------------------------------------------------------
	=========================== Round 3 ===========================
	No. of questions asked: 3 
	Recommended cars:
	--------------------------------------------------------
	|          |Price(USD)|      Year|   PowerPS|   Used KM|
	---------------------------------------------------------
	|       Car|     28800|      2013|       432|     20000|
	---------------------------------------------------------
	Which round are you satisfied more? 
	Please enter:
  	(1) 1 for Round 2 
  	(2) 2 for Round 3 
  	(3) 3 for both Round 2 and Round 3.
	Note that all car(s) returned by each round must be one of your 20 most favorite cars
	in our used car database. Please answer these based on two criteria at the same time:
	(a) No. of questions asked and (b) The recommended car(s)
	You answer: 
	
Appendix C. Format of User Log
------------------------------------
	The user log file is shown in output/result.txt.
	It contains two parts: (a) Results of Rounds and (b) Results of comparison

a.  Results of Rounds
	Lines 1-3 show the algorithm name and the number of questions asked.
	Lines 4-9 show the recommended car.
	Lines 10-13 show the evaluation result.
	For example, you might see:
	--------------------------------------------------------
	|                    UH-SIMPLEX |        17 |
	--------------------------------------------------------
	Recommended cars:
	--------------------------------------------------------
	|          |Price(USD)|      Year|   PowerPS|   Used KM|
	--------------------------------------------------------
	|       Car|     28800|      2013|       432|     20000|
	---------------------------------------------------------
	Please give a number from 1 to 10 (i.e., 1, 2, .., 10) to indicate 
	how bored you feel when you are asked with 17 questions for this round 
	in order to obtain one of your 20 most favorite cars (Note: 10 denotes
	that you feel the most bored and 1 denotes that you feel the least bored.) 4
	
b. Results of comparison
	Lines 1-8 show the result of a round, including the number of 
	questions and the recommended car.
	Lines 9-16 show the result of another round, including the number of 
	questions and the recommended car.
	Line 17 show your choice which one you favor more. 
	For example, you might see:
	=========================== Round 2 ===========================
	No. of questions asked: 8 
	Recommended cars:
	--------------------------------------------------------
	|          |Price(USD)|      Year|   PowerPS|   Used KM|
	---------------------------------------------------------
	|       Car|     28800|      2013|       432|     20000|
	---------------------------------------------------------
	=========================== Round 3 ===========================
	No. of questions asked: 3 
	Recommended cars:
	--------------------------------------------------------
	|          |Price(USD)|      Year|   PowerPS|   Used KM|
	---------------------------------------------------------
	|       Car|     28800|      2013|       432|     20000|
	---------------------------------------------------------
	Which round are you satisfied more?  2
