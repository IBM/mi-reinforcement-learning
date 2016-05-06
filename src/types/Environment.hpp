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
#include <logger/Log.hpp>

#include <configuration/PropertyTree.hpp>

namespace mic {
namespace environments {

/*!
 * \brief Abstract class representing an environment.
 * \author tkornuta
 */
class Environment : public mic::configuration::PropertyTree {
public:
	/*!
	 * Constructor. Registers properties.
	 * @param node_name_ Name of the node in configuration file.
	 */
	Environment(std::string node_name_);


	/*!
	 * Virtual destructor. Empty.
	 */
	virtual ~Environment();

	/*!
	 * Returns the tensor storing the environment.
	 * @return Tensor storing the environment.
	 */
	mic::types::TensorXf & get() { return environment_grid; }

	/*!
	 * Returns current width of the environment.
	 * @return Width.
	 */
	size_t getEnvironmentWidth() { return width; }

	/*!
	 * Returns current height of the environment.
	 * @return Height.
	 */
	size_t getEnvironmentHeight() { return height; }

	/*!
	 * Returns the environment size (width * height * channels).
	 * @return Size of the environment.
	 */
	size_t getEnvironmentSize() { return width * height * channels; }

	/*!
	 * Returns the width of the observation.
	 * @return Width.
	 */
	size_t getObservationWidth() { return ((!pomdp_flag) ? width : roi_size); }

	/*!
	 * Returns the height of the observation.
	 * @return Height.
	 */
	size_t getObservationHeight() { return ((!pomdp_flag) ?height : roi_size); }

	/*!
	 * Returns the observation size, depending on the process type: FOMDP (width * height * channels) or POMDP (roi_size * roi_size * channels).
	 * @return Size of the observation.
	 */
	size_t getObservationSize() { return ((!pomdp_flag) ? width * height * channels : roi_size * roi_size * channels); }

	/*!
	 * Returns number of channels (depth) of the environment.
	 * @return Height.
	 */
	size_t getChannels() { return channels; }

	/*!
	 * Returns size of the region of iterest.
	 * @return ROI size.
	 */
	size_t getROISize() { return roi_size; }


	/*!
	 * Returns the current state of the environment in the form of a string.
	 * @return String with description of the environment.
	 */
	virtual std::string environmentToString() = 0;

	/*!
	 * Returns the current observation taken in the environment in the form of a string.
	 * @return String with description of the observation.
	 */
	virtual std::string observationToString() = 0;

	/*!
	 * Encodes the current state of the environment in as a matrix of size [1, width * height * channels].
	 * @return Matrix of size [1, width * height * channels].
	 */
	virtual mic::types::MatrixXfPtr encodeEnvironment() = 0;


	/*!
	 * Encodes the current observation taken in the environment in as a matrix of size [1, roi_size * roi_size * channels].
	 * @return Matrix of size [1, roi_size * roi_size * channels].
	 */
	virtual mic::types::MatrixXfPtr encodeObservation() = 0;

	/*!
	 * Calculates the agent position.
	 * @return Agent position.
	 */
	virtual mic::types::Position2D getAgentPosition() = 0;

	/*!
	 * Moves agent according to the selected action.
	 * @param ac_ Action to be performed.
	 * @return True if action was performed, false if destination state was invalid.
	 */
	bool moveAgent (mic::types::Action2DInterface ac_);


	/*!
	 * Moves the agent to given position.
	 * @param pos_ Desired position of the agent.
	 * @return True if position is valid and was reached, false otherwise.
	 */
	virtual bool moveAgentToPosition(mic::types::Position2D pos_) = 0;

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

	/// Property: width of the environment.
	mic::configuration::Property<size_t> width;

	/// Property: height of the environment.
	mic::configuration::Property<size_t> height;

	/// Property: size of the ROI (region of interest).
	mic::configuration::Property<size_t> roi_size;

	/// Number of channels.
	size_t channels;

	/// Flag related to
	bool pomdp_flag;

	/// Property: initial position of the agent.
	mic::types::Position2D initial_position;

	/// Tensor storing the environment (x, y, "depth" channels, representing: 0 - goals + pits, 1 - walls, 2 - agent).
	mic::types::TensorXf environment_grid;


};

} /* namespace environments */
} /* namespace mic */

#endif /* SRC_TYPES_ENVIRONMENT_HPP_ */
