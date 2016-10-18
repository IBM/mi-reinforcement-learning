/*!
 * \file MazeOfDigits.cpp
 * \brief 
 * \author tkornut
 * \date May 2, 2016
 */

#include <types/MazeOfDigits.hpp>

namespace mic {
namespace environments {


MazeOfDigits::MazeOfDigits(std::string node_name_) : Environment(node_name_),
	type("type", 0)
{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(type);

	channels = (size_t)MazeOfDigitsChannels::Count;
}


MazeOfDigits::~MazeOfDigits() {
	// TODO Auto-generated destructor stub
}

mic::environments::MazeOfDigits & MazeOfDigits::operator= (const mic::environments::MazeOfDigits & gw_) {
	width = gw_.width;
	height = gw_.height;
	channels = gw_.channels;
	initial_position = gw_.initial_position;
	environment_grid = gw_.environment_grid;
	observation_grid = gw_.observation_grid;

	return *this;
}


void MazeOfDigits::initializePropertyDependentVariables() {
	// Empty - everything will be initialized in environment initialization.
}

void MazeOfDigits::initializeEnvironment() {
	// Generate adequate maze.
	switch(type) {
		case 0 : initExemplaryMaze(); break;
		case -3:
		case -4: initRandomStructuredMaze(); break;
		case -5:
		case -6: initRandomPathMaze(); break;
		case -2:
		case -1:
		default: initFullyRandomMaze();
	}//: switch

	// Check whether it is a POMDP or not.
	if (roi_size >0) {
		pomdp_flag = true;
		observation_grid->resize({roi_size, roi_size, 1});
	} else {
		observation_grid->resize({width, height, 1});
	}//: else
}

void MazeOfDigits::initExemplaryMaze() {
	LOG(LNOTICE) << "Generating an exemplary maze of digits";
	/*
	 * [['2','4','7','7'],
	 *  ['1','5','7','9'],
	 *  ['2','3','6','8'],
	 *  ['A','2','5','6']]
	 */


	// Overwrite dimensions.
	width = 4;
	height = 4;

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	initial_position.set(0,1);
	moveAgentToPosition(initial_position);

	// Place digit
	(*environment_grid)({0,0, (size_t)MazeOfDigitsChannels::Digits}) = 2;
	(*environment_grid)({1,0, (size_t)MazeOfDigitsChannels::Digits}) = 4;
	(*environment_grid)({2,0, (size_t)MazeOfDigitsChannels::Digits}) = 7;
	(*environment_grid)({3,0, (size_t)MazeOfDigitsChannels::Digits}) = 7;
	(*environment_grid)({0,1, (size_t)MazeOfDigitsChannels::Digits}) = 1;
	(*environment_grid)({1,1, (size_t)MazeOfDigitsChannels::Digits}) = 5;
	(*environment_grid)({2,1, (size_t)MazeOfDigitsChannels::Digits}) = 7;
	(*environment_grid)({3,1, (size_t)MazeOfDigitsChannels::Digits}) = 9;
	(*environment_grid)({0,2, (size_t)MazeOfDigitsChannels::Digits}) = 2;
	(*environment_grid)({1,2, (size_t)MazeOfDigitsChannels::Digits}) = 3;
	(*environment_grid)({2,2, (size_t)MazeOfDigitsChannels::Digits}) = 6;
	(*environment_grid)({3,2, (size_t)MazeOfDigitsChannels::Digits}) = 8;
	(*environment_grid)({0,3, (size_t)MazeOfDigitsChannels::Digits}) = 1;
	(*environment_grid)({1,3, (size_t)MazeOfDigitsChannels::Digits}) = 2;
	(*environment_grid)({2,3, (size_t)MazeOfDigitsChannels::Digits}) = 5;
	(*environment_grid)({3,3, (size_t)MazeOfDigitsChannels::Digits}) = 6;


	// Place goal(s).
	(*environment_grid)({3,0, (size_t)MazeOfDigitsChannels::Goals}) = 10;

	// Calculate the optimal path length.
	optimal_path_length = 4;

}

void MazeOfDigits::reRandomAgentPosition() {
	// Generate only the new agent position.

	// Find the goal.
	mic::types::Position2D goal;
	for (size_t x = 0; x < width; x++)
		for (size_t y = 0; y < height; y++)
			if ((*environment_grid)({ x, y, (size_t)MazeOfDigitsChannels::Goals }) > 0) {
				goal.x = x;
				goal.y = y;
				break;
			} //: if

	// Try to place the agent.
	mic::types::Position2D agent;
	while (1) {
		// Random position.
		agent.rand(0, width - 1, 0, height - 1);

		// Validate pose.
		if ((*environment_grid)({ (size_t)agent.x, (size_t)agent.y, (size_t)MazeOfDigitsChannels::Goals }) != 0)
			continue;

		// Ok, move agent to that position.
		initial_position = agent;
		moveAgentToPosition(initial_position);
		break;
	} //: while

	// Recalculate the optimal path length.
	optimal_path_length = abs((int) goal.x - (int) agent.x) + abs((int) goal.y - (int) agent.y);
}


void MazeOfDigits::initFullyRandomMaze() {
	LOG(LNOTICE) << "Generating a fully random " << width << "x" << height<< " maze of digits";

	static bool maze_generated = false;

	// It maze type = -1: do not generate new maze.
	if (((short)type == -1) && (maze_generated)) {
		reRandomAgentPosition();
		return;
	}

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	mic::types::Position2D agent(0, width-1, 0, height-1);
	initial_position = agent;
	moveAgentToPosition(initial_position);


	// Place goal.
	mic::types::Position2D goal;
	while(1) {
		// Random position.
		goal.rand(0, width-1, 0, height-1);

		// Validate pose.
		if ((*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Agent}) != 0)
			continue;

		// Ok, add the goal.
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Goals}) = 10;
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Digits}) = 9;
		break;
	}//: while

	// Calculate the optimal path length.
	optimal_path_length = abs((int)goal.x-(int)agent.x) + abs((int)goal.y-(int)agent.y);

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());

	// Initialize uniform integer distribution for digit.
	std::uniform_int_distribution<size_t> d_dist(0, 8);


	// Fill the "rest" with random digits.
	for(size_t x=0; x<width; x++ ){
		for(size_t y=0; y<height; y++ ){

			// Skip the goal.
			if ((x == (size_t)goal.x) && (y == (size_t)goal.y))
				continue;

			// Random variables.
			size_t d = d_dist(rng_mt19937_64);
			(*environment_grid)({(size_t)x, (size_t)y, (size_t)MazeOfDigitsChannels::Digits}) = d;

		}//:for
	}//:for

	maze_generated = true;
}


