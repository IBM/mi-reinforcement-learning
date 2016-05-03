/*!
 * \file Environment.hpp
 * \brief 
 * \author tkornut
 * \date May 2, 2016
 */

#ifndef SRC_TYPES_ENVIRONMENT_HPP_
#define SRC_TYPES_ENVIRONMENT_HPP_

#include <types/Position2D.hpp>
#include <types/TensorTypes.hpp>
#include <types/MatrixTypes.hpp>

namespace mic {
namespace environments {

/*!
 * \brief Abstract class representing an environment.
 * \author tkornuta
 */
class Environment {
public:
	/*!
	 * Constructor. Sets dimensions
	 */
	Environment(size_t width_, size_t height_, size_t channels_);

	/*!
	 * Virtual destructor. Empty.
	 */
	virtual ~Environment();

	/*!
	 * Returns current width of the environment.
	 * @return Width.
	 */
	size_t getWidth() { return width; }

	/*!
	 * Returns current height of the environment.
	 * @return Height.
	 */
	size_t getHeight() { return height; }

	/*!
	 * Returns number of channels (depth) of the environment.
	 * @return Height.
	 */
	size_t getChannels() { return channels; }

	/*!
	 * Returns the tensor storing the environment.
	 * @return Tensor storing the environment.
	 */
	mic::types::TensorXf & get() { return environment_grid; }

	/*!
	 * Returns the current state of the environment in the form of a string.
	 * @return String with description of the environment.
	 */
	virtual std::string toString() = 0;


	/// Encodes the current state of the environment in as a matrix of size [1, environment * dimensions].
	virtual mic::types::MatrixXfPtr encode() = 0;


	/*!
	 * Calculates the agent position.
	 * @return Agent position.
	 */
	virtual mic::types::Position2D getAgentPosition() = 0;

	/*!
	 * Moves the agent to given position.
	 * @param pos_ Position to be checked.
	 * @param pos_ The position to be set.
	 */
	virtual void moveAgentToPosition(mic::types::Position2D pos_) = 0;

	/*!
	 * Moves the agent to the initial position.
	 * @param pos_ Position to be checked.
	 * @param pos_ The position to be set.
	 */
	virtual void moveAgentToInitialPosition();

	/*!
	 * Returns the reward associated with the given state.
	 * @param pos_ Position (state).
	 * @return Reward for being in given state (r).
	 */
	virtual float getStateReward(mic::types::Position2D pos_) = 0;


	/*!
	 * Checks if position is allowed, i.e. within the gridworld boundaries and there is no wall at that place.
	 * @param x_ X state coordinate
	 * @param y_ Y state coordinate
	 * @return True if the position is allowed, false otherwise.
	 */
	virtual bool isStateAllowed(long x_, long y_);

	/*!
	 * Checks if position is allowed, i.e. within the gridworld boundaries and there is no wall at that place.
	 * @param pos_ Position to be checked.
	 * @return True if the position is allowed, false otherwise.
	 */
	virtual bool isStateAllowed(mic::types::Position2D pos_) = 0;

	/*!
	 * Checks if position is terminal, i.e. agent is standing in a pit or reached the goal. Returns reward associated with given state.
	 * @param x_ X state coordinate
	 * @param y_ Y state coordinate
	 * @return The reward associated with "final" action (might be positive or negative), equal to zero means that the position is not final.
	 */
	virtual bool isStateTerminal(long x_, long y_);


	/*!
	 * Checks if position is terminal, i.e. agent is standing in a pit or reached the goal. Returns reward associated with given state.
	 * @param pos_ Position (state) to be checked.
	 * @return The reward associated with "final" action (might be positive or negative), equal to zero means that the position is not final.
	 */
	virtual bool isStateTerminal(mic::types::Position2D pos_) = 0;


	/*!
	 * Checks whether performing given action starting in given state is allowed.
	 * @param x_ X state coordinate
	 * @param y_ Y state coordinate
	 * @param action_ Action to be performed starting from given state.
	 * @return True if action is allowed, false otherwise.
	 */
	virtual bool isActionAllowed(long x_, long y_, size_t action_);

	/*!
	 * Checks whether performing given action starting in given state is allowed.
	 * @param pos_ Starting state (position).
	 * @param ac_ Action to be performed.
	 * @return True if action is allowed, false otherwise.
	 */
	virtual bool isActionAllowed(mic::types::Position2D pos_, mic::types::Action2DInterface ac_);

	/*!
	 * Checks whether performing given action from the current agent state is allowed.
	 * @param ac_ Action to be performed.
	 * @return True if action is allowed, false otherwise.
	 */
	virtual bool isActionAllowed(mic::types::Action2DInterface ac_);


protected:

	/// Width of gridworld.
	size_t width;

	/// Height of gridworld.
	size_t height;

	/// Number of gridworld channels.
	size_t channels;

	/// Property: height of gridworld.
	mic::types::Position2D initial_position;

	/// Tensor storing the environment (x, y, "depth" channels, representing: 0 - goals + pits, 1 - walls, 2 - agent).
	mic::types::TensorXf environment_grid;


};

} /* namespace environments */
} /* namespace mic */

#endif /* SRC_TYPES_ENVIRONMENT_HPP_ */
