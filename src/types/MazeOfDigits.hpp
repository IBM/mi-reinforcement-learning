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
 * \file MazeOfDigits.hpp
 * \brief 
 * \author tkornut
 * \date May 2, 2016
 */

#ifndef SRC_TYPES_MAZEOFDIGITS_HPP_
#define SRC_TYPES_MAZEOFDIGITS_HPP_

#include <types/Environment.hpp>


namespace mic {
namespace environments {

/*!
 * \brief MazeOfDigits channels
 * \author tkornuta
 */
enum class MazeOfDigitsChannels : std::size_t
{
	Digits = 0, ///< Channel storing digits
	Goals = 1, ///< Channel storing goal(s)
	Walls = 2, ///< Channel storing walls(s)
	Agent = 3, ///< Channel storing the agent position
	Count = 4 ///< Number of channels
};


/*!
 * \brief Class emulating the maze of digits environment.
 * \author tkornuta
 */
class MazeOfDigits : public mic::environments::Environment {
public:
	/*!
	 * Constructor. Registers properties.
	 * @param node_name_ Name of the node in configuration file.
	 */
	MazeOfDigits(std::string node_name_ = "maze_of_digits");

	/*!
	 * Copying constructor.
	 * @param md_ Maze of digits to be cloned.
	 */
	MazeOfDigits (const mic::environments::MazeOfDigits & md_);

	/*!
	 * Destructor. Empty for now.
	 */
	virtual ~MazeOfDigits();

	/*!
	 * Assign operator. Copies the gridworld state along with its properties.
	 */
	mic::environments::MazeOfDigits & operator=(const mic::environments::MazeOfDigits & md);

	/*!
	 * Returns the observation size, depending on the process type: FOMDP (width * height * channels) or POMDP (roi_size * roi_size * 1!) (an overridden method)
	 * @return Size of the observation.
	 */
	virtual size_t getObservationSize() { return ((!pomdp_flag) ? width * height * channels : roi_size * roi_size * 1); }

	/*!
	 * Initializes all variables that are property-dependent.
	 */
	virtual void initializePropertyDependentVariables();

	/*!
	 * (Re)initializes the environment - generates the maze of a required (defined by property) type, sets agent, goal etc.
	 */
	virtual void initializeEnvironment();

	/*!
	 * 	Method initializes the exemplary maze.
	 *
	 * [['2','4','7','7'],
	 *  ['1','5','7','9'],
	 *  ['2','3','6','8'],
	 *  ['A','2','5','6']]
	 */
	void initExemplaryMaze();

	/*!
	 * Generates only the agent new position, leaving the maze unchanged. Recalculates optimal path length to (unchanged) goal.
	 */
	void reRandomAgentPosition();

	/*!
	 * Generates a fully random maze of size (width x height), with spatially independent values of digits.
	 */
	void initFullyRandomMaze();

	/*!
	 * Generates a random maze of size (width x height), with spatially dependent values of digits, creating a heat map around the goal (9).
	 */
	void initRandomStructuredMaze();

	/*!
	 * Generates a random maze of size (width x height), with spatially dependent values of digits, creating a path leading to the goal (9).
	 */
	void initRandomPathMaze();

	/*!
	 * Sets the digit.
	 * @param point_
	 * @param value_
	 */
	void setBiggerDigit(size_t x_, size_t y_, size_t value_);

	/*!
	 * Returns the tensor being the observation.
	 * @return Observation tensor of size [roi_size, roi_size, channels].
	 */
	mic::types::TensorXfPtr getObservation();

	/*!
	 * Returns the current state of the environment in the form of a string.
	 * @return String with description of the environment.
	 */
	virtual std::string environmentToString();

	/*!
	 * Returns the current observation taken in the environment in the form of a string.
	 * @return String with description of the observation.
	 */
	virtual std::string observationToString();

	/*!
	 * Encodes the current state of the environment in as a matrix of size [1, width * height * channels].
	 * @return Matrix of size [1, width * height * channels].
	 */
	virtual mic::types::MatrixXfPtr encodeEnvironment();

	/*!
	 * Encodes the current observation taken in the environment in as a matrix of size [1, roi_size * roi_size * channels].
	 * @return Matrix of size [1, roi_size * roi_size * channels].
	 */
	virtual mic::types::MatrixXfPtr encodeObservation();


	/// Encode the current state of the reduced grid (only the agent position) as a matrix of size [1, width * height]
	virtual mic::types::MatrixXfPtr encodeAgentGrid();

	/*!
	 * Calculates the agent position.
	 * @return Agent position.
	 */
	virtual mic::types::Position2D getAgentPosition();

	/*!
	 * Moves the agent to the position.Type of move (deterministic vs stochastic) depends on the environment (the same goes to e.g. circular world assumption).
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

	/*!
	 * Property: type of the generated gridworld.
	 * Currently available types:
	 * 0: the exemplary maze 4x4.
	 * -1 (or else): random maze - random maze generated, but generated only once, random initial agent position in each episode
	 * -2 (or else): random maze - all randomly generated each time
	 */
	mic::configuration::Property<short> type;

	/*!
	 * Returns the current state of the grid passed as an argument in the form of a string.
	 * @param grid_ Grid to be processed.
	 * @return String with description of the grid.
	 */
	std::string gridToString(mic::types::TensorXfPtr & grid_);

	/*!
	 * Optimal number of steps from initial agent position to goal.
	 */
	unsigned int optimal_path_length;


};

} /* namespace environments */
} /* namespace mic */

#endif /* SRC_TYPES_MAZEOFDIGITS_HPP_ */
