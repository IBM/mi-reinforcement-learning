/*!
 * \file SimpleGridworld.cpp
 * \brief 
 * \author tkornut
 * \date Mar 17, 2016
 */

#include <application/SimpleGridworld.hpp>

#include  <data_utils/RandomGenerator.hpp>

#include <limits>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::SimpleGridworld);
}


SimpleGridworld::SimpleGridworld(std::string node_name_) : OpenGLApplication(node_name_),
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


SimpleGridworld::~SimpleGridworld() {

}


void SimpleGridworld::initialize(int argc, char* argv[]) {
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

void SimpleGridworld::initializePropertyDependentVariables() {
	// Initialize gridworld.
	switch(gridworld_type) {
		case 0 : initExemplaryGrid(); break;
		case 1 : initClassicCliffGrid(); break;
		case 2 : initDiscountGrid(); break;
		case 3 : initBridgeGrid(); break;
		case 4 : initBookGrid(); break;
		case 5 : initMazeGrid(); break;
		case -1:
		default: initRandomGrid();
	}//: switch

	LOG(LSTATUS) << std::endl << streamGrid();

	// Resize and reset the action-value table.
	state_value_table.resize({width,height});
	//state_value_table.zeros();
	state_value_table.setValue( -std::numeric_limits<float>::infinity() );

	LOG(LSTATUS) << std::endl << streamStateActionTable();
}


void SimpleGridworld::initExemplaryGrid() {
	LOG(LINFO) << "Generating exemplary gridworld";
	// [[' ',' ',' ',' '],
	//  ['S',-10,' ',' '],
	//  [' ','','#',' '],
	//  [' ',' ',' ',10]]

	// Set gridworld size.
	gridworld.resize({width, height, 4});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,1);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({2,2, (size_t)GridworldChannels::Wall}) = 1;

	// Place pit(s).
	gridworld({1,1, (size_t)GridworldChannels::Pit}) = -10;

	// Place goal(s).
	gridworld({3,3, (size_t)GridworldChannels::Goal}) = 10;
}


void SimpleGridworld::initClassicCliffGrid() {
	LOG(LINFO) << "Generating classic cliff gridworld";
	// [[' ',' ',' ',' ',' '],
	//  ['S',' ',' ',' ',10],
	//  [-100,-100, -100, -100, -100]]

	// Overwrite dimensions.
	width = 5;
	height = 3;

	// Set gridworld size.
	gridworld.resize({width, height, 4});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,1);
	movePlayerToPosition(initial_position);

	// Place pit(s).
	for(size_t x=0; x<width; x++)
		gridworld({x,2, (size_t)GridworldChannels::Pit}) = -100;

	// Place goal(s).
	gridworld({4,1, (size_t)GridworldChannels::Goal}) = 10;
}

