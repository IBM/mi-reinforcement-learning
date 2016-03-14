/*!
 * \file nBanditsApplication.hpp
 * \brief 
 * \author tkornut
 * \date Mar 14, 2016
 */

#ifndef SRC_APPLICATION_NBANDITSAPPLICATION_HPP_
#define SRC_APPLICATION_NBANDITSAPPLICATION_HPP_

#include <types/MatrixTypes.hpp>

#include <opengl/application/OpenGLApplication.hpp>
#include <opengl/visualization/WindowFloatCollectorChart.hpp>
using namespace mic::opengl::visualization;

namespace mic {
namespace application {

/*!
 * \brief Class implementing a n-Armed Bandits problem.
 * \author tkornuta
 */
class nBanditsApplication: public mic::opengl::application::OpenGLApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	nBanditsApplication(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~nBanditsApplication();

protected:
	/*!
	 * Initializes all variables that are property-dependent.
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

	/// Window for displaying average reward.
	WindowFloatCollectorChart* w_reward;

	/// Reward collector.
	mic::data_io::DataCollectorPtr<std::string, float> reward_collector_ptr;

	/// Property: number of bandits
	mic::configuration::Property<short> number_of_bandits;

	/// Property: variable denoting epsilon in aciton selection (the probability "below" which a random action will be selected).
	mic::configuration::Property<double> epsilon;

	/// Property: name of the file to which the statistics will be exported.
	mic::configuration::Property<std::string> statistics_filename;
};


} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_NBANDITSAPPLICATION_HPP_ */
