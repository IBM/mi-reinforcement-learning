/*!
 * \file MazeParticleFilter.cpp
 * \brief 
 * \author tkornut
 * \date Feb 19, 2016
 */

#include <algorithms/MazeHistogramFilter.hpp>

#include <data_io/DataCollector.hpp>

namespace mic {
namespace algorithms {


MazeHistogramFilter::MazeHistogramFilter(){
	// Reset variables.
	hidden_maze_number = hidden_y = hidden_y = 0;
	number_of_mazes = maze_width = maze_height = problem_dimensions = number_of_distinctive_patches = 0;

}

void MazeHistogramFilter::setMazes(std::vector<mic::types::MatrixXiPtr> & mazes_, unsigned int number_of_distinctive_patches_)
{
	mazes = mazes_;
	// Set problem dimensions.
	number_of_mazes = mazes.size();
	maze_width = mazes[0]->cols();
	maze_height = mazes[0]->rows();
	problem_dimensions = number_of_mazes * maze_width * maze_height;
	number_of_distinctive_patches = number_of_distinctive_patches_;

	// Create matrices with probabilities.
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::MatrixXdPtr position_probabilities(new mic::types::MatrixXd (maze_height, maze_width));
		maze_position_probabilities.push_back(position_probabilities);
	}

	maze_probabilities.resize(number_of_mazes);
	maze_x_coordinate_probilities.resize(maze_width);
	maze_y_coordinate_probilities.resize(maze_height);

	maze_patch_probabilities.resize(number_of_distinctive_patches);

}


void MazeHistogramFilter::setHiddenPose(int hidden_maze_number_, int hidden_x_, int hidden_y_)
{
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


void MazeHistogramFilter::assignInitialProbabilities() {

	// Assign initial probabilities for all mazes/positions.
	LOG(LNOTICE) << "Initial maze_position_probabilities:";
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::MatrixXdPtr position_probabilities = maze_position_probabilities[m];

		for (size_t i=0; i<maze_height; i++) {
			for (size_t j=0; j<maze_width; j++) {
				(*position_probabilities)(i,j) = (double) 1.0/(problem_dimensions);
			}//: for j
		}//: for i

		LOG(LNOTICE) << "maze_position_prob(" <<m<<"):\n" << (*maze_position_probabilities[m]);
	}//: for m

	// Assign initial probabilities to maze - for visualization.
	for (size_t m=0; m<number_of_mazes; m++) {
		maze_probabilities[m] = ((double) 1.0/ mazes.size());
	}//: for


	//  Assign initial probabilities to x coordinate - for visualization.
	for (size_t x=0; x<maze_width; x++) {
		maze_x_coordinate_probilities[x] = ((double) 1.0/ maze_width);
	}//: for

	//  Assign initial probabilities to y coordinate - for visualization.
	for (size_t y=0; y<maze_height; y++) {
		maze_y_coordinate_probilities[y] = ((double) 1.0/ maze_height);
	}//: for

	// Collect statistics for all mazes - number of appearances of a given "patch" (i.e. digit).
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::MatrixXiPtr maze = mazes[m];

		// Iterate through maze and collect occurrences.
		for (size_t i=0; i<maze_height; i++) {
			for (size_t j=0; j<maze_width; j++) {
				short patch_id = (*maze)(i,j);
				maze_patch_probabilities[patch_id] += 1.0;
			}//: for j
		}//: for i

	}//: for m(azes)

	// Divide by problem dimensions (number of mazes * width * height) -> probabilities.
	LOG(LNOTICE) << "maze_patch_probabilities:";
	for (size_t i=0; i<number_of_distinctive_patches; i++) {
		maze_patch_probabilities[i] /= problem_dimensions;
		LOG(LNOTICE) << "maze_patch_prob(" <<i<<"):\n" << maze_patch_probabilities[i];
	}//: for

}



void MazeHistogramFilter::sense (double hit_factor_, double miss_factor_) {

	// Get observation.
	obs =(*mazes[hidden_maze_number])(hidden_y, hidden_x);
	LOG(LINFO) << "Current observation=" << obs;

	// Compute posterior distribution given Z (observation) - total probability.

	// For all mazes.
	double prob_sum = 0;
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::MatrixXdPtr pos_probs = maze_position_probabilities[m];
		mic::types::MatrixXiPtr maze = mazes[m];

		// Display results.
/*		LOG(LERROR) << "Przed updatem";
		LOG(LERROR) << (*mazes[m]);
		LOG(LERROR) << (*maze_position_probabilities[m]);
*/

		// Iterate through position probabilities and update them.
		for (size_t y=0; y<maze_height; y++) {
			for (size_t x=0; x<maze_width; x++) {
				if ((*maze)(y,x) == obs)
					(*pos_probs)(y,x) *= hit_factor_;
				else
					(*pos_probs)(y,x) *= miss_factor_;
				prob_sum += (*pos_probs)(y,x);
			}//: for j
		}//: for i
	}//: for m

	prob_sum = 1/prob_sum;
	// Normalize probabilities for all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::MatrixXdPtr pos_probs = maze_position_probabilities[m];
		for (size_t i=0; i<maze_height; i++) {
			for (size_t j=0; j<maze_width; j++) {
				(*pos_probs)(i,j) *= prob_sum;
			}//: for j
		}//: for i

		// Display results.
		LOG(LNOTICE) << "maze(" <<m<<"):\n" << (*mazes[m]);
		LOG(LNOTICE) << "maze_pose_prob(" <<m<<"):\n" << (*maze_position_probabilities[m]);
	}//: for m

}