void MazeOfDigits::initRandomStructuredMaze() {
	LOG(LNOTICE) << "Generating a structured random " << width << "x" << height<< " maze of digits";

	static bool maze_generated = false;

	// It maze type = -3: do not generate new maze.
	if (((short)type == -3) && (maze_generated)) {
		reRandomAgentPosition();
		return;
	}

	// Set environment_grid size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();

	// Place the agent.
	mic::types::Position2D agent(0, width-1, 0, height-1);
	initial_position = agent;
	moveAgentToPosition(initial_position);


	// Place goal.
	mic::types::Position2D goal;
	while(1) {
		// Random position.
		goal.rand(0, width-1, 0, height-1);

		// Validate pose.
		if ((*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Agent}) != 0)
			continue;

		// Ok, add the goal.
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Goals}) = 10;
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Digits}) = 9;
		break;
	}//: while

	// Calculate the optimal path length.
	optimal_path_length = abs((int)goal.x-(int)agent.x) + abs((int)goal.y-(int)agent.y);

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());


	// Fill the "rest" with random digits.
	for(size_t x=0; x<width; x++ ){
		for(size_t y=0; y<height; y++ ){

			// Skip the goal.
			if ((x == (size_t)goal.x) && (y == (size_t)goal.y))
				continue;

			// Calculate the distance.
			float dist = (float)sqrt((x-goal.x)*(x-goal.x) + (y-goal.y)*(y-goal.y));
			// Take into account the scale - size of maze.
			float scaled_dist = 10*dist/sqrt((width*height));
			// Truncate it to 0-0.
			size_t min, max;
			if (scaled_dist<1.1) {
				min = max = 8;
			} else {
				min = 9 - ((scaled_dist > 9) ? 9 : scaled_dist);
				max = ((min + 1 > 9) ? 9 : min + 1);
			}//: else

			// Random variables.
			std::uniform_int_distribution<size_t> d_dist(min, max);
			size_t d = d_dist(rng_mt19937_64);
			LOG(LDEBUG)<< " x = " << x << " goal.x = " << goal.x << " y = " << y << " goal.y = " << goal.y << " dist = " << dist << " scaled_dist = " << scaled_dist << " min = " << min << " max = " << max << " d = " << d;
			(*environment_grid)({(size_t)x, (size_t)y, (size_t)MazeOfDigitsChannels::Digits}) = d;

		}//:for
	}//:for

	maze_generated = true;
}

