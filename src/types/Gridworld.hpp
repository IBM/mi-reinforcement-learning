/*!
 * \file Gridworld.hpp
 * \brief 
 * \author tkornut
 * \date Mar 21, 2016
 */

#ifndef SRC_TYPES_GRIDWORLD_HPP_
#define SRC_TYPES_GRIDWORLD_HPP_

#include <logger/Log.hpp>
#include <types/Environment.hpp>


namespace mic {
namespace environments {

/*!
 * \brief Gridworld channels
 * \author tkornuta
 */
enum class GridworldChannels : std::size_t
{
	Rewards = 0, ///< Channel storing the goal(s) and pit(s)
	Walls = 1, ///< Channel storing the walls(s)
	Agent = 2, ///< Channel storing the agent position
	Count = 3 ///< Number of channels
};


/*!
 * \brief Class responsible for generation and presentation of gridworld environments.
 * \author tkornuta
 */
class Gridworld : public mic::environments::Environment {
public:
	/*!
	 * Constructor. Creates and empty gridworld (set size to 0x0).
	 */
	Gridworld();

	/*!
	 * Destructor. Empty for now.
	 */
	virtual ~Gridworld();

	/*!
	 * Assign operator. Copies the gridworld state along with its properties.
	 */
	mic::environments::Gridworld & operator=(const mic::environments::Gridworld & gw_);

	/*!
	 * Genrates the gridworld of a given, predefined type.
	 * @param gridworld_type_ Type of the generated gridworld.
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
	 * @param width_ Grid width (used in the case of random grid generation).
	 * @param height_ Grid height (used in the case of random grid generation).
	 */
	void generateGridworld(int gridworld_type_, size_t width_, size_t height_);

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
	void initSimpleRandomGrid(size_t width_, size_t height_);

	/*!
	 * Generates a random grid of size (width x height), with a single goal, but several walls and pits.
	 */
	void initDifficultRandomGrid(size_t width_, size_t height_);

	/*!
	 * A recursive method for checking whether the grid is traversable (i.e. there is a path from agent to goal).
	 * @param x_ Current x coordinate to check.
	 * @param y_ Current x coordinate to check.
	 * @param visited_ Matrix with visited states.
	 */
	bool isGridTraversible(long x_, long y_, mic::types::Matrix<bool> & visited_);

	/*!
	 * Returns the current state of the gridworld in the form of a string.
	 * @return String with description of the gridworld.
	 */
	virtual std::string toString();

	/// Encode the current state of the grid (walls, pits, goals and agent position) as a matrix of size [1, width * height * channels]
	virtual mic::types::MatrixXfPtr encode();

	/// Encode the current state of the reduced grid (only the agent position) as a matrix of size [1, width * height]
	virtual mic::types::MatrixXfPtr encodeAgentGrid();

	/*!
	 * Calculates the agent position.
	 * @return Agent position.
	 */
	virtual mic::types::Position2D getAgentPosition();

	/*!
	 * Moves the agent to the position.
	 * @param pos_ Position to be checked.
	 * @param pos_ The position to be set.
	 */
	virtual void moveAgentToPosition(mic::types::Position2D pos_);

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

};

} /* namespace environments */
} /* namespace mic */

#endif /* SRC_TYPES_GRIDWORLD_HPP_ */
