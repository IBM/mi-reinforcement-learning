/*!
 * \file TestApplication.cpp
 * \brief File contains definition of methods of simple exemplary application.
 * \author tkornut
 * \date Jan 27, 2016
 */

#include <applications/SimpleDigitMazeApplication.hpp>

#include <application/ApplicationFactory.hpp>


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
		hidden_y("hidden_y", 0)
	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(hidden_maze_number);
	registerProperty(hidden_x);
	registerProperty(hidden_y);

	LOG(LINFO) << "Properties registered";

	// Turn single step on.
	APP_STATE->pressSingleStep();

}


SimpleDigitMazeApplication::~SimpleDigitMazeApplication() {
	LOG(LINFO) << "Empty for now";
}


void SimpleDigitMazeApplication::initialize(int argc, char* argv[]) {
	LOG(LSTATUS) << "In here you should initialize Glut and create all OpenGL windows";

	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	// Create the visualization windows - must be created in the same, main thread :]
	w_chart = new WindowChart("Statistics", 256, 512, 0, 326);

	// Create data containers.
	w_chart->createDataContainer("Pa", mic::types::color_rgba(255, 0, 0, 180));
	w_chart->createDataContainer("Pb", mic::types::color_rgba(0, 255, 0, 180));
	w_chart->createDataContainer("Pc", mic::types::color_rgba(0, 0, 255, 180));

}

void SimpleDigitMazeApplication::initializePropertyDependentVariables() {

	// Import mazes.
	if (!importer.importData())
		return;

	// Set problem dimensions.
	number_of_mazes = importer.getData().size();
	problem_dimensions = number_of_mazes * importer.maze_width * importer.maze_height;
	number_of_distinctive_patches = 10;

	// Set pointer to data.
	mazes = importer.getData();

	// Assign initial probabilities for all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		maze_probabilities.push_back((double) 1.0/ mazes.size());

		std::shared_ptr < Matrix<double> > position_probabilities(new Matrix <double> (importer.maze_height, importer.maze_width));
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				(*position_probabilities)(i,j) = (double) 1.0/(problem_dimensions);
			}//: for j
		}//: for i

		maze_position_probabilities.push_back(position_probabilities);

		// Display results.
		LOG(LINFO) << (*mazes[m]);
		//LOG(LINFO) << (*maze_position_probabilities[m]);
	}//: for m


	// Collect statistics for all mazes - number of appearances of given "patch" (i.e. digit).
	for (size_t m=0; m<number_of_mazes; m++) {
		std::shared_ptr < Matrix<int> > maze = mazes[m];
		std::shared_ptr < mic::types::Vector<double> > patch_probabilities(new mic::types::Vector <double> (number_of_distinctive_patches));

		// Iterate through maze and collect occurences.
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				short patch_id = (*maze)(i,j);
				(*patch_probabilities)(patch_id) += 1.0;
			}//: for j
		}//: for i

		// Divide by number of maze elements -> probabilities.
		for (size_t i=0; i<number_of_distinctive_patches; i++) {
			(*patch_probabilities)(i) /= number_of_mazes;
		}//: for

		//LOG(LINFO) << (*patch_probabilities);

		maze_patch_probabilities.push_back(patch_probabilities);
	}//: for m(azes)


	LOG(LWARNING) << "Hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";
	// Get current observation.
	short obs =(*mazes[hidden_maze_number])(hidden_y, hidden_x);
	sense(obs);

	// Enter critical section - with the use of scoped lock from AppState!
	APP_DATA_SYNCHRONIZATION_SCOPED_LOCK();

	// Add data to chart window.
	w_chart->addDataToContainer("Pa", maze_probabilities[0]);
	w_chart->addDataToContainer("Pb", maze_probabilities[1]);
	w_chart->addDataToContainer("Pc", maze_probabilities[2]);

}


void SimpleDigitMazeApplication::sense (short obs_) {
	LOG(LINFO) << "Current observation=" << obs_;

	double hit_factor = 0.6;
	double miss_factor = 0.2;

	// Compute posterior distribution given Z (observation) - total probability.

	// For all mazes.
	double prob_sum = 0;
	for (size_t m=0; m<number_of_mazes; m++) {
		std::shared_ptr < Matrix<double> > pos_probs = maze_position_probabilities[m];
		std::shared_ptr < Matrix<int> > maze = mazes[m];

		// Iterate through position probabilities and update them.
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				if ((*maze)(i,j) == obs_)
					(*pos_probs)(i,j) *= hit_factor;
				else
					(*pos_probs)(i,j) *= miss_factor;
				prob_sum += (*pos_probs)(i,j);
			}//: for j
		}//: for i
	}//: for m

	prob_sum = 1/prob_sum;
	// Normalize probabilities for all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		std::shared_ptr < Matrix<double> > pos_probs = maze_position_probabilities[m];
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				(*pos_probs)(i,j) *= prob_sum;
			}//: for j
		}//: for i

		// Display results.
		LOG(LDEBUG) << (*mazes[m]);
		LOG(LINFO) << (*maze_position_probabilities[m]);
	}//: for m


}

void SimpleDigitMazeApplication::move (size_t dy_, size_t dx_) {
	LOG(LERROR) << "Current move= (" << dy_ << "," <<dx_ << ")";

	// For all mazes.
	for (size_t m=0; m<number_of_mazes; m++) {
		std::shared_ptr < Matrix<double> > pos_probs = maze_position_probabilities[m];
		Matrix<double> old_pose_probs = (*pos_probs);

		// Iterate through position probabilities and update them.
		for (size_t i=0; i<importer.maze_height; i++) {
			for (size_t j=0; j<importer.maze_width; j++) {
				//std::cout << "i=" << i << " j=" << j << " dx=" << dx_ << " dy=" << dy_ << " (i - dx_) % 3 = " << (i +3 - dx_) % 3 << " (j - dy_) % 3=" << (j + 3 - dy_) % 3 << std::endl;
				(*pos_probs)((j + importer.maze_height + dy_) %importer.maze_height, (i +importer.maze_width + dx_) % importer.maze_width) = old_pose_probs(j, i);

			}//: for j
		}//: for i

		// Display results.
		LOG(LDEBUG) << (*mazes[m]);
		LOG(LWARNING) << (*maze_position_probabilities[m]);
	}//: for m

	// Perform the REAL move.
	hidden_y = (hidden_y + importer.maze_height + dy_) % importer.maze_height;
	hidden_x = (hidden_x + importer.maze_width + dx_) % importer.maze_width;

	LOG(LWARNING) << "Hidden position in maze " << hidden_maze_number << "= (" << hidden_y << "," << hidden_x << ")";

}

bool SimpleDigitMazeApplication::performSingleStep() {
	LOG(LWARNING) << "Perform single step ";

	// Perform move.
	move(1,0);

	// Get current observation.
	short obs =(*mazes[hidden_maze_number])(hidden_y, hidden_x);
	sense(obs);


	// Add data to chart window.
	w_chart->addDataToContainer("Pa", maze_probabilities[0]);
	w_chart->addDataToContainer("Pb", maze_probabilities[1]);
	w_chart->addDataToContainer("Pc", maze_probabilities[2]);

	return true;
}






} /* namespace applications */
} /* namespace mic */
