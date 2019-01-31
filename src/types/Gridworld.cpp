/*!
 * Copyright (C) tkornuta, IBM Corporation 2015-2019
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file Gridworld.cpp
 * \brief 
 * \author tkornut
 * \date Mar 21, 2016
 */

#include <types/Gridworld.hpp>

namespace mic {
namespace environments {

Gridworld::Gridworld(std::string node_name_) : Environment(node_name_),
	type("type", 0)
{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(type);

	channels = (size_t)GridworldChannels::Count;

}

Gridworld::~Gridworld() {
	// TODO Auto-generated destructor stub
}

mic::environments::Gridworld & Gridworld::operator= (const mic::environments::Gridworld & gw_) {
	width = gw_.width;
	height = gw_.height;
	channels = gw_.channels;
	initial_position = gw_.initial_position;
	environment_grid = gw_.environment_grid;
	observation_grid = gw_.observation_grid;

	return *this;
}


void Gridworld::initializePropertyDependentVariables() {
	// Empty - everything will be initialized in environment initialization.
}

void Gridworld::initializeEnvironment() {
	// Generate adequate gridworld.
	switch(type) {
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
		case -2: initHardRandomGrid(); break;
		case -1:
		default: initSimpleRandomGrid();
	}//: switch

	// Check whether it is a POMDP or not.
	if (roi_size >0) {
		pomdp_flag = true;
		observation_grid->resize({roi_size, roi_size, channels});
	} else {
		observation_grid->resize({width, height, channels});
	}//: else

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
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,1);
	moveAgentToPosition(initial_position);

