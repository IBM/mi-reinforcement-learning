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


GridworldDeepQLearning::~GridworldDeepQLearning() {

}


void GridworldDeepQLearning::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
	collector_ptr->createContainer("iteration_number",  mic::types::color_rgba(255, 0, 0, 180));
	collector_ptr->createContainer("average_iteration_number", mic::types::color_rgba(0, 255, 0, 180));
	collector_ptr->createContainer("collected_reward", mic::types::color_rgba(0, 0, 255, 180));

	sum_of_iterations = 0;

	// Create the visualization windows - must be created in the same, main thread :]
	w_chart = new WindowFloatCollectorChart("GridworldDeepQLearning", 256, 256, 0, 0);
	w_chart->setDataCollectorPtr(collector_ptr);

}

void GridworldDeepQLearning::initializePropertyDependentVariables() {
	// Generate the gridworld.
	state.generateGridworld(gridworld_type, width, height);

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
	neural_net.addLayer(new Regression(4, 4, 1));
}


void GridworldDeepQLearning::startNewEpisode() {
	LOG(LERROR) << "Start new episode";
	// Move player to start position.
	state.movePlayerToInitialPosition();
	LOG(LSTATUS) << "Network responses:" << std::endl << streamNetworkResponseTable();
	LOG(LSTATUS) << std::endl << state.streamGrid();

}


