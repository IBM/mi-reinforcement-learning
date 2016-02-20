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

#include <types/Action.hpp>


namespace mic {
namespace algorithms {

class MazeHistogramFilter {
public:
	/*!
	 * Constructor
	 * @param mazes_ Vector of mazes.
	 */
	MazeHistogramFilter(std::vector<mic::types::MatrixXiPtr> & mazes_, int hidden_maze_number_, int hidden_x_, int hidden_y_) :
		mazes(mazes_)
	{
		// Set problem dimensions.
		number_of_mazes = mazes.size();
		maze_width = mazes[0]->cols();
		maze_height = mazes[0]->rows();
		problem_dimensions = number_of_mazes * maze_width * maze_height;
		number_of_distinctive_patches = 10;

		// Initialize position.
		// Get "hidden" maze number.
		if (hidden_maze_number_ == -1) {
			hidden_maze_number = RAN_GEN->uniRandInt(0,number_of_mazes-1);
		} else
			hidden_maze_number = hidden_maze_number_ % number_of_mazes;

		// Get "hidden" maze x coordinate.
		if (hidden_x_ == -1) {
			hidden_x = RAN_GEN->uniRandInt(0,maze_width-1);
		} else
			hidden_x = hidden_x_ % maze_width;

		// Get "hidden" maze y coordinate.
		if (hidden_y == -1) {
			hidden_y = RAN_GEN->uniRandInt(0,maze_height-1);
		} else
			hidden_y = hidden_y_ % maze_height;

		LOG(LWARNING) << "After truncation/random: hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";

	}

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




private:
	/// List of mazes.
	std::vector<mic::types::MatrixXiPtr> mazes;

	/// Variable storing the probability that we are in a given maze position.
	std::vector<mic::types::MatrixXdPtr> maze_position_probabilities;

	/// Variable storing the probability that we can find given patch in a given maze.
	std::vector<double> maze_patch_probabilities;

	/// Property: variable denoting in which maze are we right now (unknown, to be determined).
	int hidden_maze_number;

	/// Property: variable denoting the x position are we right now (unknown, to be determined).
	int hidden_x;

	/// Property: variable denoting the y position are we right now (unknown, to be determined).
	int hidden_y;

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

	/// Variable storing the probability that we are currently moving in/observing a given maze.
	std::vector<double> maze_probabilities;

	/// Variable storing the probability that we are currently in a given x coordinate.
	std::vector<double> maze_x_coordinate_probilities;

	/// Variable storing the probability that we are currently in a given y coordinate.
	std::vector<double> maze_y_coordinate_probilities;


};

} /* namespace algorithms */
} /* namespace mic */

#endif /* SRC_ALGORITHMS_MAZEHISTOGRAMFILTER_HPP_ */