void MazeHistogramFilter::move (mic::types::Action2DInterface ac_) {
	LOG(LINFO) << "Current move dy,dx= ( " << ac_.dy() << "," <<ac_.dx()<< ")";

	// For all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::MatrixXdPtr pos_probs = maze_position_probabilities[m];
		mic::types::MatrixXd old_pose_probs = (*pos_probs);

/*		LOG(LERROR) << "Przed ruchem";
		LOG(LERROR) << (*mazes[m]);
		LOG(LERROR) << (*maze_position_probabilities[m]);*/

		// Iterate through position probabilities and update them.
		for (size_t y=0; y<maze_height; y++) {
			for (size_t x=0; x<maze_width; x++) {
				//std::cout << "i=" << i << " j=" << j << " dx=" << dx_ << " dy=" << dy_ << " (i - dx_) % 3 = " << (i +3 - dx_) % 3 << " (j - dy_) % 3=" << (j + 3 - dy_) % 3 << std::endl;
				(*pos_probs)((y + maze_height +  ac_.dy()) %maze_height, (x +maze_width +  ac_.dx()) % maze_width) = old_pose_probs(y, x);

			}//: for j
		}//: for i

		// Display results.
		LOG(LNOTICE) << "maze(" <<m<<"):\n" << (*mazes[m]);
		LOG(LNOTICE) << "maze_pose_prob(" <<m<<"):\n" << (*maze_position_probabilities[m]);
	}//: for m

	// Perform the REAL move.
	hidden_y = (hidden_y + maze_height +  ac_.dy()) % maze_height;
	hidden_x = (hidden_x + maze_width +  ac_.dx()) % maze_width;

	LOG(LWARNING) << "Hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";

}

void MazeHistogramFilter::probabilisticMove (mic::types::Action2DInterface ac_, double exact_move_probability_, double overshoot_move_probability_, double undershoot_move_probability_) {

	LOG(LINFO) << "Current move dy,dx= ( " << ac_.dy() << "," <<ac_.dx()<< ")";

	// For all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::MatrixXdPtr pos_probs = maze_position_probabilities[m];
		// Make a copy of probabilities.
		mic::types::MatrixXd old_pose_probs = (*pos_probs);
		// Set probabilities to zero.
		(*pos_probs).setZero();

		// Iterate through position probabilities and update them.
		for (size_t y=0; y<maze_height; y++) {
			for (size_t x=0; x<maze_width; x++) {
				size_t exact_y = (y + maze_height +  ac_.dy()) %maze_height;
				size_t overshoot_y = (y + maze_height +  ac_.dy() + 1) %maze_height;
				size_t undershoot_y = (y + maze_height +  ac_.dy() - 1) %maze_height;

				size_t exact_x = (x + maze_width +  ac_.dx()) %maze_width;
				size_t overshoot_x = (x + maze_width +  ac_.dx() + 1) %maze_width;
				size_t undershoot_x = (x + maze_width +  ac_.dx() - 1) %maze_width;

				(*pos_probs)(exact_y, exact_x) += exact_move_probability_  * old_pose_probs(y, x);
				(*pos_probs)(overshoot_y, overshoot_x) += overshoot_move_probability_  * old_pose_probs(y, x);
				(*pos_probs)(undershoot_y, undershoot_x) += undershoot_move_probability_  * old_pose_probs(y, x);

			}//: for j
		}//: for i

		// Display results.
		LOG(LNOTICE) << "maze(" <<m<<"):\n" << (*mazes[m]);
		LOG(LNOTICE) << "maze_pose_prob(" <<m<<"):\n" << (*maze_position_probabilities[m]);
	}//: for m

	// Perform the REAL move.
	hidden_y = (hidden_y + maze_height +  ac_.dy()) % maze_height;
	hidden_x = (hidden_x + maze_width +  ac_.dx()) % maze_width;

	LOG(LWARNING) << "Hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";
}




