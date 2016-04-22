/*!
 * \file GridworldDeepQLearning.hpp
 * \brief 
 * \author tkornut
 * \date Apr 21, 2016
 */

#ifndef SRC_APPLICATION_GRIDWORLDDEEPQLEARNING_HPP_
#define SRC_APPLICATION_GRIDWORLDDEEPQLEARNING_HPP_

#include <vector>
#include <string>

#include <opengl/application/OpenGLEpisodicApplication.hpp>
#include <opengl/visualization/WindowFloatCollectorChart.hpp>
using namespace mic::opengl::visualization;

#include <mlnn/MultiLayerNeuralNetwork.hpp>
// Using multi layer neural networks
using namespace mic::mlnn;
using namespace mic::types;

#include <types/Gridworld.hpp>

namespace mic {
namespace application {


/*!
 * \brief Class responsible for solving the gridworld problem with Q-learning and (not that) deep neural networks.
 * \author tkornuta
 */
class GridworldDeepQLearning: public mic::opengl::application::OpenGLEpisodicApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	GridworldDeepQLearning(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~GridworldDeepQLearning();

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
	WindowFloatCollectorChart* w_chart;

	/// Data collector.
	mic::data_io::DataCollectorPtr<std::string, float> collector_ptr;

	/// The gridworld object - current state.
	mic::types::Gridworld state;

	/// Property: type of gridworld:
	/// 0: the exemplary grid 4x3.
	/// 1: the classic cliff grid 5x3.
	/// 2: the classic discount grid 5x5.
	/// 3: the classic bridge grid 7x3.
	/// 4: the classic book grid 4x4.
	/// 5: the classic maze grid 4x4.
	/// 6: gridworld from DQL example 4x4.
	/// -1 (or else): random grid - all items (wall, goal and pit, player) placed randomly
	mic::configuration::Property<short> gridworld_type;

	/// Property: width of gridworld.
	mic::configuration::Property<size_t> width;

	/// Property: height of gridworld.
	mic::configuration::Property<size_t> height;

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

	/// Multi-layer neural network used for approximation of the Qstate rewards.
	MultiLayerNeuralNetwork neural_net;

	/*!
	 * Matrix containing the encoded rewards at state t.
	 */
	MatrixXfPtr predicted_rewards_t;

	/*!
	 * Performs "deterministic" move. It is assumed that the move is truncated by the gridworld boundaries (no circular world assumption).
	 * @param ac_ The action to be performed.
	 * @return True if move was performed, false if it was not possible.
	 */
	bool move (mic::types::Action2DInterface ac_);

	/*!
	 * Calculates the best value for the current state - by finding the action having the maximal expected value.
	 * @return Value for given state.
	 */
	float computeBestValue();


	/*!
	 * Finds the best action for the current state.
	 * @return The best action found.
	 */
	mic::types::NESWAction selectBestAction();

};

} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_GRIDWORLDDEEPQLEARNING_HPP_ */
