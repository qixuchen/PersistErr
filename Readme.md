# Robust Best Point Selection under Unreliable User Feedback


This package contains 5 directories under `Code`: 
1. Directory `PersistErr`, which contains the source code of the proposed algorithms.

	(1). *FC* (can be found in directory `optimal`)
    
	(2). *SS-random* and *SS-score* (can be found in directory `sampling`)
  
2. Directory `ExistingAlg`, which contains the source code of (adapted) existing algorithms.

	(1). *HD-PI*

	(2). *UtilApprox*

	(3). *Verify-Point*

	(4). *Pref-Learn*

	(5). *UH-Simplex*

	(6). *Active-Ranking*

3. Directory `quest_type`, which contains the source code for variants of *FC*, *SS-random* and *SS-score*. The following variants are provided:
	
	(1). Display *s* points in each round and ask the user to select the favorite point (instead of pairwise comparison)
	
	(2). Display *s* points in each round and ask the user to partition them into *superior* and *inferior* groups (instead of pairwise comparison)

4. Directories that starts with `user_study` contains the source code for user study.

	The following algorithms are included in the user study:
  
	(1). *SS-score* (ours)

    (2). *Verify-Point*
   
    (3). *HD-PI*
  
	(4). *Pref-Learn*
  
	(5). *Active-Ranking*


5. Directory `input` contains all datasets used in our paper.
