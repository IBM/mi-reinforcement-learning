/*!
 * Copyright (C) tkornuta, IBM Corporation 2015-2019
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file MazeParticleFilter.hpp
 * \brief 
 * \author tkornut
 * \date Feb 19, 2016
 */

#ifndef SRC_ALGORITHMS_MAZEHISTOGRAMFILTER_HPP_
#define SRC_ALGORITHMS_MAZEHISTOGRAMFILTER_HPP_

#include <vector>
#include <types/MatrixTypes.hpp>

#include<logger/Log.hpp>
#include<data_utils/RandomGenerator.hpp>

#include <types/Action2D.hpp>


namespace mic {
namespace algorithms {

/*!
 * \brief Class implementing a histogram filter based solution of the maze-of-digits localization problem.
 * \author tkornuta
 */
class MazeHistogramFilter {
public:
	/*!
	 * Constructor. Resets variables.
	 */
	MazeHistogramFilter();


	/*!
	 * Copies pointer to mazes, sets problem dimensions.
	 * @param mazes_ Vector of mazes.
	 * @param number_of_distinctive_patches_ Number of distinctive patches.
	 */
	void setMazes(std::vector<mic::types::MatrixXiPtr> & mazes_, unsigned int number_of_distinctive_patches_);

	/*!
	 * Sets hidden pose. If required
	 * @param hidden_maze_number_ Hidden maze number (-1 = random)
	 * @param hidden_x_ Maze x coordinate  (-1 = random)
	 * @param hidden_y_ Maze y coordinate  (-1 = random)
	 */
	void setHiddenPose(int hidden_maze_number_, int hidden_x_, int hidden_y_);

	/*!
	 * Destructor. Empty for now.
	 */
	virtual ~MazeHistogramFilter() { }

	/*!
	 * Assigns initial probabilities (uniform distribution) to all variables.
	 */
	void assignInitialProbabilities();

	/*!
	 * Performs "probabilistic" sensing - update probabilities basing on the current observation.
	 * @param hit_factor_ hit factor (the gain when the observation coincides with current position).
	 * @param miss_factor_ miss factor (the gain when the observation does not coincide with current position).
	 */
	void sense (double hit_factor_, double miss_factor_);

	/*!
	 * Performs "deterministic" move.
	 * @param ac_ Performed action.
	 */
	void move (mic::types::Action2DInterface ac_);

	/*!
	 * Performs "probabilistic" move.
	 * @param ac_ Performed action.
	 * @param exact_move_probability_ Probability that we made the exact move (pos+dpos).
	 * @param overshoot_move_probability_ Probability that we made the "overshoot" move (pos+dpos+1).
	 * @param undershoot_move_probability_ Probability that we made the "undershoot" move (pos+dpos-1).
	 */
	void probabilisticMove (mic::types::Action2DInterface ac_, double exact_move_probability_, double overshoot_move_probability_, double undershoot_move_probability_);

	/*!
	 * Updates aggregated probabilities of current maze number, x and y coordinates.
	 */
	void updateAggregatedProbabilities();

	/*!
	 * Selects action based on analysis of current state and patch distributions.
	 * The functions tries to find the maximum action utility, taking into consideration probabilities of being in given maze in given x,y-position.
	 */
	mic::types::Action2DInterface mostUniquePatchActionSelection();


	/*!
	 * Selects action based on analysis of current state and patch distributions.
	 * The functions finds the maximum action utility, summing the results of taking given action taking into account the probabilities of being in given maze in given x,y-position.
	 */
	mic::types::Action2DInterface sumOfMostUniquePatchesActionSelection();


	//void exportStatisticsToCsv()

private:
	/// List of mazes.
	std::vector<mic::types::MatrixXiPtr> mazes;

	/// Problem dimensions - number of mazes.
	unsigned int number_of_mazes;

	/// Problem dimensions - number of distinctive patches (in here - number of different digits, i.e. 10).
	unsigned int number_of_distinctive_patches;

	/// Width of maze.
	unsigned int maze_width;

	/// Height of maze.
	unsigned int maze_height;

	/// Problem dimensions - number of mazes * their width * their height.
	unsigned  int problem_dimensions;

public:

	/// Variable storing the probability that we are in a given maze position.
	std::vector<mic::types::MatrixXdPtr> maze_position_probabilities;

	/// Variable storing the probability that we are currently moving in/observing a given maze.
	std::vector<double> maze_probabilities;

	/// Variable storing the probability that we are currently in a given x coordinate.
	std::vector<double> maze_x_coordinate_probilities;

	/// Variable storing the probability that we are currently in a given y coordinate.
	std::vector<double> maze_y_coordinate_probilities;

	/// Variable storing the probability that we can find given patch in a given maze.
	std::vector<double> maze_patch_probabilities;

	/// shortariable denoting in which maze are we right now (unknown, to be determined).
	int hidden_maze_number;

	/// Variable denoting the x position are we right now (unknown, to be determined).
	int hidden_x;

	/// Variable denoting the y position are we right now (unknown, to be determined).
	int hidden_y;

	/// Current Observation.
	short obs;
};

} /* namespace algorithms */
} /* namespace mic */

#endif /* SRC_ALGORITHMS_MAZEHISTOGRAMFILTER_HPP_ */
