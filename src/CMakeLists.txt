# Check compiler flags
include(CheckCXXCompilerFlag)

# Include current dir
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# =======================================================================
# Set compiler/linker flags.
# =======================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

# Check, whether all necessary libraries are linked
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl") 

# =======================================================================
# Build probabilistic localization (histogram filter) related apps.
# =======================================================================

set(BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE ON CACHE BOOL "Build the application implementing histogram filter based solution of the maze-of-digits problem.")

if(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE})
	# Create exeutable.
	add_executable(maze_of_digits_histogram_filter_app 
		application/HistogramFilterMazeLocalization.cpp
		application/console_application_main.cpp
		algorithms/MazeHistogramFilter.cpp
		data_io/MazeMatrixImporter.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(maze_of_digits_histogram_filter_app 
		logger
		configuration
		application
		types
		data_utils
		data_io
		encoders
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS maze_of_digits_histogram_filter_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE})


set(BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_EPISODIC ON CACHE BOOL "Build the application for episodic testing of convergence of histogram filter based maze-of-digits localization.")

if(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_EPISODIC})
	# Create exeutable.
	add_executable(maze_of_digits_histogram_filter_episodic_app 
		application/EpisodicHistogramFilterMazeLocalization.cpp
		application/console_application_main.cpp
		algorithms/MazeHistogramFilter.cpp
		data_io/MazeMatrixImporter.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(maze_of_digits_histogram_filter_episodic_app 
		logger
		configuration
		application
		types
		data_utils
		data_io
		encoders
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS maze_of_digits_histogram_filter_episodic_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_EPISODIC})

# =======================================================================
# Build n-armed bandits related apps.
# =======================================================================

set(BUILD_APP_N_ARMED_BANDITS_UNLIMITED_HISTORY ON CACHE BOOL "Build the application solving the n armed bandits problem based on unlimited history action selection (storing all action-value pairs).")

if(${BUILD_APP_N_ARMED_BANDITS_UNLIMITED_HISTORY})
	# Create exeutable.
	add_executable(narmed_bandits_unlimited_history_app 
		application/nArmedBanditsUnlimitedHistory.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(narmed_bandits_unlimited_history_app 
		logger
		configuration
		application
		data_utils
		types
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS narmed_bandits_unlimited_history_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_N_ARMED_BANDITS_UNLIMITED_HISTORY})


set(BUILD_APP_N_ARMED_BANDITS_SIMPLE_QLEARNING ON CACHE BOOL "Build the application solving the n armed bandits problem using simple Q-learning rule.")

if(${BUILD_APP_N_ARMED_BANDITS_SIMPLE_QLEARNING})
	# Create exeutable.
	add_executable(narmed_bandits_simple_qlearning_app 
		application/nArmedBanditsSimpleQlearning.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(narmed_bandits_simple_qlearning_app 
		logger
		configuration
		application
		data_utils
		types
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS narmed_bandits_simple_qlearning_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_N_ARMED_BANDITS_SIMPLE_QLEARNING})


set(BUILD_APP_N_ARMED_BANDITS_SOFTMAX ON CACHE BOOL "Build the application solving the n armed bandits problem using Softmax Action Selection.")

if(${BUILD_APP_N_ARMED_BANDITS_SOFTMAX})
	# Create exeutable.
	add_executable(narmed_bandits_softmax_app 
		application/nArmedBanditsSofmax.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(narmed_bandits_softmax_app 
		logger
		configuration
		application
		data_utils
		types
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS narmed_bandits_softmax_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_N_ARMED_BANDITS_SOFTMAX})

# =======================================================================
# Build gridworld related apps.
# =======================================================================

set(BUILD_APP_GRIDWORLD_VALUE_ITERATION ON CACHE BOOL "Build the application solving the gridworld problem by applying the reinforcement learning value iteration method.")

if(${BUILD_APP_GRIDWORLD_VALUE_ITERATION})
	# Create exeutable.
	add_executable(gridworld_value_iteration_app 
		application/GridworldValueIteration.cpp
		types/Gridworld.cpp
		types/Environment.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(gridworld_value_iteration_app 
		logger
		configuration
		application
		types
		data_utils
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS gridworld_value_iteration_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_GRIDWORLD_VALUE_ITERATION})



set(BUILD_APP_GRIDWORLD_QLEARNING ON CACHE BOOL "Build the application solving the gridworld problem with Q-learning.")

if(${BUILD_APP_GRIDWORLD_QLEARNING})
	# Create exeutable.
	add_executable(gridworld_qlearning_app 
		application/GridworldQLearning.cpp
		types/Gridworld.cpp
		types/Environment.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(gridworld_qlearning_app 
		logger
		configuration
		application
		types
		data_utils
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS gridworld_qlearning_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_GRIDWORLD_QLEARNING})


set(BUILD_APP_GRIDWORLD_DEEPQLEARNING ON CACHE BOOL "Build the application solving the gridworld problem with Q-learning and (not that) deep neural networks.")

if(${BUILD_APP_GRIDWORLD_DEEPQLEARNING})
	# Create exeutable.
	add_executable(gridworld_drl_app 
		application/GridworldDeepQLearning.cpp
		types/Gridworld.cpp
		types/Environment.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(gridworld_drl_app 
		logger
		configuration
		application
		types
		data_utils
		mlnn
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS gridworld_drl_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_GRIDWORLD_DEEPQLEARNING})


set(BUILD_APP_GRIDWORLD_DEEPQLEARNING_EXPERIENCEREPLAY ON CACHE BOOL "Build the application solving the gridworld problem with Q-learning, neural network used for approximation of the rewards and experience replay using for (batch) training of the neural network.")

if(${BUILD_APP_GRIDWORLD_DEEPQLEARNING_EXPERIENCEREPLAY})
	# Create exeutable.
	add_executable(gridworld_drl_er_app 
		application/GridworldDRLExperienceReplay.cpp
		types/Gridworld.cpp
		types/Environment.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(gridworld_drl_er_app 
		logger
		configuration
		application
		types
		data_utils
		mlnn
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS gridworld_drl_er_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_GRIDWORLD_DEEPQLEARNING_EXPERIENCEREPLAY})


