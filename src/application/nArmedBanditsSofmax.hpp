/*!
 * \file nArmedBanditsSofmax.hpp
 * \brief 
 * \author tkornut
 * \date Mar 14, 2016
 */

#ifndef SRC_APPLICATION_NARMEDBANDITSSOFMAX_HPP_
#define SRC_APPLICATION_NARMEDBANDITSSOFMAX_HPP_

#include <vector>

#include <types/MatrixTypes.hpp>

#include <opengl/application/OpenGLApplication.hpp>
#include <opengl/visualization/WindowFloatCollectorChart.hpp>
using namespace mic::opengl::visualization;

namespace mic {
namespace application {

/*!
 * \brief Class implementing a n-Armed Bandits problem solving the n armed bandits problem using Softmax Action Selection.
 * \author tkornuta
 */
class nArmedBanditsSofmax: public mic::opengl::application::OpenGLApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	nArmedBanditsSofmax(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~nArmedBanditsSofmax();

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

	/// n Bandit arms.
	mic::types::VectorXf arms;

	/// Action values.
	mic::types::VectorXf action_values;

	/// Counters storing how many times we've taken a particular action.
	mic::types::VectorXi action_counts;

	/// Action values - softmax.
	mic::types::VectorXf action_values_softmax;


	/// Property: number of bandits
	mic::configuration::Property<short> number_of_bandits;

	/// Property: the softmax "heat" parameter, scaling the probability distribution of all actions.
	/// A high temperature will tend the probabilities to be very similar, whereas a low temperature will exaggerate differences in probabilities between actions.
	mic::configuration::Property<double> tau;

	/// Property: name of the file to which the statistics will be exported.
	mic::configuration::Property<std::string> statistics_filename;

	/*!
	 * Calculates the reward.
	 * @param prob_  Probability.
	 */
	short calculateReward(float prob_);

	/*!
	 * Updates the softmax action-value table.
	 */
	void updateSoftmaxValues();

};

} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_NARMEDBANDITSSOFMAX_HPP_ */