void MazeHistogramFilter::updateAggregatedProbabilities() {
	// Update maze_probabilities.
	for (size_t m=0; m<number_of_mazes; m++) {
		// Reset probability.
		maze_probabilities[m] = 0;
		mic::types::MatrixXdPtr pos_probs = maze_position_probabilities[m];
		// Sum probabilities of all positions.
		for (size_t i=0; i<maze_height; i++) {
			for (size_t j=0; j<maze_width; j++) {
				maze_probabilities[m] += (*pos_probs)(i,j);
			}//: for j
		}//: for i
	}//: for m

	// Update maze_x_coordinate_probilities.
	for (size_t x=0; x<maze_width; x++) {
		// Reset probability.
		maze_x_coordinate_probilities[x] = 0;
		for (size_t m=0; m<number_of_mazes; m++) {
			mic::types::MatrixXdPtr pos_probs = maze_position_probabilities[m];
			// Sum probabilities of all positions.
			for (size_t y=0; y<maze_height; y++) {
				maze_x_coordinate_probilities[x] += (*pos_probs)(y,x);
				}//: for y
		}//: for m
	}//: for x


	// Update maze_y_coordinate_probilities.
	for (size_t y=0; y<maze_height; y++) {
		// Reset probability.
		maze_y_coordinate_probilities[y] = 0;
		for (size_t m=0; m<number_of_mazes; m++) {
			mic::types::MatrixXdPtr pos_probs = maze_position_probabilities[m];
			// Sum probabilities of all positions.
			for (size_t x=0; x<maze_width; x++) {
				maze_y_coordinate_probilities[y] += (*pos_probs)(y,x);
				}//: for x
		}//: for m
	}//: for y

}



mic::types::Action2DInterface MazeHistogramFilter::mostUniquePatchActionSelection() {
	double best_action_utility = 0.0;
	size_t best_action = -1;

	// Calculate probabilities of all actions.
	for (size_t act_t=0; act_t < 4; act_t++) {
		mic::types::NESWAction ac((types::NESW_action_type_t)act_t);

		// Check the score of a given action.
		for (size_t m=0; m<number_of_mazes; m++) {

			for (size_t y=0; y<maze_height; y++) {
				for (size_t x=0; x<maze_width; x++) {
					// Check result of the next motion.
					// Compute resulting coordinates.
					size_t new_y = (y + maze_height + ac.dy()) % maze_height;
					size_t new_x = (x + maze_width + ac.dx()) % maze_width;
					// Get image patch.
					short patch = (*mazes[m])(new_y, new_x);
					LOG(LDEBUG) << "maze [" << m << "] (y=" << y <<",x="<< x <<") move="<< act_t << "=> (y+dy=" << new_y << ",x+dx=" << new_x <<") patch=" << patch << std::endl;
					// Get patch probability.
					double patch_prob = maze_patch_probabilities[patch];
					// Check the action utility.
					double action_utility = (*maze_position_probabilities[m])(new_y, new_x) * (1- patch_prob);
					LOG(LDEBUG) << "patch_prob= " << patch_prob << " action_utility=" << action_utility << std::endl;
					if (action_utility > best_action_utility) {
						best_action_utility = action_utility;
						best_action = act_t;
						LOG(LDEBUG) << "found action " << best_action << " with biggest utility " << best_action_utility << std::endl;
					}
				}//: for j
			}//: for i

		}//: for each maze
	}//: for each action type

	mic::types::NESWAction a((types::NESW_action_type_t) best_action);
	return a;
}


mic::types::Action2DInterface MazeHistogramFilter::sumOfMostUniquePatchesActionSelection() {
	mic::types::VectorXf action_utilities(4);
	action_utilities.setZero();


	// Calculate probabilities of all actions.
	for (size_t act_t=0; act_t < 4; act_t++) {
		mic::types::NESWAction ac((types::NESW_action_type_t)act_t);

		// Check the score of a given action.
		for (size_t m=0; m<number_of_mazes; m++) {

			for (size_t y=0; y<maze_height; y++) {
				for (size_t x=0; x<maze_width; x++) {
					// Check result of the next motion.
					// Compute resulting coordinates.
					size_t new_y = (y + maze_height + ac.dy()) % maze_height;
					size_t new_x = (x + maze_width + ac.dx()) % maze_width;
					// Get image patch.
					short patch = (*mazes[m])(new_y, new_x);
					LOG(LDEBUG) << "maze [" << m << "] (y=" << y <<",x="<< x <<") move="<< act_t << "=> (y+dy=" << new_y << ",x+dx=" << new_x <<") patch=" << patch << std::endl;
					// Get patch probability.
					double patch_prob = maze_patch_probabilities[patch];
					// Check the action result.
					double tmp_action_utility = (*maze_position_probabilities[m])(new_y, new_x) * (1- patch_prob);
					LOG(LDEBUG) << "patch_prob= " << patch_prob << " action_utility=" << tmp_action_utility << std::endl;

					// Add action utility.
					action_utilities(act_t) += tmp_action_utility;
				}//: for j
			}//: for i

		}//: for each maze
	}//: for each action type

	// Select best action
	size_t best_action = -1;
	double best_action_utility = 0.0;
	for (size_t act_t=0; act_t < 4; act_t++) {
		if (action_utilities(act_t) > best_action_utility) {
			best_action_utility = action_utilities(act_t);
			best_action = act_t;
			LOG(LDEBUG) << "found action " << best_action << " with biggest utility " << best_action_utility << std::endl;
		}

	}//: for each action type


	mic::types::NESWAction a((types::NESW_action_type_t) best_action);
	return a;
}





} /* namespace algorithms */
} /* namespace mic */
