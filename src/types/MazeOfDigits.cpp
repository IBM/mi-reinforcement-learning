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

	return *this;
}


// Initialize environment_grid.
void MazeOfDigits::initializePropertyDependentVariables() {
	// Generate adequate gridworld.
	switch(type) {
		case 0 : initExemplaryMaze(); break;
		case -3:
		case -4: initRandomStructuredMaze(); break;
		case -2:
		case -1:
		default: initFullyRandomMaze();
	}//: switch

	// Check whether it is a POMDP or not.
	if (roi_size >0)
		pomdp_flag = true;
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
	environment_grid.resize({width, height, channels});
	environment_grid.zeros();

	// Place the agent.
	initial_position.set(0,1);
	moveAgentToPosition(initial_position);

	// Place digit
	environment_grid({0,0, (size_t)MazeOfDigitsChannels::Digits}) = 2;
	environment_grid({1,0, (size_t)MazeOfDigitsChannels::Digits}) = 4;
	environment_grid({2,0, (size_t)MazeOfDigitsChannels::Digits}) = 7;
	environment_grid({3,0, (size_t)MazeOfDigitsChannels::Digits}) = 7;
	environment_grid({0,1, (size_t)MazeOfDigitsChannels::Digits}) = 1;
	environment_grid({1,1, (size_t)MazeOfDigitsChannels::Digits}) = 5;
	environment_grid({2,1, (size_t)MazeOfDigitsChannels::Digits}) = 7;
	environment_grid({3,1, (size_t)MazeOfDigitsChannels::Digits}) = 9;
	environment_grid({0,2, (size_t)MazeOfDigitsChannels::Digits}) = 2;
	environment_grid({1,2, (size_t)MazeOfDigitsChannels::Digits}) = 3;
	environment_grid({2,2, (size_t)MazeOfDigitsChannels::Digits}) = 6;
	environment_grid({3,2, (size_t)MazeOfDigitsChannels::Digits}) = 8;
	environment_grid({0,3, (size_t)MazeOfDigitsChannels::Digits}) = 1;
	environment_grid({1,3, (size_t)MazeOfDigitsChannels::Digits}) = 2;
	environment_grid({2,3, (size_t)MazeOfDigitsChannels::Digits}) = 5;
	environment_grid({3,3, (size_t)MazeOfDigitsChannels::Digits}) = 6;


	// Place goal(s).
	environment_grid({3,0, (size_t)MazeOfDigitsChannels::Goals}) = 10;
}

void MazeOfDigits::initFullyRandomMaze() {
	LOG(LNOTICE) << "Generating a fully random " << width << "x" << height<< " maze of digits";

	static bool maze_generated = false;

	// It maze type = -1: do not generate new maze.
	if (((short)type == -1) && (maze_generated)) {
		// Generate only the new agent position.
		mic::types::Position2D agent(0, width-1, 0, height-1);
		initial_position = agent;
		moveAgentToPosition(initial_position);
		return;
	}

	// Set environment_grid size.
	environment_grid.resize({width, height, channels});
	environment_grid.zeros();

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
		if (environment_grid({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Agent}) != 0)
			continue;

		// Ok, add the goal.
		environment_grid({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Goals}) = 10;
		environment_grid({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Digits}) = 9;
		break;
	}//: while

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());

	// Initialize uniform integer distribution for digit.
	std::uniform_int_distribution<size_t> d_dist(0, 8);


	// Fill the "rest" with random digits.
	for(size_t x=0; x<width; x++ ){
		for(size_t y=0; y<height; y++ ){

			// Skip the goal.
			if ((x == goal.x) && (y == goal.y))
				continue;

			// Random variables.
			size_t d = d_dist(rng_mt19937_64);
			environment_grid({(size_t)x, (size_t)y, (size_t)MazeOfDigitsChannels::Digits}) = d;

		}//:for
	}//:for

	maze_generated = true;
}


void MazeOfDigits::initRandomStructuredMaze() {
	LOG(LNOTICE) << "Generating a structured random " << width << "x" << height<< " maze of digits";

	static bool maze_generated = false;

	// It maze type = -3: do not generate new maze.
	if (((short)type == -3) && (maze_generated)) {
		// Generate only the new agent position.
		mic::types::Position2D agent(0, width-1, 0, height-1);
		initial_position = agent;
		moveAgentToPosition(initial_position);
		return;
	}

	// Set environment_grid size.
	environment_grid.resize({width, height, channels});
	environment_grid.zeros();

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
		if (environment_grid({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Agent}) != 0)
			continue;

		// Ok, add the goal.
		environment_grid({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Goals}) = 10;
		environment_grid({(size_t)goal.x, (size_t)goal.y, (size_t)MazeOfDigitsChannels::Digits}) = 9;
		break;
	}//: while

	// Initialize random device and generator.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());


	// Fill the "rest" with random digits.
	for(size_t x=0; x<width; x++ ){
		for(size_t y=0; y<height; y++ ){

			// Skip the goal.
			if ((x == goal.x) && (y == goal.y))
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
			environment_grid({(size_t)x, (size_t)y, (size_t)MazeOfDigitsChannels::Digits}) = d;

		}//:for
	}//:for

	maze_generated = true;
}


