/*!
 * \file GridworldDeepQLearning.cpp
 * \brief 
 * \author tkornut
 * \date Apr 21, 2016
 */

#include <limits>
#include <data_utils/RandomGenerator.hpp>
#include <application/GridworldDeepQLearning.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::GridworldDeepQLearning);
}


GridworldDeepQLearning::GridworldDeepQLearning(std::string node_name_) : OpenGLEpisodicApplication(node_name_),
		gridworld_type("gridworld_type", 0),
		width("width", 4),
		height("height", 4),
		step_reward("step_reward", 0.0),
		discount_rate("discount_rate", 0.9),
		learning_rate("learning_rate", 0.1),
		move_noise("move_noise",0.2),
		epsilon("epsilon", 0.1),
		statistics_filename("statistics_filename","statistics_filename.csv"),
		predicted_rewards_t (new MatrixXf(4, 1))

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


GridworldDeepQLearning::~GridworldDeepQLearning() {

}


void GridworldDeepQLearning::initialize(int argc, char* argv[]) {
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

void GridworldDeepQLearning::initializePropertyDependentVariables() {
	// Initialize gridworld.
	switch(gridworld_type) {
		case 0 : state.initExemplaryGrid(); break;
		case 1 : state.initClassicCliffGrid(); break;
		case 2 : state.initDiscountGrid(); break;
		case 3 : state.initBridgeGrid(); break;
		case 4 : state.initBookGrid(); break;
		case 5 : state.initMazeGrid(); break;
		case 6 : state.initExemplaryDQLGrid(); break;
		case -1:
		default: state.initRandomGrid(width, height);
	}//: switch

	LOG(LSTATUS) << std::endl << state.streamGrid();
	// Get width and height.
	width = state.getWidth();
	height = state.getHeight();

	// Create a simple neural network.
	// gridworld wxhx4 -> 256 -> 100 -> 4; batch size is set to one.
	neural_net.addLayer(new Linear((size_t) width * height * 4, 256, 1));
	neural_net.addLayer(new ReLU(256, 256, 1));
	neural_net.addLayer(new Linear(256, 100, 1));
	neural_net.addLayer(new ReLU(100, 100, 1));
	neural_net.addLayer(new Linear(100, 4, 1));
	//neural_net.addLayer(new Softmax(4, 4, 1));
}


void GridworldDeepQLearning::startNewEpisode() {
	LOG(LTRACE) << "Start new episode";
	// Move player to start position.
	state.movePlayerToInitialPosition();

}


void GridworldDeepQLearning::finishCurrentEpisode() {
	LOG(LTRACE) << "End current episode";

	/*	// Add variables to container.
		collector_ptr->addDataToContainer("average_reward",running_mean_reward);
		collector_ptr->addDataToContainer("correct_arms_percentage",correct_arms_percentage);
		collector_ptr->addDataToContainer("best_possible_reward",10.0*best_arm_prob);

		// Export reward "convergence" diagram.
		collector_ptr->exportDataToCsv(statistics_filename);*/

}


bool GridworldDeepQLearning::move (mic::types::Action2DInterface ac_) {
//	LOG(LINFO) << "Current move = " << ac_;
	// Compute destination.
    mic::types::Position2D new_pos = state.getPlayerPosition() + ac_;

	// Check whether the state is allowed.
	if (!state.isStateAllowed(new_pos))
		return false;

	// Move player.
	state.movePlayerToPosition(new_pos);
	return true;
}



float GridworldDeepQLearning::computeBestValue(){
	LOG(LTRACE) << "computeBestValue";
	float best_qvalue = -std::numeric_limits<float>::infinity();

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the results of actions one by one...
	float* pred = predicted_rewards_t->data();
	for(mic::types::NESWAction action : actions) {
		// .. and find the value of teh best allowed action.
		if(state.isActionAllowed(action)) {
			float qvalue = pred[(size_t)action.getType()];
			if (qvalue > best_qvalue)
				best_qvalue = qvalue;
		}//if is allowed
	}//: for

	return best_qvalue;
}

mic::types::NESWAction GridworldDeepQLearning::selectBestAction(){
	LOG(LTRACE) << "selectBestAction";

	// Greedy methods - returns the index of element with greatest value.
	mic::types::NESWAction best_action = A_RANDOM;
    float best_qvalue = 0;

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the results of actions one by one...
	float* pred = predicted_rewards_t->data();
	for(mic::types::NESWAction action : actions) {
		// ... and find the best allowed.
		if(state.isActionAllowed(action)) {
			float qvalue = pred[(size_t)action.getType()];
			if (qvalue > best_qvalue){
				best_qvalue = qvalue;
				best_action = action;
			}
		}//if is allowed
	}//: for

	return best_action;
}

bool GridworldDeepQLearning::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	// TMP!
	double 	learning_rate = 0.005;
	double 	weight_decay = 0;

	// Get state s(t).
	mic::types::Position2D player_pos_t= state.getPlayerPosition();
	// Encoded the current state at time t (prior to making an action).
	mic::types::MatrixXfPtr encoded_state_t = state.encodeGrid();

	// Pass the data and get predictions.
	neural_net.forward(*encoded_state_t);
	predicted_rewards_t = neural_net.getPredictions();
	LOG(LSTATUS) << "Predicted rewards for state t: " << predicted_rewards_t->transpose();

	// Check whether state is terminal.
	if(state.isStateTerminal(player_pos_t)) {
		// In the terminal state we can select only one special action: "terminate".
		// All "other" actions receive the same value related to the "reward".
		float final_reward = state.getStateReward(player_pos_t);

		// Set all "rewards" to the final value.
		predicted_rewards_t->setValue(final_reward);
		LOG(LWARNING) << "Desired rewards: " << predicted_rewards_t->transpose();

		// Train network with rewards.
		neural_net.train (encoded_state_t, predicted_rewards_t, learning_rate, weight_decay);

		LOG(LINFO) << "Player position = " << player_pos_t << " Performed action = " << A_EXIT;
		LOG(LSTATUS) << std::endl << state.streamGrid();

		// Finish the episode.
		return false;
	}//: if terminal

	// Select the action.
	mic::types::NESWAction action;
	double eps = (double)epsilon;
	if ((double)epsilon < 0)
		eps = 1.0/(1.0+episode);
	LOG(LDEBUG) << "eps = " << eps;
	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > eps){
		// Select best action.
		action = selectBestAction();
	} else {
		// Random action.
		action = A_RANDOM;
	}//: if

	// Execute action - until success.
	while (!move(action)) {
		// If action could not be performed - random.
		action = A_RANDOM;
	}//: while

	// Get new state s(t+1).
	mic::types::Position2D state_t_prim = state.getPlayerPosition();

	LOG(LINFO) << "Player position = " << player_pos_t<< " Performed action = " << action;

	// Update running average for given action - Deep Q learning!
	float* pred = predicted_rewards_t->data();
	float q_st_at = pred[(size_t)action.getType()];
	float r = step_reward;
	float max_q_st_prim_at_prim = computeBestValue();
	LOG(LDEBUG) << "q_st_at = " << q_st_at;
	LOG(LDEBUG) << "state_t_prim = " << state_t_prim;
	LOG(LDEBUG) << "step_reward = " << step_reward;
	LOG(LDEBUG) << "max_q_st_prim_at_prim = " << max_q_st_prim_at_prim;

	// If those values are finite.
	if (std::isfinite(q_st_at) && std::isfinite(max_q_st_prim_at_prim)) {
		// Set new value and train network.
		(*predicted_rewards_t)((size_t)action.getType(), 0) = r + discount_rate*max_q_st_prim_at_prim;
		LOG(LWARNING) << "Desired rewards: " << predicted_rewards_t->transpose();

		// Train network with rewards.
		neural_net.train (encoded_state_t, predicted_rewards_t, learning_rate, weight_decay);

	}//: if


	LOG(LSTATUS) << "The resulting state:" << std::endl << state.streamGrid();

	return true;
}


} /* namespace application */
} /* namespace mic */
