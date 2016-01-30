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
		hidden_maze("hidden_maze", 1),
		hidden_x("hidden_x", 1),
		hidden_y("hidden_y", 1)
	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(hidden_maze);
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
	LOG(LSTATUS) << "In here you should initialize all variables that depend on properties";

	LOG(LINFO) << "You an for example allocate memory for structures)";

	// Load datasets.
	LOG(LINFO) << "Or import data with the use of importer-derived class.";

	std::shared_ptr < Matrix<short> > a(new Matrix <short> (3,3));
	(*a)(0, 0) = 1;
	(*a)(0, 1) = 2;
	(*a)(0, 2) = 3;
	(*a)(1, 0) = 3;
	(*a)(1, 1) = 1;
	(*a)(1, 2) = 9;
	(*a)(2, 0) = 8;
	(*a)(2, 1) = 7;
	(*a)(2, 2) = 6;
	mazes.push_back(a);

	std::shared_ptr < Matrix<short> > b(new Matrix <short> (3,3));
	(*b)(0, 0) = 1;
	(*b)(0, 1) = 4;
	(*b)(0, 2) = 7;
	(*b)(1, 0) = 8;
	(*b)(1, 1) = 7;
	(*b)(1, 2) = 2;
	(*b)(2, 0) = 3;
	(*b)(2, 1) = 9;
	(*b)(2, 2) = 4;
	mazes.push_back(b);

	std::shared_ptr < Matrix<short> > c(new Matrix <short> (3,3));
	(*c)(0, 0) = 1;
	(*c)(0, 1) = 3;
	(*c)(0, 2) = 2;
	(*c)(1, 0) = 8;
	(*c)(1, 1) = 7;
	(*c)(1, 2) = 2;
	(*c)(2, 0) = 2;
	(*c)(2, 1) = 9;
	(*c)(2, 2) = 3;
	mazes.push_back(c);

	// Assign initial probabilities.

	// For all mazes.
	for (size_t m=0; m<3; m++) {
		maze_probabilities.push_back((double) 1.0/3.0);

		std::shared_ptr < Matrix<double> > position_probabilities(new Matrix <double> (3,3));
		for (size_t i=0; i<3; i++) {
			for (size_t j=0; j<3; j++) {
				(*position_probabilities)(i,j) = (double) 1.0/9.0;
			}//: for j
		}//: for i

		maze_position_probabilities.push_back(position_probabilities);

		// Display results.
		LOG(LINFO) << (*mazes[m]);
		//LOG(LINFO) << (*maze_position_probabilities[m]);
	}//: for m


	// For all mazes.
	for (size_t m=0; m<3; m++) {
		std::shared_ptr < Matrix<short> > maze = mazes[m];
		// Collect statistics - number of appearances of given "patch" (i.e. digit).
		std::shared_ptr < mic::types::Vector<double> > patch_probabilities(new mic::types::Vector <double> (10));

		// Iterate through maze and collect occurences.
		for (size_t i=0; i<3; i++) {
			for (size_t j=0; j<3; j++) {
				short patch_id = (*maze)(i,j);
				(*patch_probabilities)(patch_id) += 1.0;
			}//: for j
		}//: for i

		// Divide by number of maze elements -> probabilities.
		for (size_t i=0; i<10; i++) {
			(*patch_probabilities)(i) /= 9.0;
		}//: for

		//LOG(LINFO) << (*patch_probabilities);

		maze_patch_probabilities.push_back(patch_probabilities);
	}//: for m(azes)

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
	size_t m =1 ; {//for (size_t m=0; m<3; m++) {
		std::shared_ptr < Matrix<double> > pos_probs = maze_position_probabilities[m];
		std::shared_ptr < Matrix<short> > maze = mazes[m];

		double prob_sum = 0;
		// Iterate through position probabilities and update them.
		for (size_t i=0; i<3; i++) {
			for (size_t j=0; j<3; j++) {
				if ((*maze)(i,j) == obs_)
					(*pos_probs)(i,j) *= hit_factor;
				else
					(*pos_probs)(i,j) *= miss_factor;
				prob_sum += (*pos_probs)(i,j);
			}//: for j
		}//: for i
		prob_sum = 1/prob_sum;
		// Normalize probabilities.
		for (size_t i=0; i<3; i++) {
			for (size_t j=0; j<3; j++) {
				(*pos_probs)(i,j) *= prob_sum;
			}//: for j
		}//: for i

		// Display results.
		//LOG(LINFO) << (*mazes[m]);
		LOG(LINFO) << (*maze_position_probabilities[m]);
	}//: for m


}

void SimpleDigitMazeApplication::move (size_t dy_, size_t dx_) {
	LOG(LWARNING) << "Current move= (" << dy_ << "," <<dx_ << ")";

/*	Matrix<double> tmp(4,2);
	tmp(3,0) = 2;
	std::cout << tmp;*/

	// For all mazes.
	size_t m =1 ; {//for (size_t m=0; m<3; m++) {
		std::shared_ptr < Matrix<double> > pos_probs = maze_position_probabilities[m];
		Matrix<double> old_pose_probs = (*pos_probs);
		(*pos_probs)(0,0) = 0;
		(*pos_probs)(0,0) = 1;
		(*pos_probs)(2,2) = 2;


		// Iterate through position probabilities and update them.
		for (size_t i=0; i<3; i++) {
			for (size_t j=0; j<3; j++) {
				//std::cout << "i=" << i << " j=" << j << " dx=" << dx_ << " dy=" << dy_ << " (i - dx_) % 3 = " << (i +3 - dx_) % 3 << " (j - dy_) % 3=" << (j + 3 - dy_) % 3 << std::endl;
				(*pos_probs)((j + 3 + dy_) %3, (i +3 + dx_) % 3) = old_pose_probs(j, i);

			}//: for j
		}//: for i

		// Display results.
		//LOG(LINFO) << (*mazes[m]);
		LOG(LWARNING) << (*maze_position_probabilities[m]);
	}//: for m

	// Perform the REAL move.
	hidden_x = (hidden_x + 3 + dx_) %3;
	hidden_y = (hidden_y + 3 + dy_) %3;

	LOG(LWARNING) << "Hidden position in maze " << hidden_maze << "= (" << hidden_y << "," << hidden_x << ")";

}

bool SimpleDigitMazeApplication::performSingleStep() {
	LOG(LWARNING) << "Perform single step ";

	// Perform move.
	move(1,0);

	// Get current observation.
	short obs =(*mazes[hidden_maze])(hidden_y, hidden_x);
	sense(obs);


	// Add data to chart window.
	w_chart->addDataToContainer("Pa", maze_probabilities[0]);
	w_chart->addDataToContainer("Pb", maze_probabilities[1]);
	w_chart->addDataToContainer("Pc", maze_probabilities[2]);

	return true;
}






} /* namespace applications */
} /* namespace mic */
