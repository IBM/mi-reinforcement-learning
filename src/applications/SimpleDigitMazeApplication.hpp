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
	void move (size_t dy_, size_t dx_);


private:

	/// Window for displaying chart with statistics.
	WindowChart* w_chart;

	/// List of mazes.
	std::vector<std::shared_ptr< Matrix<short> > > mazes;


	/// Variable storing the probability that we are currently moving in/observing a given maze.
	std::vector<double> maze_probabilities;

	/// Variable storing the probability that we are in a given maze position.
	std::vector<std::shared_ptr< Matrix<double> > > maze_position_probabilities;


	/// Variable storing the probability that we can find given patch in a given maze.
	std::vector<std::shared_ptr< Vector<double> > > maze_patch_probabilities;

	/// Property: variable denoting in which maze are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_maze;
	/// Property: variable denoting the x position are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_x;
	/// Property: variable denoting the y position are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_y;


};

} /* namespace applications */
} /* namespace mic */

#endif /* SRC_APPLICATIONS_SIMPLEDIGITMAZEAPPLICATION_HPP_ */
