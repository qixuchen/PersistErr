Readme (Recommending Used Car System for Interactive Search for One of the Top-k)
=========================
This package contains all source codes for user study (for motivation)
1. RH (SomeTopK) (returning k' out of top-k points, where k'<=k)
2. HDPI (sampling and accurate) (SomeTopK) (returning k' out of top-k points, where k'<=k)

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
	2. The user log could be found in output/%name.txt (whose format will be described in Appendix C.)

Example
=======
Try: ./User_study
Sample user log is provided: output/test.txt

Appendix A. Format of Input File
------------------------------------
For input/car.txt (the used car dataset with four attributes: price, year, power and used KM):
	The format of the first line is: n d
	n - number of used cars in the dataset, integer
	d - number of attributes, integer (which is 4)
	The format of the following n lines is
	<price> <year> <power> <used KM>.
	Each line corresponds to a used car
	
Appendix B. Format of Console Output
------------------------------------
The console output consists of three components: (a) Interaction and Evaluation and (b) rank
The recommending used car system interacts with you for different rounds.

a. Interaction
(1).The recommending car system will interact with you in 3 rounds.
	You will see the following at the beginning of each round
	=========================Round i=========================
	where, 
	Round 1 is the interacting process of algorithm RH (i = 1),
	Round 2 is the interacting process of algorithm HDPI_sampling (i = 2),
	and Round 3 is the interacting process of algorithm HDPI_accurate (i = 3).
	In each round, the system will continuously ask you questions and collect information based
	on your preference. When sufficient information is obtained, the system will return k' 
	(k' = 1, 5, 10, 15, 20) out of your 20 most favorite cars. 
	Specifically, the system will proceed in each round as follows.
	-Ask you questions
	-Return 1 out of your 20 most favorite cars
	-Continuously ask you questions
	-Return 5 out of your 20 most favorite cars
	-Continuously ask you questions
	-Return 10 out of your 20 most favorite cars
	-Continuously ask you questions
	-Return 15 out of your 20 most favorite cars
	-Continuously ask you questions
	-Return 20 out of your 20 most favorite cars
	
(2).In each round, you will be presented consecutive questions correspond to a method in our 
	system (i.e., two cars will be presented and you are asked to choose the one you favor more).
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
	
(3).In each round, based on your feedback, the system learns you preference. When sufficient 
	information is obtained, the system will return k' (k' = 1, 5, 10, 15, 20) out of your 20 
	most favorite cars. 
	Then, it will ask you to give a score to indicate how bored you feel when you are asked 
	with XX questions for this round in order to obtain k' (k' = 1, 5, 10, 15, 20) out of 
	your 20 most favorite cars. 
	For example, you might see:
	------------------------------------------
	No. of questions asked:  12 
	Recommended cars:
	------------------------------------------
	|  Car|Price(USD)| Year| PowerPS| Used KM|
	------------------------------------------
	|    1|     28800| 2013|     432|   20000|
	|    2|     20500| 2010|     328|   40000|
	|    3|     61500| 2013|     495|   30000|
	|    4|     18990| 2013|     280|   30000|
	|    5|     21800| 2004|     306|   40000|
	------------------------------------------
	Please give a number from 1 to 10 (i.e., 1, 2, .., 10) 
	to indicate how bored you feel when you are asked with 
	12 questions for this round in order to obtain 5 out of 
	your 20 most favorite cars (Note: 10 denotes you feel 
	the most bored and 1 denotes you feel the least bored.)
	You answer:

b. Rank
(1).The system will ask you to compare different cases in 3 rounds, where
	Round 1 is for different cases of algorithm RH, 
	Round 2 is for different cases of algorithm HDPI_sampling 
	and Round 3 is for different cases of algorithm HDPI_accurate. 
	
