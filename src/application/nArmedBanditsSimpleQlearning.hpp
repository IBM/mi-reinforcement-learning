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
#include <opengl/visualization/WindowCollectorChart.hpp>
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
	WindowCollectorChart<float>* w_reward;

	/// Reward collector.
	mic::utils::DataCollectorPtr<std::string, float> reward_collector_ptr;

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
