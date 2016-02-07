/*!
 * \file TestApplication.cpp
 * \brief File contains definition of methods of simple exemplary application.
 * \author tkornut
 * \date Jan 27, 2016
 */

#include <applications/SimpleDigitMazeApplication.hpp>

#include <application/ApplicationFactory.hpp>

#include <random>
#include  <data_utils/RandomGenerator.hpp>


namespace mic {
namespace application {

/*!
 * \brief Registers application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::applications::SimpleDigitMazeApplication);
}

} /* namespace application */

namespace applications {

SimpleDigitMazeApplication::SimpleDigitMazeApplication(std::string node_name_) : OpenGLApplication(node_name_),
		hidden_maze_number("hidden_maze", 0),
		hidden_x("hidden_x", 0),
		hidden_y("hidden_y", 0),
		hit_factor("hit_factor", 0.6),
		miss_factor("miss_factor", 0.2),
		action("action", -1)
	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(hidden_maze_number);
	registerProperty(hidden_x);
	registerProperty(hidden_y);
	registerProperty(hit_factor);
	registerProperty(miss_factor);
	registerProperty(action);

	LOG(LINFO) << "Properties registered";

	// Turn single step on.
	APP_STATE->pressSingleStep();

}


SimpleDigitMazeApplication::~SimpleDigitMazeApplication() {

}


void SimpleDigitMazeApplication::initialize(int argc, char* argv[]) {
	LOG(LSTATUS) << "In here you should initialize Glut and create all OpenGL windows";

	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	// Create the visualization windows - must be created in the same, main thread :]
	w_current_maze_chart = new WindowChart("Current_maze", 256, 256, 0, 0);
	w_current_coordinate_x = new WindowChart("Current_x", 256, 256, 0, 326);
	w_current_coordinate_y = new WindowChart("Current_y", 256, 256, 326, 326);


}

void SimpleDigitMazeApplication::initializePropertyDependentVariables() {

	// Import mazes.
	if (!importer.importData())
		return;

	// Set problem dimensions.
	number_of_mazes = importer.getData().size();
	problem_dimensions = number_of_mazes * importer.maze_width * importer.maze_height;
	number_of_distinctive_patches = 10;

	// Get "hidden" maze number.
	if ((int)hidden_maze_number == -1) {
		hidden_maze_number = RAN_GEN->uniRandInt(0,number_of_mazes-1);
	} else
		hidden_maze_number = hidden_maze_number % number_of_mazes;

	// Get "hidden" maze x coordinate.
	if ((int)hidden_x == -1) {
		hidden_x = RAN_GEN->uniRandInt(0,importer.maze_width-1);
	} else
		hidden_x = hidden_x % importer.maze_width;

	// Get "hidden" maze y coordinate.
if ((int)hidden_y == -1) {
	hidden_y = RAN_GEN->uniRandInt(0,importer.maze_height-1);
	} else
		hidden_y = hidden_y % importer.maze_height;
	LOG(LWARNING) << "After truncation/random: hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";


	// Set pointer to data.
	mazes = importer.getData();

	// Show mazes.
	LOG(LNOTICE) << "Loaded mazes";
	for (size_t m=0; m<number_of_mazes; m++) {
		// Display results.
		LOG(LNOTICE) << "maze(" <<m<<"):\n" << (*mazes[m]);
	}//: for

	// Assign initial probabilities to all variables (uniform distribution).s
	assignInitialProbabilities();

	// Create data containers - for visualization.
	createDataContainers();

	// Store the "zero" state.
	storeCurrentStateInDataContainers(true);

	LOG(LWARNING) << "Hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";
	// Get first observation.
	short obs =(*mazes[hidden_maze_number])(hidden_y, hidden_x);
	sense(obs);

	// Update aggregated probabilities.
	updateAggregatedProbabilities();

	// Store the first state.
	storeCurrentStateInDataContainers(true);

}


void SimpleDigitMazeApplication::assignInitialProbabilities() {

	// Assign initial probabilities for all mazes/positions.
	LOG(LNOTICE) << "Initial maze_position_probabilities:";
	for (size_t m=0; m<number_of_mazes; m++) {

		mic::types::matrixd_ptr_t position_probabilities(new mic::types::matrixd_t (importer.maze_height, importer.maze_width));
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				(*position_probabilities)(i,j) = (double) 1.0/(problem_dimensions);
			}//: for j
		}//: for i