(2).In each round (for each algorithm), you will be presented 5 cases (i.e., returning 
	1, 5, 10, 15, 20 out of top-20 used cars).
	Each case shows the number of questions asked and recommended used cars and you need to 
	give a rank for the cases to show your preference on the 5 cases.
	For example, you might see:
	========================= Result 1 (1 car shown in the output) ==========================
	The number of questions asked: 3 
	Recommended cars:
	------------------------------------------     ------------------------------------------
	| Cars|Price(USD)| Year| PowerPS| Used KM|     | Cars|Price(USD)| Year| PowerPS| Used KM|
	------------------------------------------     ------------------------------------------
	|    1|     28800| 2013|     432|   20000|     
	------------------------------------------     ------------------------------------------

	========================= Result 2 (5 cars shown in the output) =========================
	The number of questions asked: 12 
	Recommended cars:
	------------------------------------------     ------------------------------------------
	| Cars|Price(USD)| Year| PowerPS| Used KM|     | Cars|Price(USD)| Year| PowerPS| Used KM|
	------------------------------------------     ------------------------------------------
	|    1|     28800| 2013|     432|   20000|     |    2|     61500| 2013|     495|   30000|
	|    3|     18990| 2013|     280|   30000|     |    4|     23350| 2010|     435|  125000|
	|    5|     20500| 2010|     328|   40000|     
	------------------------------------------     ------------------------------------------

	========================= Result 3 (10 cars shown in the output) =========================
	The number of questions asked: 18 
	Recommended cars:
	------------------------------------------     ------------------------------------------
	| Cars|Price(USD)| Year| PowerPS| Used KM|     | Cars|Price(USD)| Year| PowerPS| Used KM|
	------------------------------------------     ------------------------------------------
	    1|     28800| 2013|     432|   20000|     |    2|     61500| 2013|     495|   30000|
	|    3|     23350| 2010|     435|  125000|     |    4|     16800| 2005|     500|  125000|
	|    5|     18990| 2013|     280|   30000|     |    6|     11000| 2014|     185|   20000|
	|    7|     20500| 2010|     328|   40000|     |    8|     21500| 2012|     271|   60000|
	|    9|     33900| 2015|     230|   20000|     |   10|     24999| 2015|     192|   20000|
	------------------------------------------     ------------------------------------------

	========================= Result 4 (15 cars shown in the output) =========================
	The number of questions asked: 24 
	Recommended cars:
	------------------------------------------     ------------------------------------------
	| Cars|Price(USD)| Year| PowerPS| Used KM|     | Cars|Price(USD)| Year| PowerPS| Used KM|
	------------------------------------------     ------------------------------------------
	|    1|     28800| 2013|     432|   20000|     |    2|     61500| 2013|     495|   30000|
	|    3|     23350| 2010|     435|  125000|     |    4|     16800| 2005|     500|  125000|
	|    5|     18990| 2013|     280|   30000|     |    6|     20500| 2010|     328|   40000|
	|    7|     49800| 2015|     306|   30000|     |    8|     11000| 2014|     185|   20000|
	|    9|     33900| 2015|     230|   20000|     |   10|     21500| 2012|     271|   60000|
	|   11|     24999| 2015|     192|   20000|     |   12|     47999| 2014|     313|   60000|
	|   13|     36500| 2015|     230|   20000|     |   14|     25750| 2011|     300|   60000|
	|   15|     15000| 2012|     245|   70000|     
	------------------------------------------     ------------------------------------------

	========================= Result 5 (20 cars shown in the output) =========================
	The number of questions asked: 25 
	Recommended cars:
	------------------------------------------     ------------------------------------------
	| Cars|Price(USD)| Year| PowerPS| Used KM|     | Cars|Price(USD)| Year| PowerPS| Used KM|
	------------------------------------------     ------------------------------------------
	|    1|     28800| 2013|     432|   20000|     |    2|     61500| 2013|     495|   30000|
	|    3|     23350| 2010|     435|  125000|     |    4|     18990| 2013|     280|   30000|
	|    5|     16800| 2005|     500|  125000|     |    6|     20500| 2010|     328|   40000|
	|    7|     49800| 2015|     306|   30000|     |    8|     11000| 2014|     185|   20000|
	|    9|     33900| 2015|     230|   20000|     |   10|     47999| 2014|     313|   60000|
	|   11|     24999| 2015|     192|   20000|     |   12|     85911| 2008|     575|   70000|
	|   13|     36500| 2015|     230|   20000|     |   14|     30210| 2011|     306|   50000|
	|   15|     25750| 2011|     300|   60000|     |   16|     15400| 2015|     150|   20000|
	|   17|     21500| 2012|     271|   60000|     |   18|     15000| 2012|     245|   70000|
	|   19|     21900| 2009|     355|  125000|     |   20|     12000| 2014|     150|   20000|
	------------------------------------------     ------------------------------------------
	Please give your preferences on these results where the result you like the most is placed at 
	the leftmost and so on (e.g., when you write in this format "1 2 3 4 5", you mean that 
	you prefer Result 1 the most Result 2 the second, and so on).
	Note that when you give your preferences, you should consider the number of cars shown in the 
	output and your satisfaction of the cars returned.
	For example, some users do not prefer seeing many results in the output (since they may be asked
	many questions) and just 1-2 results in the output are enough. Some users prefer seeing fewer 
	results (e.g., 1-2) since they may be asked much fewer questions and these results are sufficient
	to their need.
	(Result 1)  3 questions asked && 1 car shown in the output;  (Result 2)  12 questions asked && 5 cars shown in the output;
	(Result 3) 18 questions asked && 10 cars shown in the output; (Result 4)  24 questions asked && 15 cars shown in the output;
	(Result 5) 25 questions asked && 20 cars shown in the output; 
	Your answer: 
	
