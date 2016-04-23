/*!
 * \file GridworldQLearning.cpp
 * \brief 
 * \author tkornut
 * \date Mar 21, 2016
 */

#include <application/GridworldQLearning.hpp>

#include <limits>
#include <data_utils/RandomGenerator.hpp>
#include <application/GridworldQLearning.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::GridworldQLearning);
}


GridworldQLearning::GridworldQLearning(std::string node_name_) : OpenGLEpisodicApplication(node_name_),
		gridworld_type("gridworld_type", 0),
		width("width", 4),
		height("height", 4),
		step_reward("step_reward", 0.0),
		discount_rate("discount_rate", 0.9),
		learning_rate("learning_rate", 0.1),
		move_noise("move_noise",0.2),
		epsilon("epsilon", 0.1),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(gridworld_type);
	registerProperty(width);
	registerProperty(step_reward);
	registerProperty(discount_rate);
	registerProperty(learning_rate);
	registerProperty(move_noise);
	registerProperty(epsilon);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


GridworldQLearning::~GridworldQLearning() {

}


void GridworldQLearning::initialize(int argc, char* argv[]) {
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

void GridworldQLearning::initializePropertyDependentVariables() {
	// Generate the gridworld.
	gridworld.generateGridworld(gridworld_type, width, height);
	LOG(LSTATUS) << std::endl << gridworld.streamGrid();

	// Get width and height.
	width = gridworld.getWidth();
	height = gridworld.getHeight();

	// Resize and reset the action-value table.
	qstate_table.resize({width,height,4});
	qstate_table.zeros();
	//qstate_table.setValue( -std::numeric_limits<float>::infinity() );

	LOG(LSTATUS) << std::endl << streamQStateTable();
}


void GridworldQLearning::startNewEpisode() {
	LOG(LTRACE) << "Start new episode";
	// Move player to start position.
	gridworld.movePlayerToInitialPosition();

}


void GridworldQLearning::finishCurrentEpisode() {
	LOG(LTRACE) << "End current episode";

	/*	// Add variables to container.
		collector_ptr->addDataToContainer("average_reward",running_mean_reward);
		collector_ptr->addDataToContainer("correct_arms_percentage",correct_arms_percentage);
		collector_ptr->addDataToContainer("best_possible_reward",10.0*best_arm_prob);

		// Export reward "convergence" diagram.
		collector_ptr->exportDataToCsv(statistics_filename);*/

}



std::string GridworldQLearning::streamQStateTable() {
	std::ostringstream os;
	for (size_t y=0; y<height; y++){
		os << "| ";
		for (size_t x=0; x<width; x++) {
			for (size_t a=0; a<4; a++) {
				if ( qstate_table({x,y,a}) == -std::numeric_limits<float>::infinity())
					os << "-INF";
				else
					os << qstate_table({x,y,a});
				if (a==3)
					os << " | ";
				else
					os << " , ";

			}//: for a(ctions)
		}//: for x
		os << std::endl;
	}//: for y
	return os.str();

}




bool GridworldQLearning::move (mic::types::Action2DInterface ac_) {
//	LOG(LINFO) << "Current move = " << ac_;
	// Compute destination.
    mic::types::Position2D new_pos = gridworld.getPlayerPosition() + ac_;

	// Check whether the state is allowed.
	if (!gridworld.isStateAllowed(new_pos))
		return false;

	// Move player.
	gridworld.movePlayerToPosition(new_pos);
	return true;
}



float GridworldQLearning::computeBestValue(mic::types::Position2D pos_){
	float qbest_value = -std::numeric_limits<float>::infinity();
	// Check if the state is allowed.
	if (!gridworld.isStateAllowed(pos_))
		return qbest_value;

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the actions one by one.
	for(mic::types::NESWAction action : actions) {
		if(gridworld.isActionAllowed(pos_, action)) {
			float qvalue = qstate_table({(size_t)pos_.x, (size_t)pos_.y, (size_t)action.getType()});
			if (qvalue > qbest_value)
				qbest_value = qvalue;
		}//if is allowed
	}//: for

	return qbest_value;
}

mic::types::NESWAction GridworldQLearning::selectBestAction(mic::types::Position2D pos_){
	LOG(LTRACE) << "Select best action for state" << pos_;

	// Greedy methods - returns the index of element with greatest value.
	mic::types::NESWAction best_action = A_NONE;
    float best_qvalue = 0;

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the actions one by one.
	for(mic::types::NESWAction action : actions) {
		if(gridworld.isActionAllowed(pos_, action)) {
			float qvalue = qstate_table({(size_t)pos_.x, (size_t)pos_.y, (size_t)action.getType()});
			if (qvalue > best_qvalue){
				best_qvalue = qvalue;
				best_action = action;
			}
		}//if is allowed
	}//: for

	return best_action;
}

bool GridworldQLearning::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	// Get state s(t).
	mic::types::Position2D state_t = gridworld.getPlayerPosition();

	// Check whether state is terminal.
	if(gridworld.isStateTerminal(state_t)) {
		// In the terminal state we can select only one special action: "terminate".
		// All "other" actions receive the same value related to the "reward".
		float final_reward = gridworld.getStateReward(state_t);
		for (size_t a=0; a<4; a++)
			qstate_table({(size_t)state_t.x,(size_t)state_t.y, a}) = final_reward;

		LOG(LINFO) << "Player action = " << A_EXIT;
		LOG(LDEBUG) << "Player position = " << state_t;
		LOG(LSTATUS) << std::endl << gridworld.streamGrid();
		LOG(LSTATUS) << std::endl << streamQStateTable();

		// Finish the episode.
		return false;
	}//: if terminal


	mic::types::NESWAction action;
	double eps = (double)epsilon;
	if ((double)epsilon < 0)
		eps = 1.0/(1.0+episode);
	LOG(LWARNING) << eps;
	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > eps){
		// Select best action.
		action = selectBestAction(state_t);
		// If action could not be found.
		if (action.getType() == mic::types::NESW::None)
			action = A_RANDOM;
	} else {
		// Random move - repeat until an allowed move is made.
		action = A_RANDOM;
	}//: if

	// Execture action - until success.
	while (!move(action)) {
		// If action could not be performed - random.
		action = A_RANDOM;
	}//: while

	// Get new state s(t+1).
	mic::types::Position2D state_t_prim = gridworld.getPlayerPosition();

	LOG(LINFO) << "Performed action = " << action;
	LOG(LDEBUG) << "Player position = " << state_t;


	// Update running average for given action - Q learning;)
	float q_st_at = qstate_table({(size_t)state_t.x, (size_t)state_t.y, (size_t)action.getType()});
	float r = step_reward;
	float max_q_st_prim_at_prim = computeBestValue(state_t_prim);
	LOG(LDEBUG) << "q_st_at = " << q_st_at;
	LOG(LDEBUG) << "state_t_prim = " << state_t_prim;
	LOG(LDEBUG) << "step_reward = " << step_reward;
	LOG(LDEBUG) << "max_q_st_prim_at_prim = " << max_q_st_prim_at_prim;
	if (std::isfinite(q_st_at) && std::isfinite(max_q_st_prim_at_prim))
			qstate_table({(size_t)state_t.x, (size_t)state_t.y, (size_t)action.getType()}) = q_st_at + learning_rate * (r + discount_rate*max_q_st_prim_at_prim - q_st_at);

	LOG(LSTATUS) << std::endl << gridworld.streamGrid();
	LOG(LSTATUS) << std::endl << streamQStateTable();

	return true;
}


} /* namespace application */
} /* namespace mic */