	// Place wall(s).
	(*environment_grid)({2,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	(*environment_grid)({1,1, (size_t)GridworldChannels::Pits}) = -10;

	// Place goal(s).
	(*environment_grid)({3,3, (size_t)GridworldChannels::Goals}) = 10;
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
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,1);
	moveAgentToPosition(initial_position);

	// Place pit(s).
	for(size_t x=0; x<width; x++)
		(*environment_grid)({x,2, (size_t)GridworldChannels::Pits}) = -100;

	// Place goal(s).
	(*environment_grid)({4,1, (size_t)GridworldChannels::Goals}) = 10;
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
	(*environment_grid).resize({width, height, channels});
	(*environment_grid).zeros();

	// Place the agent.
	initial_position.set(0,3);
	moveAgentToPosition(initial_position);

	// Place pits.
	for(size_t x=0; x<width; x++)
		(*environment_grid)({x,4, (size_t)GridworldChannels::Pits}) = -10;

	// Place wall(s).
	(*environment_grid)({1,1, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({1,2, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({3,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place goal(s).
	(*environment_grid)({2,2, (size_t)GridworldChannels::Goals}) = 1;
	(*environment_grid)({4,2, (size_t)GridworldChannels::Goals}) = 10;
}


void Gridworld::initBridgeGrid() {
	LOG(LINFO) << "Generating classic bridge gridworld";
	// [[ '#',-100, -100, -100, -100, -100, '#'],
	//  [   1, 'S',  ' ',  ' ',  ' ',  ' ',  10],
	//  [ '#',-100, -100, -100, -100, -100, '#']]

	// Overwrite dimensions.
	width = 7;
	height = 3;

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(1,1);
	moveAgentToPosition(initial_position);

	// Place pits.
	for(size_t x=1; x<width-1; x++) {
		(*environment_grid)({x,0, (size_t)GridworldChannels::Pits}) = -100;
		(*environment_grid)({x,2, (size_t)GridworldChannels::Pits}) = -100;
	}//: for

	// Place wall(s).
	(*environment_grid)({0,0, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({0,2, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({6,0, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({6,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place goal(s).
	(*environment_grid)({0,1, (size_t)GridworldChannels::Goals}) = 1;
	(*environment_grid)({6,1, (size_t)GridworldChannels::Goals}) = 10;
}


void Gridworld::initBookGrid() {
	LOG(LINFO) << "Generating classic book environment_grid!!";
	// [[' ',' ',' ',+1],
	//  [' ','#',' ',-1],
	//  ['S',' ',' ',' ']]

	// Overwrite dimensions.
	width = 4;
	height = 3;

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,2);
	moveAgentToPosition(initial_position);

	// Place wall(s).
	(*environment_grid)({1,1, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	(*environment_grid)({3,1, (size_t)GridworldChannels::Pits}) = -1;

	// Place goal(s).
	(*environment_grid)({3,0, (size_t)GridworldChannels::Goals}) = 1;
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

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,4);
	moveAgentToPosition(initial_position);

	// Place wall(s).
	(*environment_grid)({0,1, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({1,1, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({1,2, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({1,3, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({2,3, (size_t)GridworldChannels::Walls}) = 1;
	(*environment_grid)({3,1, (size_t)GridworldChannels::Walls}) = 1;

	// Place goal(s).
	(*environment_grid)({3,0, (size_t)GridworldChannels::Goals}) = 1;
}


void Gridworld::initExemplaryDQLGrid() {
	LOG(LINFO) << "Generating environment_grid from Deep Q-Learning example";
	/*
	 * [[' ',' ',' ',' '],
	 *  [' ',' ',+10,' '],
	 *  [' ','#',-10,' '],
	 *  ['S',' ',' ',' ']]
	 */

	// Overwrite dimensions.
	width = 4;
	height = 4;

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,3);
	moveAgentToPosition(initial_position);

	// Place wall(s).
	(*environment_grid)({1,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	(*environment_grid)({2,2, (size_t)GridworldChannels::Pits}) = -10;

	// Place goal(s).
	(*environment_grid)({2,1, (size_t)GridworldChannels::Goals}) = 10;
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

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,3);
	moveAgentToPosition(initial_position);

	// Place wall(s).
	(*environment_grid)({1,1, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	(*environment_grid)({2,2, (size_t)GridworldChannels::Pits}) = -10;

	// Place goal(s).
	(*environment_grid)({2,1, (size_t)GridworldChannels::Goals}) = 10;
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

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,0);
	moveAgentToPosition(initial_position);

	// Place pit(s).
	(*environment_grid)({1,0, (size_t)GridworldChannels::Pits}) = -10;

	// Place goal(s).
	(*environment_grid)({0,1, (size_t)GridworldChannels::Goals}) = 10;
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

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(1,1);
	moveAgentToPosition(initial_position);

	// Place wall(s).
	(*environment_grid)({1,2, (size_t)GridworldChannels::Walls}) = 1;

	// Place pit(s).
	(*environment_grid)({0,1, (size_t)GridworldChannels::Pits}) = -10;
	(*environment_grid)({1,0, (size_t)GridworldChannels::Pits}) = -10;
	(*environment_grid)({2,1, (size_t)GridworldChannels::Pits}) = -10;

	// Place goal(s).
	(*environment_grid)({1,2, (size_t)GridworldChannels::Goals}) = 10;

}


void Gridworld::initSimpleRandomGrid() {
	LOG(LINFO) << "Generating simple " << width << "x" << height<< " random grid";

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	mic::types::Position2D agent(0, width-1, 0, height-1);
	initial_position = agent;
	moveAgentToPosition(initial_position);

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());

	// Place wall.
	while (1){
		// Random position.
		mic::types::Position2D wall(0, width-1, 0, height-1);

		// Validate pose.
		if ((*environment_grid)({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Agent}) != 0)
			continue;

		// Add wall...
		(*environment_grid)({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) = 1;
		break;
	}

	// Place pit.
	while(1){
		// Random position.
		mic::types::Position2D pit(0, width-1, 0, height-1);

		// Validate pose.
		if ((*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Agent}) != 0)
			continue;
		if ((*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Walls}) != 0)
			continue;

		// Add pit...
		(*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Pits}) = -10;

		break;
	}//: while


	// Place goal.
	while(1) {
		// Random position.
		mic::types::Position2D goal(0, width-1, 0, height-1);

		// Validate pose.
		if ((*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Agent}) != 0)
			continue;
		if ((*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Walls}) != 0)
			continue;
		if ((*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Pits}) != 0)
			continue;

		// ... but additionally check the goal surroundings - there must be at least one way out, and not going through the pit!
		bool reachable = false;
		for (size_t a=0; a<4; a++){
			mic::types::NESWAction action(a);
			mic::types::Position2D way_to_goal = goal + action;
			if ((isStateAllowed(way_to_goal)) &&
					((*environment_grid)({(size_t)way_to_goal.x, (size_t)way_to_goal.y, (size_t)GridworldChannels::Pits}) == 0)) {
				reachable = true;
				break;
			}//: if
		}//: for
		if (!reachable)
			continue;

		// Ok, add the goal.
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Goals}) = 10;
		break;
	}//: while

}

bool Gridworld::isGridTraversible(long x_, long y_, mic::types::Matrix<bool> & visited_) {
	// If not allowed...
	if (!isStateAllowed(x_, y_))
		return false;
	// .. or is a pit...
	if ((*environment_grid)({(size_t)x_, (size_t)y_, (size_t)GridworldChannels::Pits}) < 0)
		return false;
	// ... or wasa already visited.
	if (visited_(y_,x_))
		return false;
	// Ok found the goal!
	if ((*environment_grid)({(size_t)x_, (size_t)y_, (size_t)GridworldChannels::Goals}) > 0)
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



void Gridworld::initHardRandomGrid() {
	LOG(LINFO) << "Generating hard " << width << "x" << height<< " random grid";

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	mic::types::Position2D agent(0, width-1, 0, height-1);
	initial_position = agent;
	moveAgentToPosition(initial_position);

	// Place goal.
	while(1) {
		// Random position.
		mic::types::Position2D goal(0, width-1, 0, height-1);

		// Validate pose.
		if ((*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Agent}) != 0)
			continue;

		// Ok, add the goal.
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)GridworldChannels::Goals}) = 10;
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
			if ((*environment_grid)({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Agent}) != 0)
				continue;
			if ((*environment_grid)({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Goals}) != 0)
				continue;
			if ((*environment_grid)({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) != 0)
				continue;

			// Add wall...
			(*environment_grid)({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) = 1;

			// ... but additionally whether the path from agent to the goal is traversable!
			visited.setZero();
			if (!isGridTraversible(agent.x, agent.y, visited)) {
				// Sorry, we must remove this wall...
				(*environment_grid)({(size_t)wall.x, (size_t)wall.y, (size_t)GridworldChannels::Walls}) = 0;
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
			if ((*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Agent}) != 0)
				continue;
			if ((*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Goals}) != 0)
				continue;
			if ((*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Pits}) != 0)
				continue;
			if ((*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Walls}) != 0)
				continue;

			// Add pit...
			(*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Pits}) = -10;

			// ... but additionally whether the path from agent to the goal is traversable!
			visited.setZero();
			if (!isGridTraversible(agent.x, agent.y, visited)) {
				// Sorry, we must remove this pit...
				(*environment_grid)({(size_t)pit.x, (size_t)pit.y, (size_t)GridworldChannels::Pits}) = 0;
				// .. and try once again.
				continue;
			}//: if

			break;
		}//: while
	}//: for number of walls


}


std::string Gridworld::gridToString(mic::types::TensorXfPtr grid_) {
	std::string s;
	// Add line.
	s+= "+";
	for (size_t x=0; x<grid_->dim(0); x++)
		s+="---";
	s+= "+\n";

	for (size_t y=0; y<grid_->dim(1); y++){
		s += "|";
		for (size_t x=0; x<grid_->dim(0); x++) {
			// Check object occupancy.
			if ((*grid_)({x,y, (size_t)GridworldChannels::Agent}) != 0) {
				// Display agent.
				s += "<A>";
			} else if ((*grid_)({x,y, (size_t)GridworldChannels::Walls}) != 0) {
				// Display wall.
				s += " # ";
			} else if ((*grid_)({x,y, (size_t)GridworldChannels::Pits}) < 0) {
				// Display pit.
				s +=  " - ";
			} else if ((*grid_)({x,y, (size_t)GridworldChannels::Goals}) > 0) {
				// Display goal.
				s += " + ";
			} else
				s += "   ";
		}//: for x
		s += "|\n";
	}//: for y

	// Add line.
	s+= "+";
	for (size_t x=0; x<grid_->dim(0); x++)
		s+="---";
	s+= "+\n";
	return s;
}

std::string Gridworld::environmentToString() {
	return gridToString(environment_grid);
}

std::string Gridworld::observationToString() {
	if (pomdp_flag) {
		// Get observation.
		mic::types::TensorXfPtr obs = getObservation();
		return gridToString(obs);
	}
	else
		return gridToString(environment_grid);
}

mic::types::MatrixXfPtr Gridworld::encodeEnvironment() {
	// Temporarily reshape the environment_grid.
	environment_grid->conservativeResize({1, width * height * channels});
	// Create a matrix pointer and copy data from grid into the matrix.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(*environment_grid));
	// Back to the original shape.
	environment_grid->resize({width, height, channels});

	// Return the matrix pointer.
	return encoded_grid;
}

mic::types::MatrixXfPtr Gridworld::encodeObservation() {
	LOG(LDEBUG) << "encodeObservation()";
	if (pomdp_flag) {
		mic::types::Position2D p = getAgentPosition();
		LOG(LDEBUG) << p;

		// Get observation.
		mic::types::TensorXfPtr obs = getObservation();
		// Temporarily reshape the observation grid.
		obs->conservativeResize({1, roi_size * roi_size * channels});
		// Encode the observation.
		mic::types::MatrixXfPtr encoded_obs (new mic::types::MatrixXf(*obs));
		// Back to the original shape.
		obs->conservativeResize({roi_size, roi_size, channels});

		return encoded_obs;
	}
	else
		return encodeEnvironment();
}


mic::types::TensorXfPtr Gridworld::getObservation() {
	LOG(LDEBUG) << "getObservation()";
	// Reset.
	observation_grid->zeros();

	size_t delta = (roi_size-1)/2;
	mic::types::Position2D p = getAgentPosition();

	// Copy data.
	for (long oy=0, ey=(p.y-delta); oy< (long)roi_size; oy++, ey++){
		for (long ox=0, ex=(p.x-delta); ox< (long)roi_size; ox++, ex++) {
			// Check grid boundaries.
			if ((ex < 0) || (ex >= (long)width) || (ey < 0) || (ey >= (long)height)){
				// Place the wall only
				(*observation_grid)({(size_t)ox, (size_t)oy, (size_t)GridworldChannels::Walls}) = 1;
				continue;
			}//: if
			// Else : copy data for all channels.
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)GridworldChannels::Goals}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)GridworldChannels::Goals});
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)GridworldChannels::Pits}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)GridworldChannels::Pits});
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)GridworldChannels::Walls}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)GridworldChannels::Walls});
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)GridworldChannels::Agent}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)GridworldChannels::Agent});
		}//: for x
	}//: for y

	//LOG(LDEBUG) << std::endl << gridToString(observation_grid);

	return observation_grid;
}


