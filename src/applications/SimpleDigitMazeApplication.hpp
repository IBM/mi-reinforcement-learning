/*!
 * \file TestApplication.hpp
 * \brief Declaration of a class being a simple example of application.
 * \author tkornut
 * \date Jan 27, 2016
 */

#ifndef SRC_APPLICATIONS_SIMPLEDIGITMAZEAPPLICATION_HPP_
#define SRC_APPLICATIONS_SIMPLEDIGITMAZEAPPLICATION_HPP_

#include <opengl/application/OpenGLApplication.hpp>

#include <opengl/visualization/WindowChart.hpp>
using namespace mic::opengl::visualization;

#include <types/matrix.h>
#include <types/vector.h>

#include <data_io/MazeMatrixImporter.hpp>

#include <types/Action.hpp>

namespace mic {
namespace applications {

/*!
 * \brief Class implementing a simple test application (currently zero functionality).
 * \author tkornuta
 */
class SimpleDigitMazeApplication: public mic::opengl::application::OpenGLApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	SimpleDigitMazeApplication(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~SimpleDigitMazeApplication();

protected:
	/*!
	 * Initializes all variables that are property-dependent (input patches, SDRs etc.).
	 */
	virtual void initializePropertyDependentVariables();

	/*!
	 * Method initializes GLUT and OpenGL windows.
	 * @param argc Number of application parameters.
	 * @param argv Array of application parameters.
	 */
	virtual void initialize(int argc, char* argv[]);

	/*!
	 * Performs single step of computations.
	 */
	virtual bool performSingleStep();

	/*!
	 * Perform "probabilistic" sensing - update probabilities basing on the current observation.
	 * @param obs_ Current observation.
	 */
	void sense (short obs_);

	/*!
	 * Perform "probabilistic" move.
	 * @param dy_ Step along y
	 * @param dx_ Step along x
	 */
	void move (mic::types::Action2DInterface ac_);

	/*!
	 * Updates aggregated probabilities of current maze number, x and y coordinates as well as add them to data containsers.
	 * @param synchronize_ If true enters critical section when adding data to containers.
	 */
	void updateAggregatedProbabilities(bool synchronize_);


private:

	/// Window for displaying chart with statistics on current maze number.
	WindowChart* w_current_maze_chart;

	/// Window for displaying chart with statistics on current x coordinate.
	WindowChart* w_current_coordinate_x;

	/// Window for displaying chart with statistics on current y coordinate.
	WindowChart* w_current_coordinate_y;

	/// Importer responsible for loading mazes from file.
	mic::data_io::MazeMatrixImporter importer;

	/// Vector of mazes - pointer to vector of mazes returned by importer.
	std::vector<std::shared_ptr< Matrix<int> > > mazes;

	/// Variable storing the probability that we are in a given maze position.
	std::vector<std::shared_ptr< Matrix<double> > > maze_position_probabilities;


	/// Variable storing the probability that we are currently moving in/observing a given maze.
	std::vector<double> maze_probabilities;

	/// Variable storing the probability that we are currently in a given x coordinate.
	std::vector<double> maze_x_coordinate_probilities;

	/// Variable storing the probability that we are currently in a given y coordinate.
	std::vector<double> maze_y_coordinate_probilities;

	/// Variable storing the probability that we can find given patch in a given maze.
	std::vector<std::shared_ptr< Vector<double> > > maze_patch_probabilities;

	/// Property: variable denoting in which maze are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_maze_number;

	/// Property: variable denoting the x position are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_x;

	/// Property: variable denoting the y position are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_y;

	/// Problem dimensions - number of mazes.
	int number_of_mazes;

	/// Problem dimensions - number of distinctive patches (in here - number of different digits, i.e. 10).
	int number_of_distinctive_patches;

	/// Problem dimensions - number of mazes * their width * their height.
	int problem_dimensions;

	/// Property: variable denoting the hit factor (the gain when the observation coincides with current position).
	mic::configuration::Property<double> hit_factor;

	/// Property: variable denoting the miss factor (the gain when the observation does not coincide with current position).
	mic::configuration::Property<double> miss_factor;

	/// Property: performed action (0-3: NESW, -1: random).
	mic::configuration::Property<short> action;

};

} /* namespace applications */
} /* namespace mic */

#endif /* SRC_APPLICATIONS_SIMPLEDIGITMAZEAPPLICATION_HPP_ */
