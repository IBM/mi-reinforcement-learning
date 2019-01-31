/*!
 * Copyright (C) tkornuta, IBM Corporation 2015-2019
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file GridworldDRLExperienceReplay.hpp
 * \brief 
 * \author tkornut
 * \date Apr 26, 2016
 */

#ifndef SRC_APPLICATION_GRIDWORLDDRLEXPERIENCEREPLAY_HPP_
#define SRC_APPLICATION_GRIDWORLDDRLEXPERIENCEREPLAY_HPP_

#include <vector>
#include <string>

#include <opengl/application/OpenGLEpisodicApplication.hpp>
#include <opengl/visualization/WindowCollectorChart.hpp>
using namespace mic::opengl::visualization;

#include <mlnn/BackpropagationNeuralNetwork.hpp>
// Using multi layer neural networks
using namespace mic::mlnn;
using namespace mic::types;

#include <types/Gridworld.hpp>
#include <types/SpatialExperienceMemory.hpp>

namespace mic {
namespace application {


/*!
 * \brief Class responsible for solving the gridworld problem with Q-learning, neural network used for approximation of the rewards and experience replay using for (batch) training of the neural network.
 * \author tkornuta
 */
class GridworldDRLExperienceReplay: public mic::opengl::application::OpenGLEpisodicApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	GridworldDRLExperienceReplay(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~GridworldDRLExperienceReplay();

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

	/// Window for displaying statistics.
	WindowCollectorChart<float>* w_chart;

	/// Data collector.
	mic::data_io::DataCollectorPtr<std::string, float> collector_ptr;

	/// The gridworld environment.
	mic::environments::Gridworld grid_env;

	/// Size of the batch in experience replay - set to the size of maze (width*height).
	size_t batch_size;

	/*!
	 * Property: the "expected intermediate reward", i.e. reward received by performing each step (typically negative, but can be positive as all).
	 */
	mic::configuration::Property<float> step_reward;

	/*!
	 * Property: future discount (should be in range 0.0-1.0).
	 */
	mic::configuration::Property<float> discount_rate;

	/*!
	 * Property: neural network learning rate (should be in range 0.0-1.0).
	 */
	mic::configuration::Property<float> learning_rate;

	/*!
	 * Property: variable denoting epsilon in action selection (the probability "below" which a random action will be selected).
	 * if epsilon < 0 then if will be set to 1/episode, hence change dynamically depending on the episode number.
	 */
	mic::configuration::Property<double> epsilon;

	/// Property: name of the file to which the statistics will be exported.
	mic::configuration::Property<std::string> statistics_filename;

	/// Property: name of the file to which the neural network will be serialized (or deserialized from).
	mic::configuration::Property<std::string> mlnn_filename;

	/// Property: flad denoting thether the nn should be saved to a file (after every episode end).
	mic::configuration::Property<bool> mlnn_save;

	/// Property: flad denoting thether the nn should be loaded from a file (at the initialization of the task).
	mic::configuration::Property<bool> mlnn_load;

	/// Multi-layer neural network used for approximation of the Qstate rewards.
	BackpropagationNeuralNetwork<float> neural_net;

	/*!
	 * Calculates the best value for the current state and predictions.
	 * @param player_position_ State (player position).
	 * @param predictions_ Vector of predictions to be analyzed.
	 * @return Value of the best possible action for given state.
	 */
	float computeBestValueForGivenStateAndPredictions(mic::types::Position2D player_position_, float* predictions_);

	/*!
	 * Returns the predicted rewards for given state.
	 * @param player_position_ State (player position).
	 * @return Pointer to the predicted rewards (network output matrix).
	 */
	mic::types::MatrixXfPtr getPredictedRewardsForGivenState(mic::types::Position2D player_position_);

	/*!
	 * Finds the best action for the current state.
	 * @param player_position_ State (player position).
	 * @return The best action found.
	 */
	mic::types::NESWAction selectBestActionForGivenState(mic::types::Position2D player_position_);

	/*!
	 * Steams the current network response - values of actions associates with consecutive agent poses.
	 * @return Ostream with description of the state-action table.
	 */
	std::string streamNetworkResponseTable();

	/*!
	 * Sum of all iterations made till now - used in statistics.
	 */
	long long sum_of_iterations;

	/*!
	 * Sum of all rewards collected till now - used in statistics.
	 */
	long long sum_of_rewards;

	/*!
	 * Number of successes, i.e. how many times we reached goal till now - used in statistics.
	 */
	long long number_of_successes;

	/*!
	 * Table of past experiences.
	 */
	SpatialExperienceMemory experiences;
};

} /* namespace application */
} /* namespace mic */


#endif /* SRC_APPLICATION_GRIDWORLDDRLEXPERIENCEREPLAY_HPP_ */