		maze_position_probabilities.push_back(position_probabilities);

		LOG(LNOTICE) << "maze_position_prob(" <<m<<"):\n" << (*maze_position_probabilities[m]);
	}//: for m

	// Assign initial probabilities to maze - for visualization.
	for (size_t m=0; m<number_of_mazes; m++) {
		maze_probabilities.push_back((double) 1.0/ mazes.size());
	}//: for


	//  Assign initial probabilities to x coordinate - for visualization.
	for (size_t x=0; x<importer.maze_width; x++) {
		maze_x_coordinate_probilities.push_back((double) 1.0/ importer.maze_width);
	}//: for

	//  Assign initial probabilities to y coordinate - for visualization.
	for (size_t y=0; y<importer.maze_height; y++) {
		maze_y_coordinate_probilities.push_back((double) 1.0/ importer.maze_height);
	}//: for

	// Collect statistics for all mazes - number of appearances of a given "patch" (i.e. digit).
	maze_patch_probabilities.resize(number_of_distinctive_patches);
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::matrixi_ptr_t maze = mazes[m];

		// Iterate through maze and collect occurrences.
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
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

void SimpleDigitMazeApplication::createDataContainers() {
	// Random device used for generation of colors.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());
	// Initialize uniform index distribution - integers.
	std::uniform_int_distribution<> color_dist(50, 200);
	// Create a single container for each maze.
	for (size_t m=0; m<number_of_mazes; m++) {
		std::string label = "P(m" + std::to_string(m) +")";
		int r= color_dist(rng_mt19937_64);
		int g= color_dist(rng_mt19937_64);
		int b= color_dist(rng_mt19937_64);
		//std::cout << label << " g=" << r<< " g=" << r<< " b=" << b;

		// Add container to chart.
		w_current_maze_chart->createDataContainer(label, mic::types::color_rgba(r, g, b, 180));

	}//: for


	// Create a single container for each x coordinate.
	for (size_t x=0; x<importer.maze_width; x++) {
		std::string label = "P(x" + std::to_string(x) +")";
		int r= color_dist(rng_mt19937_64);
		int g= color_dist(rng_mt19937_64);
		int b= color_dist(rng_mt19937_64);

		// Add container to chart.
		w_current_coordinate_x->createDataContainer(label, mic::types::color_rgba(r, g, b, 180));

	}//: for


	// Create a single container for each y coordinate.
	for (size_t y=0; y<importer.maze_height; y++) {
		std::string label = "P(y" + std::to_string(y) +")";
		int r= color_dist(rng_mt19937_64);
		int g= color_dist(rng_mt19937_64);
		int b= color_dist(rng_mt19937_64);

		// Add container to chart.
		w_current_coordinate_y->createDataContainer(label, mic::types::color_rgba(r, g, b, 180));
	}//: for

}


void SimpleDigitMazeApplication::sense (short obs_) {
	LOG(LINFO) << "Current observation=" << obs_;

	// Compute posterior distribution given Z (observation) - total probability.

	// For all mazes.
	double prob_sum = 0;
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::matrixd_ptr_t pos_probs = maze_position_probabilities[m];
		mic::types::matrixi_ptr_t maze = mazes[m];

		// Display results.
/*		LOG(LERROR) << "Przed updatem";
		LOG(LERROR) << (*mazes[m]);
		LOG(LERROR) << (*maze_position_probabilities[m]);
*/

		// Iterate through position probabilities and update them.
		for (size_t y=0; y<importer.maze_height; y++) {
			for (size_t x=0; x<importer.maze_width; x++) {
				if ((*maze)(y,x) == obs_)
					(*pos_probs)(y,x) *= hit_factor;
				else
					(*pos_probs)(y,x) *= miss_factor;
				prob_sum += (*pos_probs)(y,x);
			}//: for j
		}//: for i
	}//: for m

