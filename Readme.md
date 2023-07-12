# Robust Best Point Selection under Unreliable User Feedback


This package contains 5 directories under `Code`: 
1. Directory `PersistErr`, which contains the source code of the proposed algorithms.

	(1). *FC* (can be found in directory `optimal`)
  
	(2). *SE-random* and *SE-score* (can be found in directory `exact`)
  
	(3). *SS-random* and *SS-score* (can be found in directory `sampling`)
  
2. Directory `ExistingAlg`, which contains the source code of (adapted) existing algorithms.

	(1). *HD-PI*

	(2). *UtilApprox*

	(3). *Verify-Point*

	(4). *Pref-Learning*

	(5). *UH-Simplex*

	(6). *Active-Ranking*

3. Directory `quest_type`, which contains the source code for variants of *FC*, *SE-random*, *SE-score*, *SS-random* and *SS-score*. The following variants are provided:
	
	(3). Display *s* points in each round and ask the user to select the favorite point (instead of pairwise comparison)
	
	(4). Display *s* points in each round and ask the user to partition them into *superior* and *inferior* groups (instead of pairwise comparison)

4. Directory `user_study` and `user_study_persist`, which contains the source code for user study.

	The following algorithms are included in the user study:

	(1). *SE* (ours)
  
	(2). *Verify-Space* (ours)
  
	(3). *Preference-Learning*
  
	(4). *HDPI*
  
	(5). *Active-Ranking*
  
	(6). *UH-Simplex*

