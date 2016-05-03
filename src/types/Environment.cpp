/*!
 * \file Environment.cpp
 * \brief 
 * \author tkornut
 * \date May 2, 2016
 */

#include <types/Environment.hpp>

namespace mic {
namespace environments {

Environment::Environment(size_t width_, size_t height_, size_t channels_) :
		width(width_),
		height(height_),
		channels(channels_)
{
	// TODO Auto-generated constructor stub

}

Environment::~Environment() {
	// TODO Auto-generated destructor stub
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
