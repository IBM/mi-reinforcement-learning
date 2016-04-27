/*!
 * \file Gridworld.hpp
 * \brief 
 * \author tkornut
 * \date Mar 21, 2016
 */

#ifndef SRC_TYPES_GRIDWORLD_HPP_
#define SRC_TYPES_GRIDWORLD_HPP_

#include <types/Position2D.hpp>
#include <types/TensorTypes.hpp>
#include <types/MatrixTypes.hpp>
#include <logger/Log.hpp>

namespace mic {
namespace types {

/*!
 * \brief Gridworld channels
 * \author tkornuta
 */
enum class GridworldChannels : std::size_t
{
	Goal = 0, ///< Channel storing the goal(s)
	Pit = 1, ///< Channel storing the pits(s)
	Wall = 2, ///< Channel storing the walls(s)
	Player = 3 ///< Channel storing the player pose
};


/*!
 * \brief Class responsible for generation and presentation of gridworld environments.
 * \author tkornuta
 */
class Gridworld {
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
	mic::types::Gridworld & operator=(const mic::types::Gridworld & gw_);

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
	 * 7: alightly gridworld from DQL example 4x4.
	 * 8: debug grid 2x2.
	 * 9: debug grid 3x3.
	 * -1 (or else): random grid - all items (wall, goal and pit, player) placed randomly
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
	 * Generates a random grid of size (width x height).
	 */
	void initRandomGrid(size_t width_, size_t height_);

	/*!
	 * Returns the (flattened, i.e. 2D) grid of characters.
	 * @return Flattened grid of chars.
	 */
	mic::types::Tensor<char> flattenGrid();

	/*!
	 * Steams the current state of the gridworld.
	 * @return Ostream with description of the gridworld.
	 */
	std::string streamGrid();

	/// Encode the current state of the grid (walls, pits, goals and player position) as a matrix of size [1, width * height * 4]
	mic::types::MatrixXfPtr encodeWholeGrid();

	/// Encode the current state of the reduced grid (only the player position) as a matrix of size [1, width * height]
	mic::types::MatrixXfPtr encodePlayerGrid();

	/*!
	 * Calculates the player position.
	 * @return Player position.
	 */
	mic::types::Position2D getPlayerPosition();

	/*!
	 * Move player to the position.
	 * @param pos_ Position to be checked.
	 * @param pos_ The position to be set.
	 */
	void movePlayerToPosition(mic::types::Position2D pos_);

	/*!
	 * Move player to the initial position.
	 * @param pos_ Position to be checked.
	 * @param pos_ The position to be set.
	 */
	void movePlayerToInitialPosition();

	/*!
	 * Returns the reward associated with the given state.
	 * @param pos_ Position (state).
	 * @return Reward for being in given state (r).
	 */
	float getStateReward(mic::types::Position2D pos_);


	/*!
	 * Checks if position is allowed, i.e. within the gridworld boundaries and there is no wall at that place.
	 * @param x_ X state coordinate
	 * @param y_ Y state coordinate
	 * @return True if the position is allowed, false otherwise.
	 */
	bool isStateAllowed(long x_, long y_);

	/*!
	 * Checks if position is allowed, i.e. within the gridworld boundaries and there is no wall at that place.
	 * @param pos_ Position to be checked.
	 * @return True if the position is allowed, false otherwise.
	 */
	bool isStateAllowed(mic::types::Position2D pos_);

	/*!
	 * Checks if position is terminal, i.e. player is standing in a pit or reached the goal. Returns reward associated with given state.
	 * @param x_ X state coordinate
	 * @param y_ Y state coordinate
	 * @return The reward associated with "final" action (might be positive or negative), equal to zero means that the position is not final.
	 */
	bool isStateTerminal(long x_, long y_);


	/*!
	 * Checks if position is terminal, i.e. player is standing in a pit or reached the goal. Returns reward associated with given state.
	 * @param pos_ Position (state) to be checked.
	 * @return The reward associated with "final" action (might be positive or negative), equal to zero means that the position is not final.
	 */
	bool isStateTerminal(mic::types::Position2D pos_);


	/*!
	 * Checks whether performing given action starting in given state is allowed.
	 * @param x_ X state coordinate
	 * @param y_ Y state coordinate
	 * @param action_ Action to be performed starting from given state.
	 * @return True if action is allowed, false otherwise.
	 */
	bool isActionAllowed(long x_, long y_, size_t action_);

	/*!
	 * Checks whether performing given action starting in given state is allowed.
	 * @param pos_ Starting state (position).
	 * @param ac_ Action to be performed.
	 * @return True if action is allowed, false otherwise.
	 */
	bool isActionAllowed(mic::types::Position2D pos_, mic::types::Action2DInterface ac_);

	/*!
	 * Checks whether performing given action from the current player state is allowed.
	 * @param ac_ Action to be performed.
	 * @return True if action is allowed, false otherwise.
	 */
	bool isActionAllowed(mic::types::Action2DInterface ac_);

	/*!
	 * Returns current width of the gridworld.
	 * @return Width.
	 */
	size_t getWidth() { return width; }

	/*!
	 * Returns current height of the gridworld.
	 * @return Height.
	 */
	size_t getHeight() { return height; }

	/*!
	 * Returns the grid tensor.
	 * @return Grid.
	 */
	mic::types::TensorXf & getGrid() { return gridworld; }

protected:

	/// Width of gridworld.
	size_t width;

	/// Height of gridworld.
	size_t height;

	/// Property: height of gridworld.
	mic::types::Position2D initial_position;

	/// Tensor storing the 3D Gridworld (x + y + 4 "depth" channels representing: 0 - goals, 1 - pits, 2 - walls, 3 - player).
	mic::types::TensorXf gridworld;

};

} /* namespace types */
} /* namespace mic */

#endif /* SRC_TYPES_GRIDWORLD_HPP_ */
