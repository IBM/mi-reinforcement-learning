Machine Intelligence Core: Reinforcement Learning
=========================================

Description
-----------

A subproject of Machine Intelligence Core framework.

The repository contains solutions and applications related to reinforcement learning.

MIC dependencies
------------
   * MIToolchain - the core of MIC framework.
   * MIAlgorithms - contains basic (core) types and algorithms.
   * MIVisualization - contains OpenGL-based visualization.

External dependencies
------------
Additionally it depends on the following external libraries:
   * Boost - library of free (open source) peer-reviewed portable C++ source libraries.
   * Eigen - a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.

Applications
------------
   *  maze_of_digits_histogram_filter_app - application implementing histogram filter based solution of the maze-of-digits problem.
   *  maze_of_digits_histogram_filter_episodic_app - application for episodic testing of convergence of histogram filter based maze-of-digits localization.


Installation
------------
```
git clone git@github.rtp.raleigh.ibm.com:tkornut-us/mi-rl.git
cd mi-rl
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=~/Documents/workspace/mic/
make -j4 install
```

Maintainer
----------
tkornuta

