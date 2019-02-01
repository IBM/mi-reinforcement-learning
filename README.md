# Machine Intelligence Core: Reinforcement Learning

![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
[![GitHub license](https://img.shields.io/github/license/IBM/mi-reinforcement-learning.svg)](https://github.com/IBM/mi-reinforcement-learning/blob/master/LICENSE)
![](https://img.shields.io/github/release/IBM/mi-reinforcement-learning.svg)
[![Build Status](https://travis-ci.com/IBM/mi-reinforcement-learning.svg?branch=master)](https://travis-ci.com/IBM/mi-reinforcement-learning)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/IBM/mi-reinforcement-learning.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/IBM/mi-reinforcement-learning/context:cpp)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/IBM/mi-reinforcement-learning.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/IBM/mi-reinforcement-learning/alerts/)

## Description

A subproject of Machine Intelligence Core framework.

The repository contains solutions and applications related to reinforcement learning.

## MIC dependencies
   * MI-Toolchain - the core of MIC framework.
   * MI-Algorithms - contains basic (core) types and algorithms.
   * MI-Visualization - contains OpenGL-based visualization.
   * MI-Neural-Nets - contains implementation of (deep) neural nets.

## External dependencies
Additionally it depends on the following external libraries:
   * Boost - library of free (open source) peer-reviewed portable C++ source libraries.
   * Eigen - a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.

## Applications
   *  maze_of_digits_histogram_filter_app - application implementing histogram filter based solution of the maze-of-digits problem.
   *  maze_of_digits_histogram_filter_episodic_app - application for episodic testing of convergence of histogram filter based maze-of-digits localization.


## Installation
```
git clone git@github.rtp.raleigh.ibm.com:tkornut-us/mi-rl.git
cd mi-rl
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/Documents/workspace/mic/
make -j4 install
```

## Documentation

In order to generate a "living" documentation of the code please run Doxygen:

    cd ~/workspace/mi-reinforcement-learning
    doxygen mi-reinforcement-learning.doxyfile
    firefox html/index.html

The current documentation (generated straight from the code and automatically uploaded to github pages by Travis) is available at:

https://ibm.github.io/mi-reinforcement-learning/

Maintainer
----------
tkornuta


