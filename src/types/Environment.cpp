/*!
 * \file Environment.cpp
 * \brief 
 * \author tkornut
 * \date May 2, 2016
 */

#include <types/Environment.hpp>

namespace mic {
namespace environments {

Environment::Environment(std::string node_name_) : PropertyTree(node_name_),
		width("width", 4),
		height("height", 4),
		roi_size("roi_size", 0),
		environment_grid(new mic::types::TensorXf()),
		observation_grid(new mic::types::TensorXf())
{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(width);
	registerProperty(height);
	registerProperty(roi_size);

	pomdp_flag = 0;
}

Environment::~Environment() {
	// TODO Auto-generated destructor stub
}


bool Environment::moveAgent (mic::types::Action2DInterface ac_) {
	mic::types::Position2D cur_pos = getAgentPosition();
	LOG(LDEBUG) << "Current agent position = " << cur_pos;
	LOG(LDEBUG) << "Move to be performed = " << ac_;
	// Compute destination.
	mic::types::Position2D new_pos = cur_pos + ac_;
	return moveAgentToPosition(new_pos);
}

void Environment::moveAgentToInitialPosition() {
	moveAgentToPosition(initial_position);
}


bool Environment::isStateAllowed(long x_, long y_) {
	return isStateAllowed(mic::types::Position2D(x_, y_));
}

bool Environment::isStateTerminal(long x_, long y_) {
	return isStateTerminal(mic::types::Position2D(x_, y_));
}

bool Environment::isActionAllowed(long x_, long y_, size_t action_) {
    mic::types::Position2D pos(x_,y_);
    mic::types::NESWAction ac(action_);
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos + ac;
    return isStateAllowed(new_pos);
}


bool Environment::isActionAllowed(mic::types::Position2D pos_, mic::types::Action2DInterface ac_) {
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos_ + ac_;
    return isStateAllowed(new_pos);
}

bool Environment::isActionAllowed(mic::types::Action2DInterface ac_) {
	// Get current player position.
	mic::types::Position2D pos = getAgentPosition();
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos + ac_;
    return isStateAllowed(new_pos);
}


} /* namespace environments */
} /* namespace mic */
