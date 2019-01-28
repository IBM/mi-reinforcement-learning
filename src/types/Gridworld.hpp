/*!
 * \file Gridworld.hpp
 * \brief 
 * \author tkornut
 * \date Mar 21, 2016
 */

#ifndef SRC_TYPES_GRIDWORLD_HPP_
#define SRC_TYPES_GRIDWORLD_HPP_

#include <types/Environment.hpp>


namespace mic {
namespace environments {

/*!
 * \brief Gridworld channels
 * \author tkornuta
 */
enum class GridworldChannels : std::size_t
{
	Goals = 0, ///< Channel storing goal(s)
	Pits = 1, ///< Channel storing  pit(s)
	Walls = 2, ///< Channel storing walls(s)
	Agent = 3, ///< Channel storing the agent position
	Count = 4 ///< Number of channels
};


/*!
 * \brief Class emulating the gridworld environment.
 * \author tkornuta
 */
class Gridworld : public mic::environments::Environment {
public:
	/*!
	 * Constructor. Registers properties.
	 * @param node_name_ Name of the node in configuration file.
	 */
	Gridworld(std::string node_name_ = "gridworld");

	/*!
	 * Destructor. Empty for now.
	 */
	virtual ~Gridworld();

	/*!
	 * Assign operator. Copies the gridworld state along with its properties.
	 */
	mic::environments::Gridworld & operator=(const mic::environments::Gridworld & gw_);

	/*!
	 * Initializes all variables that are property-dependent.
	 */
	virtual void initializePropertyDependentVariables();

	/*!
	 * (Re)initializes the environment - generates the gridworld of a required (defined by property) type, sets agent, goal etc.
	 */
	virtual void initializeEnvironment();

	/*!
	 * 	Method initializes the exemplary grid.
	 *
	 * [[' ',' ',' ',' '],
	 *  ['S',-10,' ',' '],
	 *  [' ','','#',' '],
	 *  [' ',' ',' ',10]]
	 */
	void initExemplaryGrid();

	/*!
	 * Initializes the classic cliff gridworld.
	 *
	 * [[' ',' ',' ',' ',' '],
	 *  ['S',' ',' ',' ',10],
	 *  [-100,-100, -100, -100, -100]]
	 */
	void initClassicCliffGrid();

	/*!
	 * Initializes the classic discount gridworld.
	 *
	 * [[' ',' ',' ',' ',' '],
	 *  [' ','#',' ',' ',' '],
	 *  [' ','#', 1,'#', 10],
	 *   ['S',' ',' ',' ',' '],
	 *   [-10,-10, -10, -10, -10]]
	 */
	void initDiscountGrid();

	/*!
	 * Initializes the classic discount gridworld.
	 *
	 * [[ '#',-100, -100, -100, -100, -100, '#'],
	 *  [   1, 'S',  ' ',  ' ',  ' ',  ' ',  10],
	 *  [ '#',-100, -100, -100, -100, -100, '#']]
	 */
	void initBridgeGrid();

	/*!
	 * Initializes the classic Book gridworld - example from Sutton&Barto book on RL.
	 *
	 * [[' ',' ',' ',+1],
	 *  [' ','#',' ',-1],
	 *  ['S',' ',' ',' ']]
	 */
	void initBookGrid();

	/*!
	 * Initializes the classic maze gridworld.
	 *
	 * [[' ',' ',' ',+1],
	 *  ['#','#',' ','#'],
	 *  [' ','#',' ',' '],
	 *  [' ','#','#',' '],
	 *  ['S',' ',' ',' ']]
	 */
	void initMazeGrid();

	/*!
	 * 	Method initializes the grid from Deep Q-Learning example.
	 *
	 * [[' ',' ',' ',' '],
	 *  [' ',' ',+10,' '],
	 *  [' ','#',-10,' '],
	 *  ['S',' ',' ',' ']]
	 */
	void initExemplaryDQLGrid();

	/*!
	 * 	Method initializes a slightly modified grid from Deep Q-Learning example.
	 *
	 * [[' ',' ',' ',' '],
	 *  [' ','#',+10,' '],
	 *  [' ',' ',-10,' '],
	 *  ['S',' ',' ',' ']]
	 */
	void initModifiedDQLGrid();


	/*!
	 * 	Method initializes the 2x2 grid useful during the debugging.
	 *
	 * [['S',-10],
	 *  [+10,' ']]
	 */
	void initDebug2x2Grid();

	/*!
	 * 	Method initializes the 3x3 grid useful during the debugging.
	 *
	 * [[' ',-10,' '],
	 *  [-10,'S',-10],
	 *  [' ',+10,' ']]
	 */
	void initDebug3x3Grid();


	/*!
	 * Generates a random grid of size (width x height), with a single pit, goal and wall.
	 */
	void initSimpleRandomGrid();

	/*!
	 * Generates a random grid of size (width x height), with a single goal, but several walls and pits.
	 */
	void initHardRandomGrid();

	/*!
	 * A recursive method for checking whether the grid is traversable (i.e. there is a path from agent to goal).
	 * @param x_ Current x coordinate to check.
	 * @param y_ Current x coordinate to check.
	 * @param visited_ Matrix with visited states.
	 */
	bool isGridTraversible(long x_, long y_, mic::types::Matrix<bool> & visited_);

	/*!
	 * Returns the tensor being the observation.
	 * @return Observation tensor of size [roi_size, roi_size, channels].
	 */
	mic::types::TensorXfPtr getObservation();

	/*!
	 * Returns the current state of the gridworld in the form of a string.
	 * @return String with description of the gridworld.
	 */
	virtual std::string environmentToString();

	/*!
	 * Returns the current observation taken in the gridworld in the form of a string.
	 * @return String with description of the observation.
	 */
	virtual std::string observationToString();

	/*!
	 * Encodes the current state of the gridworld in as a matrix of size [1, width * height * channels].
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

protected:

	/*!
	 * Property: type of the generated gridworld.
	 * Currently available types:
	 * 0: the exemplary grid 4x3.
	 * 1: the classic cliff grid 5x3.
	 * 2: the classic discount grid 5x5.
	 * 3: the classic bridge grid 7x3.
	 * 4: the classic book grid 4x4.
	 * 5: the classic maze grid 4x4.
	 * 6: gridworld from DQL example 4x4.
	 * 7: slightly modified gridworld from DQL example 4x4.
	 * 8: debug grid 2x2.
	 * 9: debug grid 3x3.
	 * -1 (or else): random grid - all items (wall, goal and pit, agent) placed randomly
	 * -2 (or else): random grid - all items (wall, goal and pit, agent) placed randomly with multiple pits and walls
	 */
	mic::configuration::Property<short> type;

	/*!
	 * Returns the current state of the grid passed as an argument in the form of a string.
	 * @param grid_ Grid to be processed.
	 * @return String with description of the grid.
	 */
	std::string gridToString(mic::types::TensorXfPtr grid_);

};

} /* namespace environments */
} /* namespace mic */

#endif /* SRC_TYPES_GRIDWORLD_HPP_ */
