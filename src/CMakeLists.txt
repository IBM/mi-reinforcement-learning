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

# Default length of SDR.
set(DEFAULT_SDR_LENGTH 1024 CACHE STRING "Default length of SDR")
add_definitions(-DDEFAULT_SDR_LENGTH=${DEFAULT_SDR_LENGTH})

set(BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE ON CACHE BOOL "Build the application implementing histogram filter based solution of the maze-of-digits problem.")


# =======================================================================
# Build MI Reinforcement Learning Apps.
# =======================================================================

if(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE})
	# Create exeutable.
	add_executable(maze_of_digits_histogram_filter_app 
		applications/HistogramFilterMazeLocalization.cpp
		applications/console_application_main.cpp
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
		visualization_opengl ${OPENGL_LIBRARIES} ${GLUT_LIBRARY}
		${Boost_LIBRARIES}  
		)

	# install test to bin directory
	install(TARGETS maze_of_digits_histogram_filter_app RUNTIME DESTINATION bin)
	
endif(${BUILD_APP_HISTOGRAM_FILTER_MAZE_OF_DIGITS_SIMPLE})

