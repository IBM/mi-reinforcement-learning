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
}

Gridworld::~Gridworld() {
	// TODO Auto-generated destructor stub
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


void Gridworld::initClassicCliffGrid() {
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

void Gridworld::initDiscountGrid() {
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


void Gridworld::initBridgeGrid() {
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


void Gridworld::initBookGrid() {
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


void Gridworld::initRandomGrid(size_t width_, size_t height_) {
	// TODO!
	width = width_;
	height = height_;
	LOG(LFATAL) << "initRandomGrid() not implemented!";
	exit(1);
}

mic::types::Tensor<char> Gridworld::flattenGrid() {
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


float Gridworld::getStateReward(mic::types::Position2D pos_) {
    if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pit}) != 0)
    	// Pit.
        return gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pit});
    else if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goal}) != 0)
    	// Goal.
        return gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goal});
    else
        return 0;
}


bool Gridworld::isStateAllowed(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

	// Check walls!
	if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Wall}) != 0)
		return false;

	return true;
}


bool Gridworld::isStateTerminal(mic::types::Position2D pos_) {
    if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pit}) != 0)
    	// Pit.
        return true;
    else if (gridworld({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goal}) != 0)
    	// Goal.
        return true;
    else
        return false;
}


} /* namespace types */
} /* namespace mic */