Appendix C. Format of User Log
------------------------------------
	The user log file is shown in output/test.txt.
	It contains two parts: (a) Results of cases and (b) Results of rank

a.  Results of cases
(1).You will see the 15 results, where 
	result 1 is the result of returning 1 out of top-20 used cars by algorithm RH,
	result 2 is the result of returning 5 out of top-20 used cars by algorithm RH,
	result 3 is the result of returning 10 out of top-20 used cars by algorithm RH,
	result 4 is the result of returning 15 out of top-20 used cars by algorithm RH,
	result 5 is the result of returning 20 out of top-20 used cars by algorithm RH,
	result 6 is the result of returning 1 out of top-20 used cars by algorithm HDPI_sampling,
	result 7 is the result of returning 5 out of top-20 used cars by algorithm HDPI_sampling,
	result 8 is the result of returning 10 out of top-20 used cars by algorithm HDPI_sampling,
	result 9 is the result of returning 15 out of top-20 used cars by algorithm HDPI_sampling,
	result 10 is the result of returning 20 out of top-20 used cars by algorithm HDPI_sampling,
	result 11 is the result of returning 1 out of top-20 used cars by algorithm HDPI_accurate,
	result 12 is the result of returning 5 out of top-20 used cars by algorithm HDPI_accurate,
	result 13 is the result of returning 10 out of top-20 used cars by algorithm HDPI_accurate,
	result 14 is the result of returning 15 out of top-20 used cars by algorithm HDPI_accurate,
	and result 15 is the result of returning 20 out of top-20 used cars by algorithm HDPI_accurate.
	
(2).For each result, lines 1-3 show the algorithm name and the number of questions asked.
	Then the recommended used cars are shown.
	The last line shows the evaluation result.
	For example, you might see:
	--------------------------------------------------------
	Algorithm: RH
	The number of questions asked:  16 
	Recommended cars:
	--------------------------------------------------------
	|  5 Top-20|Price(USD)|      Year|   PowerPS|   Used KM|
	---------------------------------------------------------
	|       Car|     61500|      2013|       495|     30000|
	|       Car|     85911|      2008|       575|     70000|
	|       Car|     28800|      2013|       432|     20000|
	|       Car|     95400|      2013|       400|     50000|
	|       Car|     92000|      2013|       400|     50000|
	---------------------------------------------------------
	Degree of boredness 5
	
b. Results of rank
(1).You will see the 3 results, where
	result 1 is the ranking of 5 cases (i.e., returning 1, 5, 10, 15, 20 out of top-20 used cars) of algorithm RH,
	result 2 is the ranking of 5 cases (i.e., returning 1, 5, 10, 15, 20 out of top-20 used cars) of algorithm HDPI_sampling,
	and result 3 is the ranking of 5 cases (i.e., returning 1, 5, 10, 15, 20 out of top-20 used cars) of algorithm HDPI_accurate.

(2).In each result, there are 5 indexes shown, each of which represents a case, where
	1 represents the case of returning 1 out of top-20 used cars,
	2 represents the case of returning 5 out of top-20 used cars,
	3 represents the case of returning 10 out of top-20 used cars,
	4 represents the case of returning 15 out of top-20 used cars,
	and 5 represents the case of returning 20 out of top-20 used cars.
	The index the user favors the most is placed at the leftmost and so on.
	For example, you might see:
	Rank: 1 2 3 4 5