void MazeOfDigits::initRandomPathMaze() {

	LOG(LNOTICE) << "Generating a random patch " << width << "x" << height<< " maze of digits";

	static bool maze_generated = false;

	// It maze type = -5: do not generate new maze.
	if (((short)type == -5) && (maze_generated)) {
		reRandomAgentPosition();
		return;
	}

	// Set environment size.
	environment_grid->resize({width, height, channels});
	environment_grid->zeros();
//	environment_grid->setValue(-1);

	// Place the agent.
	mic::types::Position2D agent(0, width-1, 0, height-1);
	initial_position = agent;
	moveAgentToPosition(initial_position);


	// Place goal.
	mic::types::Position2D goal;
	while(1) {
		// Random position.
		goal.rand(0, width-1, 0, height-1);

		// Validate pose.
		if ((*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Agent}) != 0)
			continue;

		// Ok, add the goal.
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Goals}) = 10;
		(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Digits}) = 9;
		break;
	}//: while

	// Calculate the optimal path length.
	optimal_path_length = abs((int)goal.x-(int)agent.x) + abs((int)goal.y-(int)agent.y);

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());

	// Check quarter and calculate "main path direction".
	types::NESWAction dir_min, dir_max;
	if (((size_t)goal.x < width/2) && ((size_t)goal.y < height/2)) {
		// First square -> go E, S or ES.
		dir_min.dx = 0;
		dir_max.dx = 1;
		dir_min.dy = 0;
		dir_max.dy = 1;

	} else if (((size_t)goal.x >= width/2) && ((size_t)goal.y < height/2)) {
		// Second square -> go S or WS or W.
		dir_min.dx = -1;
		dir_max.dx = 0;
		dir_min.dy = 0;
		dir_max.dy = 1;

	} else if (((size_t)goal.x >= width/2) && ((size_t)goal.y >= height/2)) {
		// Third square -> go N, W or NW
		dir_min.dx = -1;
		dir_max.dx = 0;
		dir_min.dy = -1;
		dir_max.dy = 0;

	} else {
		// Fourth square -> go N, E or NE
		dir_min.dx = 0;
		dir_max.dx = 1;
		dir_min.dy = -1;
		dir_max.dy = 0;
	}

	// Generate path direction distributions.
	std::uniform_int_distribution<size_t> x_dist(dir_min.dx, dir_max.dx);
	std::uniform_int_distribution<size_t> y_dist(dir_min.dy, dir_max.dy);

	// Create a path starting in the goal and using the main direction.
	mic::types::Position2D cur = goal;

	std::vector<mic::types::Position2D> path;
	path.push_back(goal);
	while (1) {

		// "Move" ;)
		types::NESWAction action;
		action.dx = x_dist(rng_mt19937_64);
		action.dy = y_dist(rng_mt19937_64);
		cur = cur + action;

		if ((cur.x < 0) || (cur.y < 0) || ((size_t)cur.x >= width) || ((size_t)cur.y >= height))
			break;

		// Skip the goal.
		if ((cur.x == goal.x) && (cur.y == goal.y))
			continue;

		// Add point to path.
		path.push_back(cur);

		// Calculate the distance from goal.
		float dist = (float)sqrt((cur.x-goal.x)*(cur.x-goal.x) + (cur.y-goal.y)*(cur.y-goal.y));
		// Take into account the scale - size of maze.
		float scaled_dist = 5*dist/sqrt((width*height));
		// Truncate it to 0-0.
		size_t min, max;
		if (scaled_dist<1.1) {
			min = max = 8;
		} else {
			min = 9 - ((scaled_dist > 9) ? 9 : scaled_dist);
			max = ((min + 1 > 9) ? 9 : min + 1);
		}//: else

		// Random variables.
		std::uniform_int_distribution<size_t> d_dist(min, max);
		size_t d = d_dist(rng_mt19937_64);
		LOG(LDEBUG)<< " x = " << cur.x << " goal.x = " << goal.x << " y = " << cur.y << " goal.y = " << goal.y << " dist = " << dist << " scaled_dist = " << scaled_dist << " min = " << min << " max = " << max << " d = " << d;
		(*environment_grid)({(size_t)cur.x, (size_t)cur.y, (size_t)MazeOfDigitsChannels::Digits}) = d;

	};

	// "Grow the path" by 1.
	for (auto point : path) {
		// Create distribution basing on patch point.
		size_t min, max;
		max = (*environment_grid)({(size_t)point.x, (size_t)point.y, (size_t)MazeOfDigitsChannels::Digits});
		max = (max == 9) ? 8 : max;
		min = ((max < 1) ? 1 : max - 1); // size_t truncates that to zero.
		std::uniform_int_distribution<size_t> d_dist(min, max);

		// Check 4 neighbours.
		setBiggerDigit(point.x, point.y-1, d_dist(rng_mt19937_64));
		setBiggerDigit(point.x-1, point.y, d_dist(rng_mt19937_64));
		setBiggerDigit(point.x+1, point.y, d_dist(rng_mt19937_64));
		setBiggerDigit(point.x, point.y+1, d_dist(rng_mt19937_64));
	}//:for

	// "Grow the path" by 2.
	for (auto point : path) {
		// Create distribution basing on patch point.
		size_t min, max;
		max = (*environment_grid)({(size_t)point.x, (size_t)point.y, (size_t)MazeOfDigitsChannels::Digits});
		max = (max > 8) ? 7 : max;
		min = ((max < 2) ? 1 : max - 2); // size_t truncates that to zero.
		std::uniform_int_distribution<size_t> d2_dist(min, max);

		// Check 8 neighbours.
		setBiggerDigit(point.x, point.y-2, d2_dist(rng_mt19937_64));
		setBiggerDigit(point.x-1, point.y-1, d2_dist(rng_mt19937_64));
		setBiggerDigit(point.x+1, point.y-1, d2_dist(rng_mt19937_64));
		setBiggerDigit(point.x-2, point.y, d2_dist(rng_mt19937_64));
		setBiggerDigit(point.x+2, point.y, d2_dist(rng_mt19937_64));
		setBiggerDigit(point.x-1, point.y+1, d2_dist(rng_mt19937_64));
		setBiggerDigit(point.x+1, point.y+1, d2_dist(rng_mt19937_64));
		setBiggerDigit(point.x, point.y+2, d2_dist(rng_mt19937_64));
	}//:for

	// "Grow the path" by 3.
	for (auto point : path) {
		// Create distribution basing on patch point.
		size_t min, max;
		max = (*environment_grid)({(size_t)point.x, (size_t)point.y, (size_t)MazeOfDigitsChannels::Digits});
		max = (max > 7) ? 6 : max;
		min = ((max < 2) ? 1 : max - 2); // size_t truncates that to zero.
		std::uniform_int_distribution<size_t> d3_dist(min, max);

		// Check 8 neighbours.
		setBiggerDigit(point.x+2, point.y-1, d3_dist(rng_mt19937_64));
		setBiggerDigit(point.x+2, point.y+1, d3_dist(rng_mt19937_64));
		setBiggerDigit(point.x-2, point.y-1, d3_dist(rng_mt19937_64));
		setBiggerDigit(point.x-2, point.y+1, d3_dist(rng_mt19937_64));
		setBiggerDigit(point.x+1, point.y-2, d3_dist(rng_mt19937_64));
		setBiggerDigit(point.x+1, point.y+2, d3_dist(rng_mt19937_64));
		setBiggerDigit(point.x-1, point.y-2, d3_dist(rng_mt19937_64));
		setBiggerDigit(point.x-1, point.y+2, d3_dist(rng_mt19937_64));
	}//:for

	// Fill the "rest" with random digits.
	for(size_t x=0; x<width; x++ ){
		for(size_t y=0; y<height; y++ ){

			// Random variables.
			std::uniform_int_distribution<size_t> d_dist(0, 1);
			size_t d = d_dist(rng_mt19937_64);
			if ((*environment_grid)({(size_t)x, (size_t)y, (size_t)MazeOfDigitsChannels::Digits}) == 0)
				(*environment_grid)({(size_t)x, (size_t)y, (size_t)MazeOfDigitsChannels::Digits}) = d;

		}//:for
	}//:for

	// Quick fix :]