void GridworldDeepQLearning::finishCurrentEpisode() {
	LOG(LTRACE) << "End current episode";

	sum_of_iterations += iteration;

	// Add variables to container.
	collector_ptr->addDataToContainer("iteration_number",iteration);
	collector_ptr->addDataToContainer("average_iteration_number",(float)sum_of_iterations/episode);
	collector_ptr->addDataToContainer("collected_reward",state.getStateReward(state.getPlayerPosition()));

/*		// Export reward "convergence" diagram.
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


std::string GridworldDeepQLearning::streamNetworkResponseTable() {
	std::ostringstream os;
	// Make a copy of current gridworld.
	Gridworld tmp_grid = state;
	MatrixXf best_vals (height, width);
	best_vals.setValue(-std::numeric_limits<float>::infinity());

	os << "All rewards:";
	// Generate all possible states and all possible rewards.
	for (size_t y=0; y<height; y++){
		os << "| ";
		for (size_t x=0; x<width; x++) {
			// Check network response for given state.
			tmp_grid.movePlayerToPosition(Position2D(x,y));
			mic::types::MatrixXfPtr tmp_state = tmp_grid.encodeGrid();
			// Pass the data and get predictions.
			neural_net.forward(*tmp_state);
			mic::types::MatrixXfPtr tmp_predicted_rewards = neural_net.getPredictions();
			float*  qstate = tmp_predicted_rewards->data();

			for (size_t a=0; a<4; a++) {
				os << qstate[a];
				if (a==3)
					os << " | ";
				else
					os << " , ";
				// Remember the best value.
				if (qstate[a] >= best_vals(y,x))
					best_vals(y,x) = qstate[a];

			}//: for a(ctions)
		}//: for x
		os << std::endl;
	}//: for y
	os << std::endl;

	os << "Best rewards:";
	// Stream only the biggerst states.
	for (size_t y=0; y<height; y++){
		os << "| ";
		for (size_t x=0; x<width; x++) {
			os << best_vals(y,x) << " | ";
		}//: for x
		os << std::endl;
	}//: for y

	return os.str();
}



float GridworldDeepQLearning::computeBestValueForCurrentState(){
	LOG(LTRACE) << "computeBestValue";
	float best_qvalue = -std::numeric_limits<float>::infinity();

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the results of actions one by one... (there is no need to create a separate copy of predictions)
	float* pred = getPredictedRewardsForCurrentState()->data();

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

mic::types::MatrixXfPtr GridworldDeepQLearning::getPredictedRewardsForCurrentState() {
	// Encode the current state.
	mic::types::MatrixXfPtr encoded_state = state.encodeGrid();
	// Pass the data and get predictions.
	neural_net.forward(*encoded_state);
	// Return the predictions.
	return neural_net.getPredictions();
}

mic::types::NESWAction GridworldDeepQLearning::selectBestActionForCurrentState(){
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

	// Check the results of actions one by one... (there is no need to create a separate copy of predictions)
	float* pred = getPredictedRewardsForCurrentState()->data();

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
	LOG(LERROR) << "Performing a single step (" << iteration << ")";

	// TMP!
	double 	nn_learning_rate = 0.005;
	double 	nn_weight_decay = 0;

	// Get player pos at time t.
	mic::types::Position2D player_pos_t= state.getPlayerPosition();

	// Encode the current state at time t.
	mic::types::MatrixXfPtr encoded_state_t = state.encodeGrid();

	// Get the prediced rewards at time t...
	MatrixXfPtr tmp_rewards_t = getPredictedRewardsForCurrentState();
	// ... but make a local copy!
	MatrixXfPtr predicted_rewards_t (new MatrixXf(*tmp_rewards_t));
	LOG(LINFO) << "Player position at state t: " << player_pos_t;
	LOG(LSTATUS) << "Predicted rewards for state t: " << predicted_rewards_t->transpose();

/*	// Check whether state is terminal.
	if(state.isStateTerminal(player_pos_t)) {
		// In the terminal state we can select only one special action: "terminate".
		// All "other" actions receive the same value related to the "reward".
		float final_reward = state.getStateReward(player_pos_t);
		// Create a vector of rewards and train the network.
		for (size_t a=0; a<4; a++)
			(*predicted_rewards_t)(a, 0) = final_reward;

		LOG(LINFO) << "Player position at t+1:" << player_pos_t<< " and the performed action = " << A_EXIT;
		LOG(LSTATUS) << "The resulting state:" << std::endl << state.streamGrid();


		LOG(LERROR) << "Training with desired rewards: " << predicted_rewards_t->transpose();
		LOG(LSTATUS) << "Network responses before training:" << std::endl << streamNetworkResponseTable();

		// Train network with rewards.
		float loss = neural_net.train (encoded_state_t, predicted_rewards_t, nn_learning_rate, nn_weight_decay);

		LOG(LSTATUS) << "Network responses after training:" << std::endl << streamNetworkResponseTable();

		// Finish the episode.
		return false;
	}//: if terminal*/


	// Select the action.
	mic::types::NESWAction action;
	//action = A_NORTH;
	double eps = (double)epsilon;
	if ((double)epsilon < 0)
		eps = 1.0/(1.0+episode);
	LOG(LDEBUG) << "eps = " << eps;
	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > eps){
		// Select best action.
		action = selectBestActionForCurrentState();
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
	mic::types::Position2D player_pos_t_prim = state.getPlayerPosition();

	LOG(LINFO) << "Player position at t+1: " << player_pos_t_prim << " after performing the action = " << action << " action index=" << (size_t)action.getType();

	// Update running average for given action - Deep Q learning!
	float r = step_reward;
	// Get best value for the NEXT state (!)
	float max_q_st_prim_at_prim = computeBestValueForCurrentState();

	LOG(LWARNING) << "step_reward = " << step_reward;
	LOG(LWARNING) << "max_q_st_prim_at_prim = " << max_q_st_prim_at_prim;

	// If those values are finite.
	if (std::isfinite(max_q_st_prim_at_prim)) {

		// Check whether state t+1 is terminal.
		if(state.isStateTerminal(player_pos_t_prim))
			(*predicted_rewards_t)((size_t)action.getType(), 0) = 100 * state.getStateReward(player_pos_t_prim);
		else
			(*predicted_rewards_t)((size_t)action.getType(), 0) = r + discount_rate*max_q_st_prim_at_prim;


		LOG(LERROR) << "Training with desired rewards: " << predicted_rewards_t->transpose();
		LOG(LSTATUS) << "Network responses before training:" << std::endl << streamNetworkResponseTable();

		// Train network with rewards.
		float loss = neural_net.train (encoded_state_t, predicted_rewards_t, nn_learning_rate, nn_weight_decay);

		LOG(LSTATUS) << "Network responses after training:" << std::endl << streamNetworkResponseTable();

	}//: if

	//LOG(LSTATUS) << "Network responses:" << std::endl << streamNetworkResponseTable();
	LOG(LSTATUS) << "The resulting state:" << std::endl << state.streamGrid();

	// Check whether state t+1 is terminal - finish the episode.
	if(state.isStateTerminal(player_pos_t_prim))
		return false;

	return true;
}


} /* namespace application */
} /* namespace mic */
