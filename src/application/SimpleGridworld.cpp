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
	gridworld.resize({height, height, 4});
	gridworld.zeros();

	initGrid();
}


void SimpleGridworld::initGrid() {
	// place player
	//state[0,1] = np.array([0,0,0,1])
	gridworld({0,1,3}) = 1;

	// place wall
	//state[2,2] = np.array([0,0,1,0])
	gridworld({2,2,2}) = 1;

	// place pit
	//state[1,1] = np.array([0,1,0,0])
	gridworld({1,1,1}) = 1;

	// place the goal
	//state[3,3] = np.array([1,0,0,0])
	gridworld({3,3,0}) = 1;

	std::cout<< gridworld << std::endl;

	displayGrid();
}


void SimpleGridworld::displayGrid() {
	mic::types::Tensor<char> grid;
	grid.resize({width, height});
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			// Check object occupancy.
			if (gridworld({x,y,3}) == 1) {
				// Display player.
				grid({x,y}) = 'P';
			} else if (gridworld({x,y,2}) == 1) {
				// Display wall.
				grid({x,y}) = 'W';
			} else if (gridworld({x,y,1}) == 1) {
				// Display pit.
				grid({x,y}) = '-';
			} else if (gridworld({x,y,0}) == 1) {
				// Display goal.
				grid({x,y}) = '+';
			} else
				grid({x,y}) = ' ';
		}//: for x
	}//: for y

	//std::cout<< grid;
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			std::cout << grid({x,y}) << " , ";
		}//: for x
		std::cout << std::endl;
	}//: for y
}


std::pair<size_t, size_t> SimpleGridworld::getPlayerPosition() {
	std::pair<size_t, size_t> position = std::make_pair<size_t, size_t>(0,0);
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			if (gridworld({x,y,3}) == 1) {
				position.first = x;
				position.second = y;
			}// if
		}//: for x
	}//: for y
	return position;
}


short SimpleGridworld::calculateReward() {
	std::pair<size_t, size_t> player_position = getPlayerPosition();
    if (gridworld({player_position.first, player_position.second, 1}) == 1)
    	// Pit.
        return -10;
    else if (gridworld({player_position.first, player_position.second, 0}) == 1)
    	// Goal.
        return 10;
    else
        return -1;
}


void SimpleGridworld::move (mic::types::Action2DInterface ac_) {
	LOG(LINFO) << "Current move dx,dy= ( " << ac_.dx() << "," <<ac_.dy()<< ")";
	// Get player position.
	std::pair<size_t, size_t> player_position = getPlayerPosition();
	size_t px = player_position.first;
	size_t py = player_position.second;
	// "Reset" player position.
	gridworld({px,py,3}) = 0;

	// Calculate new coordinates - this will already truncate values to be >= 0.
	size_t nx = px + ac_.dx();
	size_t ny = py + ac_.dy();

	// Check move.
	if (nx >= width)
		nx = px;
	if (ny >= height)
		ny = py;

	// Check walls!.
	if (gridworld({nx, ny, 2}) == 1) {
		nx = px;
		ny = py;
	}//: if

	gridworld({nx, ny, 3}) = 1;

}


bool SimpleGridworld::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	move(A_RANDOM);

	displayGrid();
	std::pair<size_t, size_t> player_position = getPlayerPosition();
	std::cout << "Player pose = (" << player_position.first << "," << player_position.second << ")\n";
	std::cout << "Reward: " << calculateReward() << std::endl;
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
