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
# Add CMAKE variables.
# =======================================================================

set(BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE ON CACHE BOOL "Build the application implementing histogram filter based solution of the maze-of-digits problem.")

set(BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_EPISODIC ON CACHE BOOL "Build the application for episodic testing of convergence of histogram filter based maze-of-digits localization.")

set(BUILD_APP_N_ARMED_BANDITS_UNLIMITED_HISTORY ON CACHE BOOL "Build the application solving the n armed bandits problem based on unlimited history action selection (storing all action-value pairs).")

set(BUILD_APP_N_ARMED_BANDITS_SIMPLE_QLEARNING ON CACHE BOOL "Build the application solving the n armed bandits problem using simple Q-learning rule.")

set(BUILD_APP_N_ARMED_BANDITS_SOFTMAX ON CACHE BOOL "Build the application solving the n armed bandits problem using Softmax Action Selection.")

# =======================================================================
# Build MI Reinforcement Learning Apps.
# =======================================================================

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
		application
		types
		data_utils
		data_io
		auto_encoders
		classifiers
		auto_encoders
		configuration
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS maze_of_digits_histogram_filter_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE})


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
		application
		types
		data_utils
		data_io
		auto_encoders
		classifiers
		auto_encoders
		configuration
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS maze_of_digits_histogram_filter_episodic_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_EPISODIC})



if(${BUILD_APP_N_ARMED_BANDITS_UNLIMITED_HISTORY})
	# Create exeutable.
	add_executable(narmed_bandits_unlimited_history_app 
		application/nArmedBanditsUnlimitedHistory.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(narmed_bandits_unlimited_history_app 
		logger
		application
		data_utils
		types
		configuration
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS narmed_bandits_unlimited_history_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_N_ARMED_BANDITS_UNLIMITED_HISTORY})


if(${BUILD_APP_N_ARMED_BANDITS_SIMPLE_QLEARNING})
	# Create exeutable.
	add_executable(narmed_bandits_simple_qlearning_app 
		application/nArmedBanditsSimpleQlearning.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(narmed_bandits_simple_qlearning_app 
		logger
		application
		data_utils
		types
		configuration
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS narmed_bandits_simple_qlearning_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_N_ARMED_BANDITS_SIMPLE_QLEARNING})


if(${BUILD_APP_N_ARMED_BANDITS_SOFTMAX})
	# Create exeutable.
	add_executable(narmed_bandits_softmax_app 
		application/nArmedBanditsSofmax.cpp
		application/console_application_main.cpp
		)
	# Link it with shared libraries.
	target_link_libraries(narmed_bandits_softmax_app 
		logger
		application
		data_utils
		types
		configuration
		visualization_opengl
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS narmed_bandits_softmax_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_N_ARMED_BANDITS_SOFTMAX})


