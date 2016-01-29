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

private:

	/// Property: SDR size.
	mic::configuration::Property<size_t> sdr_size;

	/// Window for displaying chart with statistics.
	WindowChart* w_chart;

};

} /* namespace applications */
} /* namespace mic */

#endif /* SRC_APPLICATIONS_SIMPLEDIGITMAZEAPPLICATION_HPP_ */
