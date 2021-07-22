Target solution: CMake Mac

Building the Autotester:
1. To build using CMake, open a terminal and enter the following commands:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Code21
	- (If CMakeCache.txt is within the folder,) rm CMakeCache.txt
	- mkdir build
	- cd build
	- cmake ..
	- make

2. Wait for the build to complete. The autotester executable should be located under src/autotester in the build folder.


Running the System Tests:
1. In the same terminal, navigate to Team21/Tests21 to navigate to all the system tests with the following command:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21
2. Run the respective bash script file for system test on iteration 1, 2 or 3 by executing the following command on terminal

To run all system test for iteration 1,2,3 in one:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21	
	- bash run_all_tests.sh
	
To run iteration 1 system test only:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_1	
	- bash run_all_iter1_test.sh

To run iteration 2 system test only:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_2	
	- bash run_all_iter2_test.sh

To run iteration 3 system test only:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_3	
	- bash run_all_iter3_test.sh


3. The result xml files should be generated in their respective directories named "out"

To check the result xml files for Iteration 1:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_1/out	
	- The result xml files produced are:
		1.out_no_clause.xml
		2.out_Follows_Parent.xml
	  	3.out_pattern_ModifiesS_UsesS.xml
	  	4.out_pattern_suchthat_clauses.xml
	  	5.out_complex_condexpr_nested.xml
	  	6.out_uncommon_invalid.xml
		7.out_bonus_3_features.xml

To check the result xml files for Iteration 2:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_2/out/valid
	- The result xml files produced are:
		1.out_valid_Select_clause.xml
		2.out_valid_With_clause.xml
		3.out_simple_Calls.xml
		4a.out_valid_Calls.xml
		4b.out_valid_Next.xml
		4c.out_valid_ModifiesP_UsesP.xml
		5.out_valid_pattern_assign.xml
		6.out_valid_pattern_full_spec.xml
		7.out_multiclauses1.xml

	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_2/out/invalid
	- The result xml files produced are:
		1.out_invalid_Select_clause.xml
		2.out_invalid_With_clause.xml
		4a.out_invalid_Calls.xml
		4b.out_invalid_Next.xml
		4c.out_invalid_ModifiesP_UsesP.xml
		5.out_invalid_pattern_assign.xml
		6.out_invalid_pattern_full_spec.xml

To check the result xml files for Iteration 3:
	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_3/out/valid
	- The result xml files produced are:
		1.out_valid_simple_affects.xml
		2.out_valid_simple_affectsStar.xml
		3a.out_valid_complex_affects.xml
		3b.out_valid_complex_affects_star.xml
		4.out_multiclauses1.xml
		5.out_valid_multiclause_c1.xml
		6.out_valid_multiclause_c2.xml
		7.out_valid_multiclause_c3.xml
		8.out_valid_multiclause_c4.xml
		9.out_multiclause_stress_test.xml
		10.out_nested_stress_test.xml

	- cd <PATH_TO_PROJECT_DIRECTORY>/Team21/Tests21/Iteration_3/out/invalid
	- The result xml files produced are:
		1.out_invalid_simple_affects.xml
		2.out_invalid_simple_affectsStar.xml
		3a.out_invalid_complex_affects.xml
		3b.out_invalid_complex_affects_star.xml
		4.out_invalid_multiclause.xml