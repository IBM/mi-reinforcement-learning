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
 * \file MNISTDigit.hpp
 * \brief 
 * \author tkornut
 * \date Jun 8, 2016
 */

#ifndef SRC_TYPES_MNISTDIGIT_HPP_
#define SRC_TYPES_MNISTDIGIT_HPP_

#include <types/Environment.hpp>
#include <data_io/MNISTMatrixImporter.hpp>


namespace mic {
namespace environments {

/*!
 * \brief MNIST Digit environment channels
 * \author tkornuta
 */
enum class MNISTDigitChannels : std::size_t
{
	Pixels = 0, ///< Channel storing image intensities (this is a grayscale image)
	Goals = 1, ///< Channel storing goal(s)
	Agent = 2, ///< Channel storing the agent position
	Count = 3 ///< Number of channels
};


/*!
 * \brief Class emulating the MNISTDigit digit environment.
 * \author tkornuta
 */
class MNISTDigit : public mic::environments::Environment {
public:
	/*!
	 * Constructor. Registers properties.
	 * @param node_name_ Name of the node in configuration file.
	 */
	MNISTDigit(std::string node_name_ = "mnist_digit");

	/*!
	 * Copying constructor.
	 * @param md_ MNIST digit to be cloned.
	 */
	MNISTDigit (const mic::environments::MNISTDigit & md_);

	/*!
	 * Destructor. Empty for now.
	 */
	virtual ~MNISTDigit();

	/*!
	 * Assign operator. Copies the MNISTDigit state along with its properties.
	 */
	mic::environments::MNISTDigit & operator=(const mic::environments::MNISTDigit & md_);

	/*!
	 * Initializes all variables that are property-dependent - loads MNISTDigit digits.
	 */
	virtual void initializePropertyDependentVariables();

	/*!
	 * (Re)initializes the environment - gets given MNIST digit (sample number), sets agent, goal etc.
	 */
	virtual void initializeEnvironment();

	/*!
	 * Returns the tensor being the observation.
	 * @return Observation tensor of size [roi_size, roi_size, channels].
	 */
	mic::types::TensorXfPtr getObservation();

	/*!
	 * Returns the current state of the MNISTDigit in the form of a string.
	 * @return String with description of the MNISTDigit.
	 */
	virtual std::string environmentToString();

	/*!
	 * Returns the current observation taken in the MNISTDigit in the form of a string.
	 * @return String with description of the observation.
	 */
	virtual std::string observationToString();

	/*!
	 * Encodes the current state of the MNISTDigit in as a matrix of size [1, width * height * channels].
	 * @return Matrix of size [1, width * height * channels].
	 */
	virtual mic::types::MatrixXfPtr encodeEnvironment();

	/*!
	 * Encodes the current observation taken in the environment in as a matrix of size [1, roi_size * roi_size * channels].
	 * @return Matrix of size [1, roi_size * roi_size * channels].
	 */
	virtual mic::types::MatrixXfPtr encodeObservation();

	/*!
	 * Calculates the agent position.
	 * @return Agent position.
	 */
	virtual mic::types::Position2D getAgentPosition();

	/*!
	 * Moves the agent to the position.
	 * Type of move (deterministic vs stochastic) depends on the environment (the same goes to e.g. circular world assumption).
	 * @param pos_ Desired position of the agent.
	 * @return True if position is valid and was reached, false otherwise.
	 */
	virtual bool moveAgentToPosition(mic::types::Position2D pos_);

	/*!
	 * Returns the reward associated with the given state.
	 * @param pos_ Position (state).
	 * @return Reward for being in given state (r).
	 */
	virtual float getStateReward(mic::types::Position2D pos_);

	// Makes all versions of polymorphic method isStateAllowed() available.
	using mic::environments::Environment::isStateAllowed;

	/*!
	 * Checks if position is allowed, i.e. within the gridworld boundaries and there is no wall at that place.
	 * @param pos_ Position to be checked.
	 * @return True if the position is allowed, false otherwise.
	 */
	virtual bool isStateAllowed(mic::types::Position2D pos_);

	// Makes all versions of polymorphic method isStateTerminal() available.
	using mic::environments::Environment::isStateTerminal;

	/*!
	 * Checks if position is terminal, i.e. agent is standing in a pit or reached the goal. Returns reward associated with given state.
	 * @param pos_ Position (state) to be checked.
	 * @return The reward associated with "final" action (might be positive or negative), equal to zero means that the position is not final.
	 */
	virtual bool isStateTerminal(mic::types::Position2D pos_);

	/*!
	 * Returns the length of optimal path from agent initial position to goal.
	 * @return
	 */
	unsigned int optimalPathLength(){
		return optimal_path_length;
	}

protected:

	/// Importer responsible for loading MNIST dataset.
	mic::data_io::MNISTMatrixImporter<float> mnist_importer;

	/*!
	 * Property: MNISTDigit digit - number of sample.
	 */
	mic::configuration::Property<size_t> sample_number;

	/*!
	 * Property: initial agent x position.
	 */
	mic::configuration::Property<short> agent_x;

	/*!
	 * Property: initial agent y position.
	 */
	mic::configuration::Property<short> agent_y;

	/*!
	 * Property: initial goal x position.
	 */
	mic::configuration::Property<short> goal_x;

	/*!
	 * Property: initial goal y position.
	 */
	mic::configuration::Property<short> goal_y;

	/*!
	 * Optimal number of steps from initial agent position to goal.
	 */
	unsigned int optimal_path_length;


	/*!
	 * Returns the current state of the environment passed as an argument in the form of a string.
	 * @param env_ Environment (tensor) to be described.
	 * @return String with description of the environment.
	 */
	std::string toString(mic::types::TensorXfPtr env_);

};

} /* namespace environments */
} /* namespace mic */

#endif /* SRC_TYPES_MNISTDIGIT_HPP_ */