void SimpleGridworld::initDiscountGrid() {
	LOG(LINFO) << "Generating classic discount gridworld";
	// [[' ',' ',' ',' ',' '],
	//  [' ','#',' ',' ',' '],
	//  [' ','#', 1,'#', 10],
	//   ['S',' ',' ',' ',' '],
	//   [-10,-10, -10, -10, -10]]

	// Overwrite dimensions.
	width = 5;
	height = 5;

	// Set gridworld size.
	gridworld.resize({width, height, 4});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,3);
	movePlayerToPosition(initial_position);

	// Place pits.
	for(size_t x=0; x<width; x++)
		gridworld({x,4, (size_t)GridworldChannels::Pit}) = -10;

	// Place wall(s).
	gridworld({1,1, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({1,2, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({3,2, (size_t)GridworldChannels::Wall}) = 1;

	// Place goal(s).
	gridworld({2,2, (size_t)GridworldChannels::Goal}) = 1;
	gridworld({4,2, (size_t)GridworldChannels::Goal}) = 10;
}


void SimpleGridworld::initBridgeGrid() {
	LOG(LINFO) << "Generating classic bridge gridworld";
	// [[ '#',-100, -100, -100, -100, -100, '#'],
	//  [   1, 'S',  ' ',  ' ',  ' ',  ' ',  10],
	//  [ '#',-100, -100, -100, -100, -100, '#']]

	// Overwrite dimensions.
	width = 7;
	height = 3;

	// Set gridworld size.
	gridworld.resize({width, height, 4});
	gridworld.zeros();

	// Place the player.
	initial_position.set(1,1);
	movePlayerToPosition(initial_position);

	// Place pits.
	for(size_t x=1; x<width-1; x++) {
		gridworld({x,0, (size_t)GridworldChannels::Pit}) = -100;
		gridworld({x,2, (size_t)GridworldChannels::Pit}) = -100;
	}//: for

	// Place wall(s).
	gridworld({0,0, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({0,2, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({6,0, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({6,2, (size_t)GridworldChannels::Wall}) = 1;

	// Place goal(s).
	gridworld({0,1, (size_t)GridworldChannels::Goal}) = 1;
	gridworld({6,1, (size_t)GridworldChannels::Goal}) = 10;
}


void SimpleGridworld::initBookGrid() {
	LOG(LINFO) << "Generating classic book gridworld!!";
	// [[' ',' ',' ',+1],
	//  [' ','#',' ',-1],
	//  ['S',' ',' ',' ']]

	// Overwrite dimensions.
	width = 4;
	height = 3;

	// Set gridworld size.
	gridworld.resize({width, height, 4});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,2);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({1,1, (size_t)GridworldChannels::Wall}) = 1;

	// Place pit(s).
	gridworld({3,1, (size_t)GridworldChannels::Pit}) = -1;

	// Place goal(s).
	gridworld({3,0, (size_t)GridworldChannels::Goal}) = 1;
}


void SimpleGridworld::initMazeGrid() {
	LOG(LINFO) << "Generating classic maze gridworld";
	// [[' ',' ',' ',+1],
	//  ['#','#',' ','#'],
	//  [' ','#',' ',' '],
	//  [' ','#','#',' '],
	//  ['S',' ',' ',' ']]

	// Overwrite dimensions.
	width = 4;
	height = 5;

	// Set gridworld size.
	gridworld.resize({width, height, 4});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,4);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({0,1, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({1,1, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({1,2, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({1,3, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({2,3, (size_t)GridworldChannels::Wall}) = 1;
	gridworld({3,1, (size_t)GridworldChannels::Wall}) = 1;

	// Place goal(s).
	gridworld({3,0, (size_t)GridworldChannels::Goal}) = 1;
}


void SimpleGridworld::initRandomGrid() {
	// TODO!
	LOG(LFATAL) << "initRandomGrid() not implemented!";
	exit(1);
}

mic::types::Tensor<char> SimpleGridworld::flattenGrid() {
	mic::types::Tensor<char> grid;
	grid.resize({width, height});
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			// Check object occupancy.
			if (gridworld({x,y, (size_t)GridworldChannels::Player}) != 0) {
				// Display player.
				grid({x,y}) = 'P';
			} else if (gridworld({x,y, (size_t)GridworldChannels::Wall}) != 0) {
				// Display wall.
				grid({x,y}) = '#';
			} else if (gridworld({x,y, (size_t)GridworldChannels::Pit}) != 0) {
				// Display pit.
				grid({x,y}) = '-';
			} else if (gridworld({x,y, (size_t)GridworldChannels::Goal}) != 0) {
				// Display goal.
				grid({x,y}) = '+';
			} else
				grid({x,y}) = ' ';
		}//: for x
	}//: for y
	return grid;
}


std::string SimpleGridworld::streamGrid() {
	mic::types::Tensor<char> grid = flattenGrid();
	std::ostringstream os;
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			os << grid({x,y}) << " , ";
		}//: for x
		os << std::endl;
	}//: for y
	return os.str();

}

std::string SimpleGridworld::streamStateActionTable() {
	std::ostringstream os;
	for (size_t y=0; y<height; y++){
		os << "| ";
		for (size_t x=0; x<width; x++) {
/*			os << state_action_table({x,y,0}) << " , ";
			os << state_action_table({x,y,1}) << " , ";
			os << state_action_table({x,y,2}) << " , ";
			os << state_action_table({x,y,2}) << " | ";*/
			if ( state_value_table({x,y}) == -std::numeric_limits<float>::infinity())
				os << "-INF | ";
			else
				os << state_value_table({x,y}) << " | ";
		}//: for x
		os << std::endl;
	}//: for y
	return os.str();

}


mic::types::Position2D SimpleGridworld::getPlayerPosition() {
	mic::types::Position2D position;
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			if (gridworld({x,y, (size_t)GridworldChannels::Player}) == 1) {
				position.x = x;
				position.y = y;
				return position;
			}// if
		}//: for x
	}//: for y
	// Remove warnings...
	return position;
}

void SimpleGridworld::movePlayerToPosition(mic::types::Position2D pos_) {
	// Clear old.
	mic::types::Position2D old = getPlayerPosition();
	gridworld({(size_t)old.x, (size_t)old.y, (size_t)GridworldChannels::Player}) = 0;
	// Set new.
	gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Player}) = 1;
}


float SimpleGridworld::getStateReward(mic::types::Position2D pos_) {
    if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pit}) != 0)
    	// Pit.
        return gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pit});
    else if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goal}) != 0)
    	// Goal.
        return gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goal});
    else
        return 0;
}


