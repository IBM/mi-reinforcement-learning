/*!
 * \file Gridworld.cpp
 * \brief 
 * \author tkornut
 * \date Mar 21, 2016
 */

#include <types/Gridworld.hpp>

namespace mic {
namespace types {

Gridworld::Gridworld() {
	width = height = 0;
	channels = (size_t)GridworldChannels::Count;
}

Gridworld::~Gridworld() {
	// TODO Auto-generated destructor stub
}

mic::types::Gridworld & Gridworld::operator= (const mic::types::Gridworld & gw_) {
	width = gw_.width;
	height = gw_.height;
	channels = gw_.channels;
	initial_position = gw_.initial_position;
	gridworld = gw_.gridworld;

	return *this;
}


// Initialize gridworld.
void Gridworld::generateGridworld(int gridworld_type_, size_t width_, size_t height_) {
	switch(gridworld_type_) {
		case 0 : initExemplaryGrid(); break;
		case 1 : initClassicCliffGrid(); break;
		case 2 : initDiscountGrid(); break;
		case 3 : initBridgeGrid(); break;
		case 4 : initBookGrid(); break;
		case 5 : initMazeGrid(); break;
		case 6 : initExemplaryDQLGrid(); break;
		case 7 : initModifiedDQLGrid(); break;
		case 8 : initDebug2x2Grid(); break;
		case 9 : initDebug3x3Grid(); break;
		case -2: initDifficultRandomGrid(width_, height_); break;
		case -1:
		default: initSimpleRandomGrid(width_, height_);
	}//: switch
}


void Gridworld::initExemplaryGrid() {
	LOG(LINFO) << "Generating exemplary gridworld";
	// [[' ',' ',' ',' '],
	//  ['S',-10,' ',' '],
	//  [' ','','#',' '],
	//  [' ',' ',' ',10]]

	// Overwrite dimensions.
	width = 4;
	height = 4;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,1);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({2,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	gridworld({1,1, (size_t)GridworldChannels::Rewards}) = -10;

	// Place goal(s).
	gridworld({3,3, (size_t)GridworldChannels::Rewards}) = 10;
}


void Gridworld::initClassicCliffGrid() {
	LOG(LINFO) << "Generating classic cliff gridworld";
	// [[' ',' ',' ',' ',' '],
	//  ['S',' ',' ',' ',10],
	//  [-100,-100, -100, -100, -100]]

	// Overwrite dimensions.
	width = 5;
	height = 3;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,1);
	movePlayerToPosition(initial_position);

	// Place pit(s).
	for(size_t x=0; x<width; x++)
		gridworld({x,2, (size_t)GridworldChannels::Rewards}) = -100;

	// Place goal(s).
	gridworld({4,1, (size_t)GridworldChannels::Rewards}) = 10;
}

void Gridworld::initDiscountGrid() {
	LOG(LINFO) << "Generating classic discount gridworld";
	// [[' ',' ',' ',' ',' '],
	//  [' ','#',' ',' ',' '],
	//  [' ','#', 1,'#', 10],
	//  ['S',' ',' ',' ',' '],
	//  [-10,-10, -10, -10, -10]]

	// Overwrite dimensions.
	width = 5;
	height = 5;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,3);
	movePlayerToPosition(initial_position);

	// Place pits.
	for(size_t x=0; x<width; x++)
		gridworld({x,4, (size_t)GridworldChannels::Rewards}) = -10;

