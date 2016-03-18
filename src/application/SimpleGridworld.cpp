/*!
 * \file SimpleGridworld.cpp
 * \brief 
 * \author tkornut
 * \date Mar 17, 2016
 */

#include <application/SimpleGridworld.hpp>

#include  <data_utils/RandomGenerator.hpp>

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
		init_type("init_type", 0),
		width("width", 4),
		height("height", 4),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(init_type);
	registerProperty(width);
	registerProperty(height);
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
	initClassicCliffGrid();
}


void SimpleGridworld::initExemplaryGrid() {
	// Set gridworld size.
	gridworld.resize({width, height, 4});
	gridworld.zeros();

	// Place the player.
	gridworld({0,1, (size_t)GridworldChannels::Player}) = 1;

	// Place wall(s).
	gridworld({2,2, (size_t)GridworldChannels::Wall}) = 1;

	// Place pit(s).
	gridworld({1,1, (size_t)GridworldChannels::Pit}) = -10;

	// Place goal(s).
	gridworld({3,3, (size_t)GridworldChannels::Goal}) = 10;
}


void SimpleGridworld::initClassicCliffGrid() {
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
	gridworld({0,1, (size_t)GridworldChannels::Player}) = 1;

	// Place pit(s).
	for(size_t x=0; x<width; x++)
		gridworld({x,2, (size_t)GridworldChannels::Pit}) = -100;

	// Place goal(s).
	gridworld({4,1, (size_t)GridworldChannels::Goal}) = 10;
}

/*def getCliffGrid2():
    grid = [[' ',' ',' ',' ',' '],
            [8,'S',' ',' ',10],
            [-100,-100, -100, -100, -100]]
    return Gridworld(grid)

def getDiscountGrid():
    grid = [[' ',' ',' ',' ',' '],
            [' ','#',' ',' ',' '],
            [' ','#', 1,'#', 10],
            ['S',' ',' ',' ',' '],
            [-10,-10, -10, -10, -10]]
    return Gridworld(grid)

def getBridgeGrid():
    grid = [[ '#',-100, -100, -100, -100, -100, '#'],
            [   1, 'S',  ' ',  ' ',  ' ',  ' ',  10],
            [ '#',-100, -100, -100, -100, -100, '#']]
    return Gridworld(grid)

def getBookGrid():
    grid = [[' ',' ',' ',+1],
            [' ','#',' ',-1],
            ['S',' ',' ',' ']]
    return Gridworld(grid)

def getMazeGrid():
    grid = [[' ',' ',' ',+1],
            ['#','#',' ','#'],
            [' ','#',' ',' '],
            [' ','#','#',' '],
            ['S',' ',' ',' ']]
    return Gridworld(grid)*/


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



mic::types::Position2D SimpleGridworld::getPlayerPosition() {
	mic::types::Position2D position;
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			if (gridworld({x,y,3}) == 1) {
				position.x = x;
				position.y = y;
			}// if
		}//: for x
	}//: for y
	return position;
}


short SimpleGridworld::calculateReward() {
	mic::types::Position2D player_position = getPlayerPosition();
    if (gridworld({(size_t)player_position.x, (size_t)player_position.y, (size_t)GridworldChannels::Pit}) != 0)
    	// Pit.
        return -10;
    else if (gridworld({(size_t)player_position.x, (size_t)player_position.y, (size_t)GridworldChannels::Goal}) != 0)
    	// Goal.
        return 10;
    else
        return -1;
}

bool SimpleGridworld::isPositionAllowed(long x, long y) {
        if ((x < 0) || (x >= width))
    		return false;

        if ((y < 0) || (y >= height))
        		return false;

        // Check walls!
    	if (gridworld({(size_t)x, (size_t)y, (size_t)GridworldChannels::Wall}) != 0)
    		return false;

        return true;
}

bool SimpleGridworld::move (mic::types::Action2DInterface ac_) {
	LOG(LINFO) << "Current move = " << ac_;
	// Get player position.
	mic::types::Position2D player_position = getPlayerPosition();
	size_t px = player_position.x;
	size_t py = player_position.y;

	// Calculate new coordinates - this will already truncate values to be >= 0.
	long nx = px + ac_.dx;
	long ny = py + ac_.dy;

	// Check whether the "destination" (new position) is valid.
	if (!isPositionAllowed(nx, ny))
		return false;

	// "Reset" previous player position and set the new one.
	gridworld({px,py,3}) = 0;
	gridworld({(size_t)nx, (size_t)ny, 3}) = 1;
	return true;
}


bool SimpleGridworld::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	// Move randomly - repeat until an allowed move is made.
	while (!move(A_RANDOM))
		;

	LOG(LSTATUS) << std::endl << streamGrid();
	mic::types::Position2D player_position = getPlayerPosition();
	LOG(LINFO) << "Player pose = " << player_position;
	LOG(LINFO) << "Reward: " << calculateReward();

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
