/*!
 * \file SimpleGridworld.cpp
 * \brief 
 * \author tkornut
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


GridworldValueIteration::GridworldValueIteration(std::string node_name_) : OpenGLApplication(node_name_),
		gridworld_type("gridworld_type", 0),
		width("width", 4),
		height("height", 4),
		step_reward("step_reward", 0.0),
		discount_factor("discount_factor", 0.9),
		move_noise("move_noise",0.2),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(gridworld_type);
	registerProperty(width);
	registerProperty(step_reward);
	registerProperty(discount_factor);
	registerProperty(move_noise);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


GridworldValueIteration::~GridworldValueIteration() {

}


void GridworldValueIteration::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
/*	collector_ptr->createContainer("average_reward", 0, 10, mic::types::color_rgba(255, 0, 0, 180));
	collector_ptr->createContainer("correct_arms_percentage", 0, 100, mic::types::color_rgba(0, 255, 0, 180));
	collector_ptr->createContainer("best_possible_reward", 0, 10, mic::types::color_rgba(0, 0, 255, 180));*/

	// Create the visualization windows - must be created in the same, main thread :]
	w_chart = new WindowFloatCollectorChart("nBandits", 256, 256, 0, 0);
	w_chart->setDataCollectorPtr(collector_ptr);

}

void GridworldValueIteration::initializePropertyDependentVariables() {
	// Initialize gridworld.
	switch(gridworld_type) {
		case 0 : gridworld.initExemplaryGrid(); break;
		case 1 : gridworld.initClassicCliffGrid(); break;
		case 2 : gridworld.initDiscountGrid(); break;
		case 3 : gridworld.initBridgeGrid(); break;
		case 4 : gridworld.initBookGrid(); break;
		case 5 : gridworld.initMazeGrid(); break;
		case -1:
		default: gridworld.initRandomGrid(width, height);
	}//: switch

	LOG(LSTATUS) << std::endl << gridworld.streamGrid();
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
/*			os << state_action_table({x,y,0}) << " , ";
			os << state_action_table({x,y,1}) << " , ";
			os << state_action_table({x,y,2}) << " , ";
			os << state_action_table({x,y,2}) << " | ";*/
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
    mic::types::Position2D new_pos = gridworld.getPlayerPosition() + ac_;

	// Check whether the state is allowed.
	if (!gridworld.isStateAllowed(new_pos))
		return false;

	// Update the "state-action" table.
	// ...

	// Move player.
	gridworld.movePlayerToPosition(new_pos);
	return true;
}


bool GridworldValueIteration::isActionAllowed(mic::types::Position2D pos_, mic::types::Action2DInterface ac_) {
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos_ + ac_;
    return gridworld.isStateAllowed(new_pos);
}

float GridworldValueIteration::computeQValueFromValues(mic::types::Position2D pos_, mic::types::NESWAction ac_){
	//  Compute the Q-value of action in state from the value function stored table.
	mic::types::Position2D new_pos = pos_ + ac_;
	float q_value = (1-move_noise)*(step_reward + discount_factor * state_value_table((size_t)new_pos.y, (size_t)new_pos.x));
	float probs_normalizer = (1-move_noise);

	// Consider also east and west actions as possible actions - due to move_noise.
	if ((ac_.getType() == NESW::North) || (ac_.getType() == NESW::South)) {
		if (isActionAllowed(pos_, A_EAST)) {
			mic::types::Position2D east_pos = pos_ + A_EAST;
			if (state_value_table((size_t)east_pos.y, (size_t)east_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table( (size_t)east_pos.y, (size_t)east_pos.x));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
		if (isActionAllowed(pos_, A_WEST)) {
			mic::types::Position2D west_pos = pos_ + A_WEST;
			if (state_value_table((size_t)west_pos.y, (size_t)west_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table((size_t)west_pos.y, (size_t)west_pos.x));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
	}//: if

	// Consider also north and south actions as possible actions - due to move_noise.
	if ((ac_.getType() == NESW::East) || (ac_.getType() == NESW::West)) {
		if (isActionAllowed(pos_, A_NORTH)) {
			mic::types::Position2D north_pos = pos_ + A_NORTH;
			if (state_value_table((size_t)north_pos.y, (size_t)north_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table((size_t)north_pos.y, (size_t)north_pos.x));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
		if (isActionAllowed(pos_, A_SOUTH)) {
			mic::types::Position2D south_pos = pos_ + A_SOUTH;
			if (state_value_table((size_t)south_pos.y, (size_t)south_pos.x) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table((size_t)south_pos.y, (size_t)south_pos.x));
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
	// Check the north action.
	if(isActionAllowed(pos_, A_NORTH)) {
		float value = computeQValueFromValues(pos_, A_NORTH);
		if (value > best_value)
			best_value = value;
	}//: if
	// Check the east action.
	if(isActionAllowed(pos_, A_EAST)) {
		float value = computeQValueFromValues(pos_, A_EAST);
		if (value > best_value)
			best_value = value;
	}//: if
	// Check the north action.
	if(isActionAllowed(pos_, A_SOUTH)) {
		float value = computeQValueFromValues(pos_, A_SOUTH);
		if (value > best_value)
			best_value = value;
	}//: if
	// Check the north action.
	if(isActionAllowed(pos_, A_WEST)) {
		float value = computeQValueFromValues(pos_, A_WEST);
		if (value > best_value)
			best_value = value;
	}//: if
	return best_value;
}


bool GridworldValueIteration::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";


/*	mic::types::Position2D player_position = getPlayerPosition();
//	LOG(LINFO) << "Player pose = " << player_position;
	LOG(LINFO) << "Reward: " << calculateReward();

	// Check episode state.
	short final_reward = isFinalPosition(player_position);
	if(final_reward != 0) {
		// Do recalculate state-value table.
		// In the terminal state all actions have the same "terminal" value.
		state_value_table({ (size_t)player_position.x, (size_t)player_position.y }) = final_reward;

		// Start new episode.
		LOG(LWARNING)<< "Starting new episode";
		movePlayerToPosition(initial_position);
	}

	// Move randomly - repeat until an allowed move is made.
	while (!move(A_RANDOM))
		;*/

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

	LOG(LSTATUS) << std::endl << gridworld.streamGrid();
	LOG(LSTATUS) << std::endl << streamStateActionTable();
	LOG(LSTATUS) << "Delta Value = " << running_delta;


/*	short choice;
	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > (double)epsilon){
		// Select best action.
		choice = selectBestArm();
	} else {
		//std::cout << "Random action!" << std::endl;
		// Random arm selection.
        choice = RAN_GEN->uniRandInt(0, number_of_bandits-1);
	}//: if*/

/*	// Add variables to container.
	collector_ptr->addDataToContainer("average_reward",running_mean_reward);
	collector_ptr->addDataToContainer("correct_arms_percentage",correct_arms_percentage);
	collector_ptr->addDataToContainer("best_possible_reward",10.0*best_arm_prob);

	// Export reward "convergence" diagram.
	collector_ptr->exportDataToCsv(statistics_filename);*/

	return true;
}



} /* namespace application */
} /* namespace mic */
