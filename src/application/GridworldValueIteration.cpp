/*!
 * \file GridworldValueIteration
 * \brief Definitions of the methods of class responsible for solving the gridworld problem with value iteration.
 * \author tkornuta
 * \date Mar 17, 2016
 */


#include <limits>
#include <data_utils/RandomGenerator.hpp>
#include <application/GridworldValueIteration.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::GridworldValueIteration);
}


GridworldValueIteration::GridworldValueIteration(std::string node_name_) : Application(node_name_),
		gridworld_type("gridworld_type", 0),
		width("width", 4),
		height("height", 4),
		step_reward("step_reward", 0.0),
		discount_rate("discount_rate", 0.9),
		move_noise("move_noise",0.2),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(gridworld_type);
	registerProperty(width);
	registerProperty(step_reward);
	registerProperty(discount_rate);
	registerProperty(move_noise);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


GridworldValueIteration::~GridworldValueIteration() {

}


void GridworldValueIteration::initialize(int argc, char* argv[]) {

}

void GridworldValueIteration::initializePropertyDependentVariables() {
	// Generate the gridworld.
	gridworld.generateGridworld(gridworld_type, width, height);
	LOG(LSTATUS) << std::endl << gridworld.toString();

	// Get width and height.
	width = gridworld.getWidth();
	height = gridworld.getHeight();

	// Resize and reset the action-value table.
	state_value_table.resize(height,width);
	//state_value_table.zeros();
	state_value_table.setValue( -std::numeric_limits<float>::infinity() );
	running_delta = -std::numeric_limits<float>::infinity();

	LOG(LSTATUS) << std::endl << streamStateActionTable();
}



std::string GridworldValueIteration::streamStateActionTable() {
	std::ostringstream os;
	for (size_t y=0; y<height; y++){
		os << "| ";
		for (size_t x=0; x<width; x++) {
			if ( state_value_table(y,x) == -std::numeric_limits<float>::infinity())
				os << "-INF | ";
			else
				os << state_value_table(y,x) << " | ";
		}//: for x
		os << std::endl;
	}//: for y
	return os.str();

}


bool GridworldValueIteration::move (mic::types::Action2DInterface ac_) {
//	LOG(LINFO) << "Current move = " << ac_;
	// Compute destination.
    mic::types::Position2D new_pos = gridworld.getAgentPosition() + ac_;

	// Check whether the state is allowed.
	if (!gridworld.isStateAllowed(new_pos))
		return false;

	// Move player.
	gridworld.moveAgentToPosition(new_pos);
	return true;
}


float GridworldValueIteration::computeQValueFromValues(mic::types::Position2D pos_, mic::types::NESWAction ac_){
	//  Compute the Q-value of action in state from the value function stored table.
	mic::types::Position2D new_pos = pos_ + ac_;
	float q_value = (1-move_noise)*(step_reward + discount_rate * state_value_table((size_t)new_pos.y, (size_t)new_pos.x));
	float probs_normalizer = (1-move_noise);

	// Consider also east and west actions as possible actions - due to move_noise.
	if ((ac_.getType() == types::NESW::North) || (ac_.getType() == types::NESW::South)) {
		if (gridworld.isActionAllowed(pos_, A_EAST)) {
			mic::types::Position2D east_pos = pos_ + A_EAST;
			if (state_value_table((size_t)east_pos.y, (size_t)east_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_rate * state_value_table( (size_t)east_pos.y, (size_t)east_pos.x));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
		if (gridworld.isActionAllowed(pos_, A_WEST)) {
			mic::types::Position2D west_pos = pos_ + A_WEST;
			if (state_value_table((size_t)west_pos.y, (size_t)west_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_rate * state_value_table((size_t)west_pos.y, (size_t)west_pos.x));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
	}//: if

	// Consider also north and south actions as possible actions - due to move_noise.
	if ((ac_.getType() == types::NESW::East) || (ac_.getType() == types::NESW::West)) {
		if (gridworld.isActionAllowed(pos_, A_NORTH)) {
			mic::types::Position2D north_pos = pos_ + A_NORTH;
			if (state_value_table((size_t)north_pos.y, (size_t)north_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_rate * state_value_table((size_t)north_pos.y, (size_t)north_pos.x));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
		if (gridworld.isActionAllowed(pos_, A_SOUTH)) {
			mic::types::Position2D south_pos = pos_ + A_SOUTH;
			if (state_value_table((size_t)south_pos.y, (size_t)south_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_rate * state_value_table((size_t)south_pos.y, (size_t)south_pos.x));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
	}//: if

	// Normalize the probabilities.
	q_value /= probs_normalizer;

	return q_value;
}

float GridworldValueIteration::computeBestValue(mic::types::Position2D pos_){
	float best_value = -std::numeric_limits<float>::infinity();
	// Check if the state is allowed.
	if (!gridworld.isStateAllowed(pos_))
		return best_value;

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the actions one by one.
	for(mic::types::NESWAction action : actions) {
		if(gridworld.isActionAllowed(pos_, action)) {
			float value = computeQValueFromValues(pos_, action);
			if (value > best_value)
				best_value = value;
		}//if is allowed
	}//: for

	return best_value;
}


bool GridworldValueIteration::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	// Perform the iterative policy iteration.
	mic::types::MatrixXf new_state_value_table(height, width);
	new_state_value_table.setValue( -std::numeric_limits<float>::infinity() );

	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			mic::types::Position2D pos(x,y);
			if (gridworld.isStateTerminal(pos) ) {
				// Set the state rewared.
				new_state_value_table((size_t)pos.y, (size_t)pos.x) = gridworld.getStateReward(pos);
				continue;
			}//: if
			// Else - compute the best value.
			if (gridworld.isStateAllowed(pos) )
				new_state_value_table((size_t)pos.y, (size_t)pos.x) = computeBestValue(pos);
		}//: for x
	}//: for y

	// Compute delta.
	mic::types::MatrixXf delta_value;
	float curr_delta = 0;
	for (size_t i =0; i < (size_t) width*height; i++){
		float tmp_delta = 0;
		if (std::isfinite(new_state_value_table(i)))
			tmp_delta += new_state_value_table(i);
		if (std::isfinite(state_value_table(i)))
			tmp_delta -= state_value_table(i);
		curr_delta += std::abs(tmp_delta);
	}//: for
	running_delta = curr_delta;

	// Update state.
	state_value_table = new_state_value_table;

	LOG(LSTATUS) << std::endl << gridworld.toString();
	LOG(LSTATUS) << std::endl << streamStateActionTable();
	LOG(LINFO) << "Delta Value = " << running_delta;

	if (running_delta < 1e-05)
		return false;

	return true;
}



} /* namespace application */
} /* namespace mic */