mic::types::MatrixXfPtr Gridworld::encodeAgentGrid() {
	// DEBUG - copy only agent pose data, avoid goals etc.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(height, width));
	encoded_grid->setZero();

	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			// Check object occupancy.
			if ((*environment_grid)({x,y, (size_t)GridworldChannels::Agent}) != 0) {
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


mic::types::Position2D Gridworld::getAgentPosition() {
	mic::types::Position2D position;
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			if ((*environment_grid)({x,y, (size_t)GridworldChannels::Agent}) == 1) {
				position.x = x;
				position.y = y;
				return position;
			}// if
		}//: for x
	}//: for y
	// Remove warnings...
	return position;
}

bool Gridworld::moveAgentToPosition(mic::types::Position2D pos_) {
	LOG(LDEBUG) << "New agent position = " << pos_;

	// Check whether the state is allowed.
	if (!isStateAllowed(pos_))
		return false;

	// Clear old.
	mic::types::Position2D old = getAgentPosition();
	(*environment_grid)({(size_t)old.x, (size_t)old.y, (size_t)GridworldChannels::Agent}) = 0;
	// Set new.
	(*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Agent}) = 1;

	return true;
}



float Gridworld::getStateReward(mic::types::Position2D pos_) {
	// Check reward - goal or pit.
    if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pits}) != 0)
        return (*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pits});
    else if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goals}) != 0)
		return (*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goals});
	else
        return 0;
}


bool Gridworld::isStateAllowed(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= (long)width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= (long)height))
			return false;

	// Check walls!
	if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Walls}) != 0)
		return false;

	return true;
}


bool Gridworld::isStateTerminal(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= (long)width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= (long)height))
			return false;

	// Check reward - goal or pit.
    if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Pits}) != 0)
        return true;
    else if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)GridworldChannels::Goals}) != 0)
        return true;
    else

        return false;
}


} /* namespace environments */
} /* namespace mic */