	prob_sum = 1/prob_sum;
	// Normalize probabilities for all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::matrixd_ptr_t pos_probs = maze_position_probabilities[m];
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				(*pos_probs)(i,j) *= prob_sum;
			}//: for j
		}//: for i

		// Display results.
		LOG(LNOTICE) << "maze(" <<m<<"):\n" << (*mazes[m]);
		LOG(LNOTICE) << "maze_pose_prob(" <<m<<"):\n" << (*maze_position_probabilities[m]);
	}//: for m

}

void SimpleDigitMazeApplication::storeCurrentStateInDataContainers(bool synchronize_) {

	if (synchronize_)
	{ // Enter critical section - with the use of scoped lock from AppState!
		APP_DATA_SYNCHRONIZATION_SCOPED_LOCK();

		// Add data to chart windows.
		for (size_t m=0; m<number_of_mazes; m++) {
			std::string label = "P(m" + std::to_string(m) +")";
			w_current_maze_chart->addDataToContainer(label, maze_probabilities[m]);
		}//: for

		for (size_t x=0; x<importer.maze_width; x++) {
			std::string label = "P(x" + std::to_string(x) +")";
			w_current_coordinate_x->addDataToContainer(label, maze_x_coordinate_probilities[x]);
		}//: for

		for (size_t y=0; y<importer.maze_height; y++) {
			std::string label = "P(y" + std::to_string(y) +")";
			w_current_coordinate_y->addDataToContainer(label, maze_y_coordinate_probilities[y]);
		}//: for

	}//: end of critical section.
	else {
		// Add data to chart windows.
		for (size_t m=0; m<number_of_mazes; m++) {
			std::string label = "P(m" + std::to_string(m) +")";
			w_current_maze_chart->addDataToContainer(label, maze_probabilities[m]);
		}//: for

		for (size_t x=0; x<importer.maze_width; x++) {
			std::string label = "P(x" + std::to_string(x) +")";
			w_current_coordinate_x->addDataToContainer(label, maze_x_coordinate_probilities[x]);
		}//: for

		for (size_t y=0; y<importer.maze_height; y++) {
			std::string label = "P(y" + std::to_string(y) +")";
			w_current_coordinate_y->addDataToContainer(label, maze_y_coordinate_probilities[y]);
		}//: for

	}//: else
}


void SimpleDigitMazeApplication::updateAggregatedProbabilities() {
	// Update maze_probabilities.
	for (size_t m=0; m<number_of_mazes; m++) {
		// Reset probability.
		maze_probabilities[m] = 0;
		mic::types::matrixd_ptr_t pos_probs = maze_position_probabilities[m];
		// Sum probabilities of all positions.
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				maze_probabilities[m] += (*pos_probs)(i,j);
			}//: for j
		}//: for i
	}//: for m

	// Update maze_x_coordinate_probilities.
	for (size_t x=0; x<importer.maze_width; x++) {
		// Reset probability.
		maze_x_coordinate_probilities[x] = 0;
		for (size_t m=0; m<number_of_mazes; m++) {
			mic::types::matrixd_ptr_t pos_probs = maze_position_probabilities[m];
			// Sum probabilities of all positions.
			for (size_t y=0; y<importer.maze_height; y++) {
				maze_x_coordinate_probilities[x] += (*pos_probs)(y,x);
				}//: for y
		}//: for m
	}//: for x


	// Update maze_y_coordinate_probilities.
	for (size_t y=0; y<importer.maze_height; y++) {
		// Reset probability.
		maze_y_coordinate_probilities[y] = 0;
		for (size_t m=0; m<number_of_mazes; m++) {
			mic::types::matrixd_ptr_t pos_probs = maze_position_probabilities[m];
			// Sum probabilities of all positions.
			for (size_t x=0; x<importer.maze_width; x++) {
				maze_y_coordinate_probilities[y] += (*pos_probs)(y,x);
				}//: for x
		}//: for m
	}//: for y

}

