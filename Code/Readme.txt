Readme (Interactive Search for One of the Top-k)
=========================
This package contains 5 directories 
1. Directory "InteractiveTopk", which contains the source codes of the proposed algorithms
	(1). 2D-PI (only works for 2-dimensional datasets)
	(2). HD-PI (sampling and accurate)
	(3). RH
2. Directory "ExistingAlg", which contains the source codes of the baselines
	(1). Active-Ranking
	(2). Preference-Learning
	(3). Median (only works for 2-dimensional datasets)
	(4). Median-Adapt (only works for 2-dimensional datasets)
	(5). Hull (only works for 2-dimensional datasets)
	(6). Hull-Adapt (only works for 2-dimensional datasets)
	(7). UH-Simplex
	(8). UH-Simplex-Adapt
	(9). UH-Random
	(10). UH-Random-Adapt
	(11). UtilityApprox
3. Directory "All_Top-k", which contains the source codes of the proposed algorithms for motivation study (return all the top-k points)
	(1). HD-PI (AllTopK) (sampling and accurate)
	(2). RH (AllTopK)
4. Directory "User_study", which contains the source codes for user study
	(1). UH-Simplex
	(2). Preference-Learning
	(3). RH
	(4). UH-Random
	(5). Active-Ranking
	(6). HDPI (sampling and accurate)
5. Directory "User_study(motivation)", which contains the source codes for user study (for motivation)
	(1). RH (SomeTopK) (returning k' out of top-k points, where k'<=k)
	(2). HDPI (sampling and accurate) (SomeTopK) (returning k' out of top-k points, where k'<=k)