bool SimpleGridworld::isStateAllowed(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

	// Check walls!
	if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Wall}) != 0)
		return false;

	return true;
}


bool SimpleGridworld::isStateTerminal(mic::types::Position2D pos_) {
    if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pit}) != 0)
    	// Pit.
        return true;
    else if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goal}) != 0)
    	// Goal.
        return true;
    else
        return false;
}


bool SimpleGridworld::move (mic::types::Action2DInterface ac_) {
//	LOG(LINFO) << "Current move = " << ac_;
	// Get player position.
	mic::types::Position2D old_pos = getPlayerPosition();
	mic::types::Position2D new_pos = old_pos + ac_;

	// Check whether the "destination" (new position) is valid.
	if (!isStateAllowed(new_pos))
		return false;

	// Update the "state-action" table.


	// "Reset" previous player position and set the new one.
	gridworld({(size_t)old_pos.x, (size_t)old_pos.y, (size_t)GridworldChannels::Player}) = 0;
	gridworld({(size_t)new_pos.x, (size_t)new_pos.y, (size_t)GridworldChannels::Player}) = 1;
	return true;
}


bool SimpleGridworld::isActionAllowed(mic::types::Position2D pos_, mic::types::Action2DInterface ac_) {
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos_ + ac_;
    return isStateAllowed(new_pos);
}

float SimpleGridworld::computeQValueFromValues(mic::types::Position2D pos_, mic::types::NESWAction ac_){
	//  Compute the Q-value of action in state from the value function stored table.
	mic::types::Position2D new_pos = pos_ + ac_;
	float q_value = (1-move_noise)*(step_reward + discount_factor * state_value_table({(size_t)new_pos.x, (size_t)new_pos.y}));
	float probs_normalizer = (1-move_noise);

	// Consider also east and west actions as possible actions - due to move_noise.
	if ((ac_.getType() == NESW::North) || (ac_.getType() == NESW::South)) {
		if (isActionAllowed(pos_, A_EAST)) {
			mic::types::Position2D east_pos = pos_ + A_EAST;
			if (state_value_table({(size_t)east_pos.x, (size_t)east_pos.y}) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table({(size_t)east_pos.x, (size_t)east_pos.y}));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
		if (isActionAllowed(pos_, A_WEST)) {
			mic::types::Position2D west_pos = pos_ + A_WEST;
			if (state_value_table({(size_t)west_pos.x, (size_t)west_pos.y}) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table({(size_t)west_pos.x, (size_t)west_pos.y}));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
	}//: if

	// Consider also north and south actions as possible actions - due to move_noise.
	if ((ac_.getType() == NESW::East) || (ac_.getType() == NESW::West)) {
		if (isActionAllowed(pos_, A_NORTH)) {
			mic::types::Position2D north_pos = pos_ + A_NORTH;
			if (state_value_table({(size_t)north_pos.x, (size_t)north_pos.y}) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table({(size_t)north_pos.x, (size_t)north_pos.y}));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
		if (isActionAllowed(pos_, A_SOUTH)) {
			mic::types::Position2D south_pos = pos_ + A_SOUTH;
			if (state_value_table({(size_t)south_pos.x, (size_t)south_pos.y}) != -std::numeric_limits<float>::infinity()) {
				q_value += (move_noise/2)*(step_reward + discount_factor * state_value_table({(size_t)south_pos.x, (size_t)south_pos.y}));
				probs_normalizer += (move_noise/2);
			}//:if != -INF
		}//: if
	}//: if

	// Normalize the probabilities.
	q_value /= probs_normalizer;

	return q_value;
}

float SimpleGridworld::computeBestValue(mic::types::Position2D pos_){
	float best_value = -std::numeric_limits<float>::infinity();
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


bool SimpleGridworld::performSingleStep() {
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
	mic::types::TensorXf new_state_value_table({width, height});
	new_state_value_table.setValue( -std::numeric_limits<float>::infinity() );

	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			mic::types::Position2D pos(x,y);
			if (isStateTerminal(pos) ) {
				float final_reward = getStateReward(pos);
				new_state_value_table({ (size_t)pos.x, (size_t)pos.y }) = final_reward;
				continue;
			}//: if
			// Else - find th best value.
			if (isStateAllowed(pos) )
				new_state_value_table({ (size_t)pos.x, (size_t)pos.y }) = computeBestValue(pos);
		}//: for x
	}//: for y
	// Update state.
	state_value_table = new_state_value_table;

	LOG(LSTATUS) << std::endl << streamGrid();
	LOG(LSTATUS) << std::endl << streamStateActionTable();


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
