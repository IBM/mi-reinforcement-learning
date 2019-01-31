/*!
 * \file GridworldQLearning.hpp
 * \brief 
 * \author tkornut
 * \date Mar 21, 2016
 */

#ifndef SRC_APPLICATION_GRIDWORLDQLEARNING_HPP_
#define SRC_APPLICATION_GRIDWORLDQLEARNING_HPP_

#include <vector>
#include <string>

#include <opengl/application/OpenGLEpisodicApplication.hpp>
#include <opengl/visualization/WindowCollectorChart.hpp>
using namespace mic::opengl::visualization;

#include <types/Gridworld.hpp>

namespace mic {
namespace application {



/*!
 * \brief Class responsible for solving the gridworld problem with Q-learning.
 * \author tkornuta
 */
class GridworldQLearning: public mic::opengl::application::OpenGLEpisodicApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	GridworldQLearning(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~GridworldQLearning();

protected:

	/*!
	 * Method initializes GLUT and OpenGL windows.
	 * @param argc Number of application parameters.
	 * @param argv Array of application parameters.
	 */
	virtual void initialize(int argc, char* argv[]);

	/*!
	 * Initializes all variables that are property-dependent.
	 */
	virtual void initializePropertyDependentVariables();

	/*!
	 * Performs single step of computations.
	 */
	virtual bool performSingleStep();

	/*!
	 * Method called at the beginning of new episode (goal: to reset the statistics etc.) - abstract, to be overridden.
	 */
	virtual void startNewEpisode();

	/*!
	 * Method called when given episode ends (goal: export collected statistics to file etc.) - abstract, to be overridden.
	 */
	virtual void finishCurrentEpisode();


private:

	/// Window for displaying ???.
	WindowCollectorChart<float>* w_chart;

	/// Data collector.
	mic::data_io::DataCollectorPtr<std::string, float> collector_ptr;

	/// The gridworld object.
	mic::environments::Gridworld grid_env;

	/// Tensor storing values for all states (gridworld w * h * 4 (number of actions)). COL MAJOR(!).
	mic::types::TensorXf qstate_table;

	/*!
	 * Property: the "expected intermediate reward", i.e. reward received by performing each step (typically negative, but can be positive as all).
	 */
	mic::configuration::Property<float> step_reward;

	/*!
	 * Property: future discount (should be in range 0.0-1.0).
	 */
	mic::configuration::Property<float> discount_rate;

	/*!
	 * Property: learning rate (should be in range 0.0-1.0).
	 */
	mic::configuration::Property<float> learning_rate;

	/*!
	 * Property: move noise, determining gow often action results in unintended direction.
	 */
	mic::configuration::Property<float> move_noise;

	/*!
	 * Property: variable denoting epsilon in action selection (the probability "below" which a random action will be selected).
	 * if epsilon < 0 then if will be set to 1/episode, hence change dynamically depending on the episode number.
	 */
	mic::configuration::Property<double> epsilon;

	/// Property: name of the file to which the statistics will be exported.
	mic::configuration::Property<std::string> statistics_filename;

	/*!
	 * Steams the current state of the state-action values.
	 * @return Ostream with description of the state-action table.
	 */
	std::string streamQStateTable();


	/*!
	 * Calculates the best value for given state - by finding the action having the maximal expected value.
	 * @param pos_ Starting state (position).
	 * @return Value for given state.
	 */
	float computeBestValue(mic::types::Position2D pos_);


	/*!
	 * Finds the best action.
	 * @return The best action found.
	 */
	mic::types::NESWAction selectBestAction(mic::types::Position2D pos_);


	/*!
	 * Sum of all iterations made till now - used in statistics.
	 */
	long long sum_of_iterations;

	/*!
	 * Sum of all rewards collected till now - used in statistics.
	 */
	long long sum_of_rewards;

};

} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_GRIDWORLDQLEARNING_HPP_ */
