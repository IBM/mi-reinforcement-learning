# Copyright (C) tkornuta, IBM Corporation 2015-2019
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Project name
project(MIReinforcementLearning CXX)

#  Set current version number.
set (MIReinforcementLearning_VERSION_MAJOR 1)
set (MIReinforcementLearning_VERSION_MINOR 3)
set (MIReinforcementLearning_VERSION_PATCH 0)
set (MIReinforcementLearning_VERSION ${MIReinforcementLearning_VERSION_MAJOR}.${MIReinforcementLearning_VERSION_MINOR}.${MIReinforcementLearning_VERSION_PATCH})

# CMake required version.
cmake_minimum_required(VERSION 3.2)

# Add C++11 dependency.
# - fopenmp
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -Wall")


# =======================================================================
# Find required packages
# =======================================================================
# Add path to cmake dir.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Find Boost package
#find_package(Boost 1.54 REQUIRED COMPONENTS system thread random)
find_package(Boost 1.54 REQUIRED COMPONENTS system thread random serialization)
# Try to include Boost as system directory to suppress it's warnings
include_directories(SYSTEM ${Boost_INCLUDE_DIR})

# Find GLUT package
find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})
link_directories(${GLUT_LIBRARY_DIRS})
add_definitions(${GLUT_DEFINITIONS})
if(NOT GLUT_FOUND)
    message(ERROR " GLUT not found!")
endif(NOT GLUT_FOUND)

# Find OPENGL package
find_package(OpenGL REQUIRED)
if(NOT OPENGL_FOUND)
    message(ERROR " OPENGL not found!")
elsif(NOT OPENGL_FOUND)
	include_directories(${OpenGL_INCLUDE_DIRS})
	link_directories(${OpenGL_LIBRARY_DIRS})
	add_definitions(${OpenGL_DEFINITIONS})
endif(NOT OPENGL_FOUND)


# Find Eigen package
find_package( Eigen3 REQUIRED )
include_directories( ${EIGEN3_INCLUDE_DIR} )

# Find MIC Toolchain
find_package(MIToolchain 1.3 REQUIRED)

# Find MIC Algorithms
find_package(MIAlgorithms 1.3 REQUIRED)

# Find MI Neural Nets
find_package(MINeuralNets 1.3 REQUIRED)

# Find MIC Visualization
find_package(MIVisualization 1.3 REQUIRED)

# =======================================================================
# RPATH settings
# =======================================================================
# use, i.e. don't skip the full RPATH for the build tree
SET(CMAKE_SKIP_BUILD_RPATH  FALSE)

# when building, use the install RPATH already
# (but later on when installing)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) 

SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# =======================================================================
# Add subdirectories
# =======================================================================

add_subdirectory(src)

add_subdirectory(configs)

add_subdirectory(datasets)
