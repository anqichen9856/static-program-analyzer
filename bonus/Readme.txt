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
2. Run run_all_iter1_test.sh by executing the following command on terminal
	- bash run_all_iter1_test.sh
3. The script will also run the bonus tests. The output files should be generated within the same folder.