void SimpleDigitMazeApplication::move (mic::types::Action2DInterface ac_) {
	LOG(LINFO) << "Current move dy,dx= ( " << ac_.dy() << "," <<ac_.dx()<< ")";

	// For all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		mic::types::matrixd_ptr_t pos_probs = maze_position_probabilities[m];
		mic::types::matrixd_t old_pose_probs = (*pos_probs);

/*		LOG(LERROR) << "Przed ruchem";
		LOG(LERROR) << (*mazes[m]);
		LOG(LERROR) << (*maze_position_probabilities[m]);*/

		// Iterate through position probabilities and update them.
		for (size_t y=0; y<importer.maze_height; y++) {
			for (size_t x=0; x<importer.maze_width; x++) {
				//std::cout << "i=" << i << " j=" << j << " dx=" << dx_ << " dy=" << dy_ << " (i - dx_) % 3 = " << (i +3 - dx_) % 3 << " (j - dy_) % 3=" << (j + 3 - dy_) % 3 << std::endl;
				(*pos_probs)((y + importer.maze_height +  ac_.dy()) %importer.maze_height, (x +importer.maze_width +  ac_.dx()) % importer.maze_width) = old_pose_probs(y, x);

			}//: for j
		}//: for i

		// Display results.
		LOG(LNOTICE) << "maze(" <<m<<"):\n" << (*mazes[m]);
		LOG(LNOTICE) << "maze_pose_prob(" <<m<<"):\n" << (*maze_position_probabilities[m]);
	}//: for m

	// Perform the REAL move.
	hidden_y = (hidden_y + importer.maze_height +  ac_.dy()) % importer.maze_height;
	hidden_x = (hidden_x + importer.maze_width +  ac_.dx()) % importer.maze_width;

	LOG(LINFO) << "Hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";

}

mic::types::Action2DInterface SimpleDigitMazeApplication::mostInfrequentPatchActionSelection() {
	double most_probable_rarest_patch_prob = 0.0;
	size_t most_probable_rarest_patch_action = -1;

	// Calculate probabilities of all actions.
	for (size_t act_t=0; act_t < 4; act_t++) {
		mic::types::NESWAction ac((NESW_action_type_t)act_t);

		// Check the score of a given action.
		for (size_t m=0; m<number_of_mazes; m++) {

			for (size_t y=0; y<importer.maze_height; y++) {
				for (size_t x=0; x<importer.maze_width; x++) {
					// Check result of the next motion.
					// Compute resulting coordinates.
					size_t new_y = (y + importer.maze_height + ac.dy()) % importer.maze_height;
					size_t new_x = (x + importer.maze_width + ac.dx()) % importer.maze_width;
					// Get image patch.
					short patch = (*mazes[m])(new_y, new_x);
					std::cout << "maze [" << m << "] (y=" << y <<",x="<< x <<") move="<< act_t << "=> (y+dy=" << new_y << ",x+dx=" << new_x <<" patch=" << patch << std::endl;
					// Get patch probability.
					double patch_prob = maze_patch_probabilities[patch];
					// Check the action result.
					double maze_patch_prob = maze_probabilities[m] * maze_x_coordinate_probilities[x] * maze_y_coordinate_probilities[y] * (1- patch_prob);
					std::cout << "patch_prob= " << patch_prob << " maze_patch_prob=" << maze_patch_prob << std::endl;
					if (maze_patch_prob > most_probable_rarest_patch_prob) {
						most_probable_rarest_patch_prob = maze_patch_prob;
						most_probable_rarest_patch_action = act_t;
						std::cout << "found most probable action: " << act_t << std::endl;
					}
				}//: for j
			}//: for i

		}//: for each maze
	}//: for each action type

	mic::types::NESWAction a((NESW_action_type_t) most_probable_rarest_patch_action);
	return a;
}

bool SimpleDigitMazeApplication::performSingleStep() {
	LOG(LINFO) << "Performing a single step ";

	// Perform move.
	if (action == (short)-2)
		move(A_RANDOM);
	else if (action == (short)-1)
		move(mostInfrequentPatchActionSelection());
	else
		move(mic::types::NESWAction((mic::types::NESW_action_type_t) (short)action));



	// Get current observation.
	short obs =(*mazes[hidden_maze_number])(hidden_y, hidden_x);
	sense(obs);

	// Update state.
	updateAggregatedProbabilities();

	// Store collected data for visualization/export.
	storeCurrentStateInDataContainers(false);

	return true;
}






} /* namespace applications */
} /* namespace mic */