//	(*environment_grid)({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Digits}) = 9;

	maze_generated = true;
}

void MazeOfDigits::setBiggerDigit(size_t x_, size_t y_, size_t value_){
	//if ((x_ < 0)  || (y_ < 0) || (x_ >= width) || (y_ >= height))
	if ((x_ >= width) || (y_ >= height))
		return;

	if ((*environment_grid)({(size_t)x_, (size_t)y_, (size_t)MazeOfDigitsChannels::Digits}) < value_)
		(*environment_grid)({(size_t)x_, (size_t)y_, (size_t)MazeOfDigitsChannels::Digits}) = value_;
}

std::string MazeOfDigits::gridToString(mic::types::TensorXfPtr & grid_) {
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
			if ((*grid_)({x,y, (size_t)MazeOfDigitsChannels::Agent}) != 0) {
				// Display agent.
				s += "<A>";
			} else if ((*grid_)({x,y, (size_t)MazeOfDigitsChannels::Walls}) != 0) {
				// Display wall.
				s += " # ";
/*			} else if ((*grid_)({x,y, (size_t)MazeOfDigitsChannels::Goals}) > 0) {
				// Display goal.
				s += " + ,";*/
			} else
				// Display pit.
				s +=  " " + std::to_string((unsigned short)(*grid_)({x,y, (size_t)MazeOfDigitsChannels::Digits})) + " ";
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

std::string MazeOfDigits::environmentToString() {
	return gridToString(environment_grid);
}

std::string MazeOfDigits::observationToString() {
	if (pomdp_flag) {
		// Get observation.
		mic::types::TensorXfPtr obs = getObservation();
		return gridToString(obs);
	}
	else
		return gridToString(environment_grid);
}

mic::types::MatrixXfPtr MazeOfDigits::encodeEnvironment() {
	// Temporarily reshape the environment grid.
	environment_grid->conservativeResize({1, width * height * channels});
	// Create a matrix pointer and copy data from grid into the matrix.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(*environment_grid));
	// Back to the original shape.
	environment_grid->resize({width, height, channels});

	// Return the matrix pointer.
	return encoded_grid;
}

mic::types::MatrixXfPtr MazeOfDigits::encodeObservation() {
	LOG(LDEBUG) << "encodeObservation()";
	if (pomdp_flag) {
		mic::types::Position2D p = getAgentPosition();
		LOG(LDEBUG) << p;

		// Get observation.
		mic::types::TensorXfPtr obs = getObservation();
		// Temporarily reshape the observation grid.
		obs->conservativeResize({1, roi_size * roi_size * 1});
		// Encode the observation.
		mic::types::MatrixXfPtr encoded_obs (new mic::types::MatrixXf(*obs));
		// Back to the original shape.
		obs->conservativeResize({roi_size, roi_size, 1});

		// Return encoded observation.
		return encoded_obs;
	}
	else
		return encodeEnvironment();
}


mic::types::TensorXfPtr MazeOfDigits::getObservation() {
	LOG(LDEBUG) << "getObservation()";
	// Reset observation.
	observation_grid->zeros();

	size_t delta = (roi_size-1)/2;
	mic::types::Position2D p = getAgentPosition();

	// Copy data.
	for (long oy=0, ey=(p.y-delta); oy<(long)roi_size; oy++, ey++){
		for (long ox=0, ex=(p.x-delta); ox<(long)roi_size; ox++, ex++) {
			// Check grid boundaries.
			if ((ex < 0) || (ex >= (long)width) || (ey < 0) || (ey >= (long)height)){
				// Place the wall only
				//(*observation_grid)({(size_t)ox, (size_t)oy, (size_t)MazeOfDigitsChannels::Walls}) = 1;
				continue;
			}//: if
			// Else : copy data for all channels.
			//(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Goals}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Goals});
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Digits}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Digits});
			//(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Walls}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Walls});
			//(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Agent}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Agent});
		}//: for x
	}//: for y

	//LOG(LDEBUG) << std::endl << gridToString(observation_grid);

	return observation_grid;
}


