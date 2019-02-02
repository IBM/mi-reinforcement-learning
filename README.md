# Machine Intelligence Core: Reinforcement Learning

![Language](https://img.shields.io/badge/language-C%2B%2B-blue.svg)
[![GitHub license](https://img.shields.io/github/license/IBM/mi-reinforcement-learning.svg)](https://github.com/IBM/mi-reinforcement-learning/blob/master/LICENSE)
![](https://img.shields.io/github/release/IBM/mi-reinforcement-learning.svg)
[![Build Status](https://travis-ci.com/IBM/mi-reinforcement-learning.svg?branch=master)](https://travis-ci.com/IBM/mi-reinforcement-learning)
[![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/IBM/mi-reinforcement-learning.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/IBM/mi-reinforcement-learning/context:cpp)
[![Total alerts](https://img.shields.io/lgtm/alerts/g/IBM/mi-reinforcement-learning.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/IBM/mi-reinforcement-learning/alerts/)

## Description

A subproject of Machine Intelligence Core (MIC) framework.

The repository contains solutions and applications related to (deep) reinforcement learning.
In particular, it contains several classical problems (N-armed bandits, several variations of Gridworld), POMDP environments (Gridworld, Maze of Digits, MNIST digit) and algorithms (from simple Value Iteartion and Q-learning to DQN with Experience Replay).


### Classic RL Applications
   * narmed_bandits_unlimited_history_app - application solving the n armed bandits problem based on unlimited history action selection (storing all action-value pairs).
   * narmed_bandits_simple_qlearning_app - application solving the n armed bandits problem using simple Q-learning rule.
   * narmed_bandits_softmax_app - application solving the n armed bandits problem using Softmax Action Selection.
   * gridworld_value_iteration_app - application solving the gridworld problem by applying the reinforcement learning value iteration method.
   * gridworld_qlearning_app - application solving the gridworld problem with Q-learning.

### Other RL & DRL POMDP Applications
   * gridworld_drl_app - application solving the gridworld problem with Q-learning and (not that) deep neural networks.
   * gridworld_drl_er_app - application solving the gridworld problem with Q-learning, neural network used for approximation of the rewards and experience replay using for (batch) training of the neural network.
   * gridworld_drl_er_pomdp_app - application solving the gridworld with partial observation and Deep Reinforcement Learning with Experience Replay.
   * mazeofdigits_histogram_filter_app - application implementing histogram filter based solution of the maze-of-digits problem.
   * mazeofdigits_histogram_filter_episodic_app - application for episodic testing of convergence of histogram filter based maze-of-digits localization.
   * mazeofdigits_drl_er_pomdp_app - application solving the maze of digits with partial observation and Deep Reinforcement Learning with Experience Replay.
   * mnist_digit_drl_er_pomdp_app - application solving the MNIST digit patch localization proble with partial observation and Deep Reinforcement Learning with Experience Replay.


## MIC dependencies
   * [MI-toolchain](https://github.com/IBM/mi-toolchain) - the core of MIC framework.
   * [MI-algorithms](https://github.com/IBM/mi-algorithms) - contains basic (core) types and algorithms.
   * [MI-visualization](https://github.com/IBM/mi-visualization) - contains OpenGL-based visualization.
   * [MI-neural-nets](https://github.com/IBM/mi-neural-nets) - contains implementation of (deep) neural nets.


## External dependencies

Additionally it depends on the following external libraries:
   * Boost - library of free (open source) peer-reviewed portable C++ source libraries.
   * Eigen - a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.
   * OpenGL/GLUT - a cross-language, cross-platform application programming interface for rendering 2D and 3D vector graphics.
   * OpenBlas (optional) - An optimized library implementing BLAS routines. If present - used for fastening operation on matrices.
   * Doxygen (optional) - Tool for generation of documentation.
   * GTest (optional) - Framework for unit testing.

### Installation of the dependencies/required tools

#### On Linux (Ubuntu 14.04):

    sudo apt-get install git cmake doxygen libboost1.54-all-dev libeigen3-dev freeglut3-dev libxmu-dev libxi-dev

To install GTest on Ubuntu:

    sudo apt-get install libgtest-dev

#### On Mac (OS X 10.14): (last tested on: Feb/01/2019)

    brew install git cmake doxygen boost eigen glfw3

To install GTest on Mac OS X:

    brew install --HEAD https://gist.githubusercontent.com/Kronuz/96ac10fbd8472eb1e7566d740c4034f8/raw/gtest.rb

### Installation of all MIC dependencies (optional)

This step is required only when not downloaded/installed the listed MIC dependencies earlier.

In directory scripts one can find script that will download and install all required MIC modules.

    git clone git@github.com:IBM/mi-reinforcement-learning.git
    cd mi-reinforcement-learning
    ./scripts/build_mic_module.sh ../mic

Please note that it will create a directory 'deps' and download all sources into that directory.
After compilation all dependencies will be installed in the directory '../mic'.

### Installation of MI-reinforcement-learning
The following assumes that all MIC dependencies are installed in the directory '../mic'.

    git clone git@github.com:IBM/mi-reinforcement-learning.git
    cd mi-reinforcement-learning
    ./scripts/build_mic_module.sh ../mic

### Make commands

   * make install - install applications to ../mic/bin, headers to ../mic/include, libraries to ../mic/lib, cmake files to ../mic/share
   * make configs - install config files to ../mic/bin
   * make datasets - install config files to ../mic/datasets

## Documentation

In order to locally generate a "living" documentation of the code please run Doxygen:

    cd ~/workspace/mi-reinforcement-learning
    doxygen mi-reinforcement-learning.doxyfile
    firefox html/index.html

The current documentation (generated straight from the code and automatically uploaded to github pages by Travis) is available at:

https://ibm.github.io/mi-reinforcement-learning/

## Maintainer

[tkornuta][github.com/tkornut]

[![HitCount](http://hits.dwyl.io/tkornut/ibm/mi-reinforcement-learning.svg)](http://hits.dwyl.io/tkornut/ibm/mi-reinforcement-learning)