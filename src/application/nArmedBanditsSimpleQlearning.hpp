/*!
 * \file nArmedBanditsSimpleQlearning.hpp
 * \brief 
 * \author tkornut
 * \date Mar 14, 2016
 */

#ifndef SRC_APPLICATION_NARMEDBANDITSSIMPLEQLEARNING_HPP_
#define SRC_APPLICATION_NARMEDBANDITSSIMPLEQLEARNING_HPP_

#include <vector>

#include <types/MatrixTypes.hpp>

#include <opengl/application/OpenGLApplication.hpp>
#include <opengl/visualization/WindowFloatCollectorChart.hpp>
using namespace mic::opengl::visualization;

namespace mic {
namespace application {

/*!
 * \brief Class implementing a n-Armed Bandits problem solving the n armed bandits problem using simple Q-learning rule.
 * \author tkornuta
 */
class TestApp: public mic::opengl::application::OpenGLApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	TestApp(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~TestApp();

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

	/// Property: number of bandits
	mic::configuration::Property<size_t> number_of_bandits;

	/// Property: variable denoting epsilon in action selection (the probability "below" which a random action will be selected).
	mic::configuration::Property<double> epsilon;

	/// Property: name of the file to which the statistics will be exported.
	mic::configuration::Property<std::string> statistics_filename;

	/*!
	 * The best arm (hidden state).
	 */
	size_t best_arm;

	/*!
	 * The best arm probability/"reward" (hidden state).
	 */
	float best_arm_prob;

	/*!
	 * Calculates the reward.
	 * @param prob_  Probability.
	 */
	short calculateReward(float prob_);

	/*!
	 * Greedy method that selects best arm based on historical action-value pairs.
	 */
	size_t selectBestArm();

};

} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_NARMEDBANDITSSIMPLEQLEARNING_HPP_ */