std::string MazeOfDigits::gridToString(mic::types::TensorXf & grid_) {
	std::string s;
	// Add line.
	s+= "+";
	for (size_t x=0; x<grid_.dim(0); x++)
		s+="---";
	s+= "+\n";

	for (size_t y=0; y<grid_.dim(1); y++){
		s += "|";
		for (size_t x=0; x<grid_.dim(0); x++) {
			// Check object occupancy.
			if (grid_({x,y, (size_t)MazeOfDigitsChannels::Agent}) != 0) {
				// Display agent.
				s += "<A>";
			} else if (grid_({x,y, (size_t)MazeOfDigitsChannels::Walls}) != 0) {
				// Display wall.
				s += " # ";
/*			} else if (grid_({x,y, (size_t)MazeOfDigitsChannels::Goals}) > 0) {
				// Display goal.
				s += " + ,";*/
			} else
				// Display pit.
				s +=  " " + std::to_string((unsigned short)grid_({x,y, (size_t)MazeOfDigitsChannels::Digits})) + " ";
		}//: for x
		s += "|\n";
	}//: for y

	// Add line.
	s+= "+";
	for (size_t x=0; x<grid_.dim(0); x++)
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
		mic::types::TensorXf obs = getObservation();
		return gridToString(obs);
	}
	else
		return gridToString(environment_grid);
}

mic::types::MatrixXfPtr MazeOfDigits::encodeEnvironment() {
	// Temporarily reshape the environment_grid.
	environment_grid.conservativeResize({1, width * height * channels});
	// Create a matrix pointer and copy data from grid into the matrix.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(environment_grid));
	// Back to the original shape.
	environment_grid.resize({width, height, channels});

	// Return the matrix pointer.
	return encoded_grid;
}

mic::types::MatrixXfPtr MazeOfDigits::encodeObservation() {
	LOG(LDEBUG) << "encodeObservation()";
	if (pomdp_flag) {
		mic::types::Position2D p = getAgentPosition();
		LOG(LDEBUG) << p;

		mic::types::TensorXf obs = getObservation();
		obs.conservativeResize({1, roi_size * roi_size * channels});

		mic::types::MatrixXfPtr encoded_obs (new mic::types::MatrixXf(obs));

		return encoded_obs;
	}
	else
		return encodeEnvironment();
}


mic::types::TensorXf MazeOfDigits::getObservation() {
	LOG(LDEBUG) << "getObservation()";
	// Set size.
	mic::types::TensorXf observation({roi_size, roi_size, channels});
	observation.zeros();

	size_t delta = (roi_size-1)/2;
	mic::types::Position2D p = getAgentPosition();

	// Copy data.
	for (long oy=0, ey=(p.y-delta); oy<roi_size; oy++, ey++){
		for (long ox=0, ex=(p.x-delta); ox<roi_size; ox++, ex++) {
			// Check grid boundaries.
			if ((ex < 0) || (ex >= width) || (ey < 0) || (ey >= height)){
				// Place the wall only
				observation({(size_t)ox, (size_t)oy, (size_t)MazeOfDigitsChannels::Walls}) = 1;
				continue;
			}//: if
			// Else : copy data for all channels.
			observation({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Goals}) = environment_grid({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Goals});
			observation({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Digits}) = environment_grid({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Digits});
			observation({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Walls}) = environment_grid({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Walls});
			observation({(size_t)ox,(size_t)oy, (size_t)MazeOfDigitsChannels::Agent}) = environment_grid({(size_t)ex,(size_t)ey, (size_t)MazeOfDigitsChannels::Agent});
		}//: for x
	}//: for y

	LOG(LDEBUG) << std::endl << gridToString(observation);

	return observation;
}


mic::types::MatrixXfPtr MazeOfDigits::encodeAgentGrid() {
	// DEBUG - copy only agent pose data, avoid goals etc.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(height, width));
	encoded_grid->setZero();

	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			// Check object occupancy.
			if (environment_grid({x,y, (size_t)MazeOfDigitsChannels::Agent}) != 0) {
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
			if (environment_grid({x,y, (size_t)MazeOfDigitsChannels::Agent}) == 1) {
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
	environment_grid({(size_t)old.x, (size_t)old.y, (size_t)MazeOfDigitsChannels::Agent}) = 0;
	// Set new.
	environment_grid({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Agent}) = 1;

	return true;
}


float MazeOfDigits::getStateReward(mic::types::Position2D pos_) {
	// Check reward - goal.
    if (environment_grid({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Goals}) != 0)
		return environment_grid({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Goals});
	else
        return 0;
}


bool MazeOfDigits::isStateAllowed(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

	// Check walls!
	if (environment_grid({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Walls}) != 0)
		return false;

	return true;
}


bool MazeOfDigits::isStateTerminal(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

	// Check reward - goal or pit.
	if (environment_grid({(size_t)pos_.x, (size_t)pos_.y, (size_t)MazeOfDigitsChannels::Goals}) != 0)
        return true;
    else
        return false;
}
} /* namespace environments */
} /* namespace mic */