	// Place wall(s).
	gridworld({1,1, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({1,2, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({3,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place goal(s).
	gridworld({2,2, (size_t)GridworldChannels::Rewards}) = 1;
	gridworld({4,2, (size_t)GridworldChannels::Rewards}) = 10;
}


void Gridworld::initBridgeGrid() {
	LOG(LINFO) << "Generating classic bridge gridworld";
	// [[ '#',-100, -100, -100, -100, -100, '#'],
	//  [   1, 'S',  ' ',  ' ',  ' ',  ' ',  10],
	//  [ '#',-100, -100, -100, -100, -100, '#']]

	// Overwrite dimensions.
	width = 7;
	height = 3;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(1,1);
	movePlayerToPosition(initial_position);

	// Place pits.
	for(size_t x=1; x<width-1; x++) {
		gridworld({x,0, (size_t)GridworldChannels::Rewards}) = -100;
		gridworld({x,2, (size_t)GridworldChannels::Rewards}) = -100;
	}//: for

	// Place wall(s).
	gridworld({0,0, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({0,2, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({6,0, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({6,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place goal(s).
	gridworld({0,1, (size_t)GridworldChannels::Rewards}) = 1;
	gridworld({6,1, (size_t)GridworldChannels::Rewards}) = 10;
}


void Gridworld::initBookGrid() {
	LOG(LINFO) << "Generating classic book gridworld!!";
	// [[' ',' ',' ',+1],
	//  [' ','#',' ',-1],
	//  ['S',' ',' ',' ']]

	// Overwrite dimensions.
	width = 4;
	height = 3;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,2);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({1,1, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	gridworld({3,1, (size_t)GridworldChannels::Rewards}) = -1;

	// Place goal(s).
	gridworld({3,0, (size_t)GridworldChannels::Rewards}) = 1;
}


void Gridworld::initMazeGrid() {
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
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,4);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({0,1, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({1,1, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({1,2, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({1,3, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({2,3, (size_t)GridworldChannels::Walls}) = 1;
	gridworld({3,1, (size_t)GridworldChannels::Walls}) = 1;

	// Place goal(s).
	gridworld({3,0, (size_t)GridworldChannels::Rewards}) = 1;
}


void Gridworld::initExemplaryDQLGrid() {
	LOG(LINFO) << "Generating gridworld from Deep Q-Learning example";
	/*
	 * [[' ',' ',' ',' '],
	 *  [' ',' ',+10,' '],
	 *  [' ','#',-10,' '],
	 *  ['S',' ',' ',' ']]
	 */

	// Overwrite dimensions.
	width = 4;
	height = 4;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,3);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({1,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	gridworld({2,2, (size_t)GridworldChannels::Rewards}) = -10;

	// Place goal(s).
	gridworld({2,1, (size_t)GridworldChannels::Rewards}) = 10;
}

void Gridworld::initModifiedDQLGrid() {
	LOG(LINFO) << "Generating a slightly modified grid from Deep Q-Learning example";
	/*
	 * [[' ',' ',' ',' '],
	 *  [' ','#',+10,' '],
	 *  [' ',' ',-10,' '],
	 *  ['S',' ',' ',' ']]
	 */

	// Overwrite dimensions.
	width = 4;
	height = 4;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,3);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({1,1, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	gridworld({2,2, (size_t)GridworldChannels::Rewards}) = -10;

	// Place goal(s).
	gridworld({2,1, (size_t)GridworldChannels::Rewards}) = 10;
}


void Gridworld::initDebug2x2Grid() {
	LOG(LINFO) << "Generating the 2x2 debug grid";
	/*
	 * [['S',-10],
	 *  [+10,' ']]
	 */

	// Overwrite dimensions.
	width = 2;
	height = 2;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(0,0);
	movePlayerToPosition(initial_position);

	// Place pit(s).
	gridworld({1,0, (size_t)GridworldChannels::Rewards}) = -10;

	// Place goal(s).
	gridworld({0,1, (size_t)GridworldChannels::Rewards}) = 10;
}


/*!
 * 	Method initializes the 3x3 grid useful during the debugging.
 *
 */
void Gridworld::initDebug3x3Grid() {
	LOG(LINFO) << "Generating the 3x3 debug grid";
	/*
	 * [[' ',-10,' '],
	 *  [-10,'S',-10],
	 *  [' ',+10,' ']]
	 */

	// Overwrite the dimensions.
	width = 3;
	height = 3;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	initial_position.set(1,1);
	movePlayerToPosition(initial_position);

	// Place wall(s).
	gridworld({1,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	gridworld({0,1, (size_t)GridworldChannels::Rewards}) = -10;
	gridworld({1,0, (size_t)GridworldChannels::Rewards}) = -10;
	gridworld({2,1, (size_t)GridworldChannels::Rewards}) = -10;

	// Place goal(s).
	gridworld({1,2, (size_t)GridworldChannels::Rewards}) = 10;

}


void Gridworld::initSimpleRandomGrid(size_t width_, size_t height_) {
	LOG(LINFO) << "Generating simple " << width_ << "x" << height_<< " random grid";
	// Overwrite the dimensions.
	width = width_;
	height = height_;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	mic::types::Position2D player(0, width-1, 0, height-1);
	initial_position = player;
	movePlayerToPosition(initial_position);

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());

	// Place wall.
	while (1){
		// Random position.
		mic::types::Position2D wall(0, width-1, 0, height-1);

		// Validate pose.
		if (gridworld({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Player}) != 0)
			continue;

		// Add wall...
		gridworld({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) = 1;
		break;
	}

	// Place pit.
	while(1){
		// Random position.
		mic::types::Position2D pit(0, width-1, 0, height-1);

		// Validate pose.
		if (gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Player}) != 0)
			continue;
		if (gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Walls}) != 0)
			continue;

		// Add pit...
		gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Rewards}) = -10;

		break;
	}//: while


	// Place goal.
	while(1) {
		// Random position.
		mic::types::Position2D goal(0, width-1, 0, height-1);

		// Validate pose.
		if (gridworld({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Player}) != 0)
			continue;
		if (gridworld({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Walls}) != 0)
			continue;
		if (gridworld({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Rewards}) != 0)
			continue;

		// ... but additionally check the goal surroundings - there must be at least one way out, and not going through the pit!
		bool reachable = false;
		for (size_t a=0; a<4; a++){
			NESWAction action(a);
			mic::types::Position2D way_to_goal = goal + action;
			if ((isStateAllowed(way_to_goal)) &&
					(gridworld({(size_t)way_to_goal.x, (size_t)way_to_goal.y, (size_t)GridworldChannels::Rewards}) >= 0)) {
				reachable = true;
				break;
			}//: if
		}//: for
		if (!reachable)
			continue;

		// Ok, add the goal.
		gridworld({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Rewards}) = 10;
		break;
	}//: while

}

bool Gridworld::isGridTraversible(long x_, long y_, mic::types::Matrix<bool> & visited_) {
	// If not allowed...
	if (!isStateAllowed(x_, y_))
		return false;
	// .. or is a pit...
	if (gridworld({(size_t)x_, (size_t)y_, (size_t)GridworldChannels::Rewards}) < 0)
		return false;
	// ... or wasa already visited.
	if (visited_(y_,x_))
		return false;
	// Ok found the goal!
	if (gridworld({(size_t)x_, (size_t)y_, (size_t)GridworldChannels::Rewards}) > 0)
		return true;
	// Ok, new state.
	visited_(y_,x_) = true;

	// Recursive check NESW.
	if (isGridTraversible(x_, y_-1, visited_))
		return true;
	if (isGridTraversible(x_+1, y_, visited_))
		return true;
	if (isGridTraversible(x_, y_+1, visited_))
		return true;
	if (isGridTraversible(x_-1, y_, visited_))
		return true;
	// Sorry, no luck in her.
	return false;
}



void Gridworld::initDifficultRandomGrid(size_t width_, size_t height_) {
	LOG(LINFO) << "Generating difficult " << width_ << "x" << height_<< " random grid";
	// Overwrite the dimensions.
	width = width_;
	height = height_;

	// Set gridworld size.
	gridworld.resize({width, height, channels});
	gridworld.zeros();

	// Place the player.
	mic::types::Position2D player(0, width-1, 0, height-1);
	initial_position = player;
	movePlayerToPosition(initial_position);

	// Place goal.
	while(1) {
		// Random position.
		mic::types::Position2D goal(0, width-1, 0, height-1);

		// Validate pose.
		if (gridworld({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Player}) != 0)
			continue;

		// Ok, add the goal.
		gridworld({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Rewards}) = 10;
		break;
	}//: while

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());


	// Initialize uniform integer distribution.
	size_t max_obstacles = sqrt(width*height) - 2;
	std::uniform_int_distribution<size_t> obstacle_dist(0, max_obstacles);

	// Calculate number of walls.
	size_t number_of_walls = obstacle_dist(rng_mt19937_64);

	// Matrix informing us thwther we already visited the state or not.
	mic::types::Matrix<bool> visited (height, width);

	// Place wall(s).
	for (size_t i=0; i<number_of_walls; i++) {
		while (1){
			// Random position.
			mic::types::Position2D wall(0, width-1, 0, height-1);

			// Validate pose.
			if (gridworld({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Player}) != 0)
				continue;
			if (gridworld({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Rewards}) != 0)
				continue;
			if (gridworld({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) != 0)
				continue;

			// Add wall...
			gridworld({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) = 1;

			// ... but additionally whether the path from agent to the goal is traversable!
			visited.setZero();
			if (!isGridTraversible(player.x, player.y, visited)) {
				// Sorry, we must remove this wall...
				gridworld({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) = 0;
				// .. and try once again.
				continue;
			}//: if

			break;
		}//: while
	}//: for number of walls


	// Calculate number of pits.
	size_t number_of_pits = obstacle_dist(rng_mt19937_64);

	// Place pit(s).
	for (size_t i=0; i<number_of_pits; i++) {
		while(1){
			// Random position.
			mic::types::Position2D pit(0, width-1, 0, height-1);

			// Validate pose.
			if (gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Player}) != 0)
				continue;
			if (gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Rewards}) != 0)
				continue;
			if (gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Walls}) != 0)
				continue;

			// Add pit...
			gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Rewards}) = -10;

			// ... but additionally whether the path from agent to the goal is traversable!
			visited.setZero();
			if (!isGridTraversible(player.x, player.y, visited)) {
				// Sorry, we must remove this pit...
				gridworld({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Rewards}) = 0;
				// .. and try once again.
				continue;
			}//: if

			break;
		}//: while
	}//: for number of walls


}


mic::types::Tensor<char> Gridworld::flattenGrid() {
	mic::types::Tensor<char> grid;
	grid.resize({width, height});
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			// Check object occupancy.
			if (gridworld({x,y, (size_t)GridworldChannels::Player}) != 0) {
				// Display agent.
				grid({x,y}) = 'P';
			} else if (gridworld({x,y, (size_t)GridworldChannels::Walls}) != 0) {
				// Display wall.
				grid({x,y}) = '#';
			} else if (gridworld({x,y, (size_t)GridworldChannels::Rewards}) < 0) {
				// Display pit.
				grid({x,y}) = '-';
			} else if (gridworld({x,y, (size_t)GridworldChannels::Rewards}) > 0) {
				// Display goal.
				grid({x,y}) = '+';
			} else
				grid({x,y}) = ' ';
		}//: for x
	}//: for y
	return grid;
}


std::string Gridworld::streamGrid() {
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


mic::types::MatrixXfPtr Gridworld::encodePlayerGrid() {

	// DEBUG - copy only player pose data, avoid goals etc.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(height, width));
	encoded_grid->setZero();

	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			// Check object occupancy.
			if (gridworld({x,y, (size_t)GridworldChannels::Player}) != 0) {
				// Set one.
				(*encoded_grid)(y,x) = 1;
				break;
			}
		}//: for x
	}//: for y
	encoded_grid->resize(height*width, 1);

	// Return the matrix pointer.
	return encoded_grid;
}


mic::types::MatrixXfPtr Gridworld::encodeWholeGrid() {
	// Temporarily reshape the gridworld.
	gridworld.conservativeResize({1, width * height * channels});
	// Create a matrix pointer and copy data from grid into the matrix.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(gridworld));
	// Back to the original shape.
	gridworld.resize({width, height, channels});

	// Return the matrix pointer.
	return encoded_grid;
}





mic::types::Position2D Gridworld::getPlayerPosition() {
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

void Gridworld::movePlayerToPosition(mic::types::Position2D pos_) {
	// Clear old.
	mic::types::Position2D old = getPlayerPosition();
	gridworld({(size_t)old.x, (size_t)old.y, (size_t)GridworldChannels::Player}) = 0;
	// Set new.
	gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Player}) = 1;
}

void Gridworld::movePlayerToInitialPosition() {
	movePlayerToPosition(initial_position);
}


float Gridworld::getStateReward(mic::types::Position2D pos_) {
    if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Rewards}) != 0)
    	// Pit or Goal.
        return gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Rewards});
    else
        return 0;
}


bool Gridworld::isStateAllowed(long x_, long y_) {
	if ((x_ < 0) || (x_ >= width))
		return false;

	if ((y_ < 0) || (y_ >= height))
		return false;

	// Check walls!
	if (gridworld({(size_t)x_, (size_t)y_, (size_t)GridworldChannels::Walls}) != 0)
		return false;

	return true;
}


bool Gridworld::isStateAllowed(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

	// Check walls!
	if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Walls}) != 0)
		return false;

	return true;
}


bool Gridworld::isStateTerminal(long x_, long y_) {
	if ((x_ < 0) || (x_ >= width))
		return false;

	if ((y_ < 0) || (y_ >= height))
		return false;

    if (gridworld({(size_t)x_, (size_t)y_, (size_t)GridworldChannels::Rewards}) != 0)
    	// Reward - goal or pit.
        return true;
    else
        return false;
}

bool Gridworld::isStateTerminal(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

    if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Rewards}) != 0)
    	// Reward - goal or pit.
        return true;
    else
        return false;
}

bool Gridworld::isActionAllowed(long x_, long y_, size_t action_) {
    mic::types::Position2D pos(x_,y_);
    mic::types::NESWAction ac(action_);
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos + ac;
    return isStateAllowed(new_pos);
}


bool Gridworld::isActionAllowed(mic::types::Position2D pos_, mic::types::Action2DInterface ac_) {
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos_ + ac_;
    return isStateAllowed(new_pos);
}

bool Gridworld::isActionAllowed(mic::types::Action2DInterface ac_) {
	// Get current player position.
	mic::types::Position2D pos = getPlayerPosition();
	// Compute the "destination" coordinates.
    mic::types::Position2D new_pos = pos + ac_;
    return isStateAllowed(new_pos);
}

} /* namespace types */
} /* namespace mic */