mic::types::MatrixXfPtr MazeOfDigits::encodeAgentGrid() {
	// DEBUG - copy only agent pose data, avoid goals etc.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(height, width));
	encoded_grid->setZero();

	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			// Check object occupancy.
			if ((*environment_grid)({x,y, (size_t)MazeOfDigitsChannels::Agent}) != 0) {
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


mic::types::Position2D MazeOfDigits::getAgentPosition() {
	mic::types::Position2D position;
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			if ((*environment_grid)({x,y, (size_t)MazeOfDigitsChannels::Agent}) == 1) {
				position.x = x;
				position.y = y;
				return position;
			}// if
		}//: for x
	}//: for y
	// Remove warnings...
	return position;
}

bool MazeOfDigits::moveAgentToPosition(mic::types::Position2D pos_) {
	LOG(LDEBUG) << "New agent position = " << pos_;

	// Check whether the state is allowed.
	if (!isStateAllowed(pos_))
		return false;

	// Clear old.
	mic::types::Position2D old = getAgentPosition();
	(*environment_grid)({(size_t)old.x, (size_t)old.y, (size_t)MazeOfDigitsChannels::Agent}) = 0;
	// Set new.
	(*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Agent}) = 1;

	return true;
}


float MazeOfDigits::getStateReward(mic::types::Position2D pos_) {
	// Check reward - goal.
    if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Goals}) != 0)
		return (*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Goals});
	else
        return 0;
}


bool MazeOfDigits::isStateAllowed(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || ((size_t)pos_.x >= width))
		return false;

	if ((pos_.y < 0) || ((size_t)pos_.y >= height))
			return false;

	// Check walls!
	if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Walls}) != 0)
		return false;

	return true;
}


bool MazeOfDigits::isStateTerminal(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || ((size_t)pos_.x >= width))
		return false;

	if ((pos_.y < 0) || ((size_t)pos_.y >= height))
			return false;

	// Check reward - goal or pit.
	if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Goals}) != 0)
        return true;
    else
        return false;
}
} /* namespace environments */
} /* namespace mic */
