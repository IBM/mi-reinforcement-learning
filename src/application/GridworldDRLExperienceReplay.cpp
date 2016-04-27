/*!
 * \file GridworldDRLExperienceReplay.cpp
 * \brief 
 * \author tkornut
 * \date Apr 26, 2016
 */

#include <limits>
#include <data_utils/RandomGenerator.hpp>
#include <application/GridworldDRLExperienceReplay.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::GridworldDRLExperienceReplay);
}


GridworldDRLExperienceReplay::GridworldDRLExperienceReplay(std::string node_name_) : OpenGLEpisodicApplication(node_name_),
		gridworld_type("gridworld_type", 0),
		width("width", 4),
		height("height", 4),
		batch_size("batch_size",1),
		step_reward("step_reward", 0.0),
		discount_rate("discount_rate", 0.9),
		learning_rate("learning_rate", 0.005),
		epsilon("epsilon", 0.1),
		statistics_filename("statistics_filename","statistics_filename.csv"),
		experiences(10000,1)
	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(gridworld_type);
	registerProperty(width);
	registerProperty(height);
	registerProperty(batch_size);
	registerProperty(step_reward);
	registerProperty(discount_rate);
	registerProperty(learning_rate);
	registerProperty(epsilon);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


GridworldDRLExperienceReplay::~GridworldDRLExperienceReplay() {

}


void GridworldDRLExperienceReplay::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
	collector_ptr->createContainer("number_of_steps",  mic::types::color_rgba(255, 0, 0, 180));
	collector_ptr->createContainer("average_number_of_steps", mic::types::color_rgba(0, 255, 0, 180));
	collector_ptr->createContainer("collected_reward", mic::types::color_rgba(0, 0, 255, 180));

	sum_of_iterations = 0;

	// Create the visualization windows - must be created in the same, main thread :]
	w_chart = new WindowFloatCollectorChart("GridworldDRLExperienceReplay", 256, 256, 0, 0);
	w_chart->setDataCollectorPtr(collector_ptr);

}

void GridworldDRLExperienceReplay::initializePropertyDependentVariables() {
	// Generate the gridworld.
	state.generateGridworld(gridworld_type, width, height);

	// Get width and height.
	width = state.getWidth();
	height = state.getHeight();

	// Create a simple neural network.
	// gridworld wxhx4 -> 100 -> 4 -> regression!; batch size is set to one.
	neural_net.addLayer(new Linear((size_t) width * height , 250, batch_size));
	neural_net.addLayer(new ReLU(250, 250, batch_size));
	neural_net.addLayer(new Linear(250, 100, batch_size));
	neural_net.addLayer(new ReLU(100, 100, batch_size));
	neural_net.addLayer(new Linear(100, 4, batch_size));
	neural_net.addLayer(new Regression(4, 4, batch_size));

	// Set batch size in experience replay memory.
	experiences.setBatchSize(batch_size);
}


void GridworldDRLExperienceReplay::startNewEpisode() {
	LOG(LERROR) << "Start new episode";
	// Move player to start position.
	state.movePlayerToInitialPosition();

	LOG(LSTATUS) << "Network responses:" << std::endl << streamNetworkResponseTable();
	LOG(LSTATUS) << std::endl << state.streamGrid();

}


void GridworldDRLExperienceReplay::finishCurrentEpisode() {
	LOG(LTRACE) << "End current episode";

	sum_of_iterations += iteration;

	// Add variables to container.
	collector_ptr->addDataToContainer("number_of_steps",iteration);
	collector_ptr->addDataToContainer("average_number_of_steps",(float)sum_of_iterations/episode);
	collector_ptr->addDataToContainer("collected_reward",state.getStateReward(state.getPlayerPosition()));

/*		// Export reward "convergence" diagram.
		collector_ptr->exportDataToCsv(statistics_filename);*/

}


bool GridworldDRLExperienceReplay::move (mic::types::Action2DInterface ac_) {
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


std::string GridworldDRLExperienceReplay::streamNetworkResponseTable() {
	LOG(LTRACE) << "streamNetworkResponseTable()";
	std::ostringstream os;
	// Make a copy of current gridworld.
	Gridworld tmp_grid = state;
	MatrixXf best_vals (height, width);
	best_vals.setValue(-std::numeric_limits<float>::infinity());

	os << "All rewards:\n";
	// Generate all possible states and all possible rewards.
	for (size_t y=0; y<height; y++){
		os << "| ";
		for (size_t x=0; x<width; x++) {
			// Check network response for given state.
			// Get the first prediction only.
			MatrixXfPtr predictions = getPredictedRewardsForGivenState(Position2D(x,y));
			//LOG(LWARNING) << "Received predictions:\n" << predictions->transpose();
			float*  qstate = predictions->data();

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
/*	os << std::endl;

	os << "Best rewards:\n";
	// Stream only the biggerst states.
	for (size_t y=0; y<height; y++){
		os << "| ";
		for (size_t x=0; x<width; x++) {
			os << best_vals(y,x) << " | ";
		}//: for x
		os << std::endl;
	}//: for y*/

	return os.str();
}



float GridworldDRLExperienceReplay::computeBestValueForGivenState(mic::types::Position2D player_position_){
	LOG(LTRACE) << "computeBestValueForGivenState()";

	// Check the results of actions one by one... (there is no need to create a separate copy of predictions)
	float* pred = getPredictedRewardsForGivenState(player_position_)->data();

	return computeBestValueForGivenStateAndPredictions(player_position_, pred);
}


float GridworldDRLExperienceReplay::computeBestValueForGivenStateAndPredictions(mic::types::Position2D player_position_, float* predictions_){
	LOG(LTRACE) << "computeBestValueForGivenState()";
	float best_qvalue = -std::numeric_limits<float>::infinity();

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	for(mic::types::NESWAction action : actions) {
		// .. and find the value of teh best allowed action.
		if(state.isActionAllowed(player_position_, action)) {
			float qvalue = predictions_[(size_t)action.getType()];
			if (qvalue > best_qvalue)
				best_qvalue = qvalue;
		}//if is allowed
	}//: for

	return best_qvalue;
}


mic::types::MatrixXfPtr GridworldDRLExperienceReplay::getPredictedRewardsForGivenState(mic::types::Position2D player_position_) {
	LOG(LTRACE) << "getPredictedRewardsForGivenState()";
	// Remember the current state i.e. player position.
	mic::types::Position2D current_player_pos_t = state.getPlayerPosition();

	// Move the player to giveh state.
	state.movePlayerToPosition(player_position_);

	// Encode the current state.
	mic::types::MatrixXfPtr encoded_state = state.encodeGrid();

	// Create NEW matrix for the inputs batch.
	MatrixXfPtr inputs_batch(new MatrixXf((size_t) width * height, batch_size));

	// Iterate through samples and copy the same inputs n times.
	for (size_t i=0; i<batch_size; i++)
		inputs_batch->col(i) = encoded_state->col(0);

	//LOG(LERROR) << "Getting predictions for input batch:\n" <<inputs_batch->transpose();

	// Pass the data and get predictions.
	neural_net.forward(*inputs_batch);

	MatrixXfPtr predictions_batch = neural_net.getPredictions();

	//LOG(LERROR) << "Resulting predictions batch:\n" << predictions_batch->transpose();

	// Get the first prediction only.
	MatrixXfPtr predictions_sample(new MatrixXf(4, 1));
	predictions_sample->col(0) = predictions_batch->col(0);

	//LOG(LERROR) << "Returned predictions sample:\n" << predictions_sample->transpose();

	// Move player to previous position.
	state.movePlayerToPosition(current_player_pos_t);

	// Return the predictions.
	return predictions_sample;
}

mic::types::NESWAction GridworldDRLExperienceReplay::selectBestActionForGivenState(mic::types::Position2D player_position_){
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
	float* pred = getPredictedRewardsForGivenState(player_position_)->data();

	for(mic::types::NESWAction action : actions) {
		// ... and find the best allowed.
		if(state.isActionAllowed(player_position_, action)) {
			float qvalue = pred[(size_t)action.getType()];
			if (qvalue > best_qvalue){
				best_qvalue = qvalue;
				best_action = action;
			}
		}//if is allowed
	}//: for

	return best_action;
}

bool GridworldDRLExperienceReplay::performSingleStep() {
	LOG(LERROR) << "Episode "<< episode << ": step " << iteration << "";

	// TMP!
	double 	nn_weight_decay = 0;

	// Get player pos at time t.
	mic::types::Position2D player_pos_t= state.getPlayerPosition();
	LOG(LINFO) << "Player position at state t: " << player_pos_t;

	// Select the action.
	mic::types::NESWAction action;
	//action = A_NORTH;
	double eps = (double)epsilon;
	if ((double)epsilon < 0)
		eps = 1.0/(1.0+sqrt(episode));
	if (eps < 0.1)
		eps = 0.1;

	LOG(LDEBUG) << "eps = " << eps;
	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > eps){
		// Select best action.
		action = selectBestActionForGivenState(player_pos_t);
	} else {
		// Random action.
		action = A_RANDOM;
	}//: if

	// Execute action - do not monitor the success.
	move(action);

	// Get new state s(t+1).
	mic::types::Position2D player_pos_t_prim = state.getPlayerPosition();
	LOG(LINFO) << "Player position at t+1: " << player_pos_t_prim << " after performing the action = " << action << " action index=" << (size_t)action.getType();

	// Collect the experience.
	GridworldExperiencePtr exp(new GridworldExperience(player_pos_t, action, player_pos_t_prim));
	// Create an empty matrix for rewards - this will be recalculated each time the experience will be replayed anyway.
	MatrixXfPtr rewards (new MatrixXf(width * height , batch_size));
	// Add experience to experience table.
	experiences.add(exp, rewards);


	// Deep Q learning - train network with random sample from the experience memory.
	if (experiences.size() >= 2*batch_size) {
		// Create new matrices for batches of inputs and targets.
		MatrixXfPtr inputs_t_batch(new MatrixXf((size_t) width * height, batch_size));
		MatrixXfPtr inputs_t_prim_batch(new MatrixXf((size_t) width * height, batch_size));
		MatrixXfPtr targets_t_batch(new MatrixXf(4, batch_size));

		// Get the random batch.
		GridworldExperienceBatch geb = experiences.getRandomBatch();

		// Debug purposes.
		geb.setNextSampleIndex(0);
		for (size_t i=0; i<batch_size; i++) {
			GridworldExperienceSample ges = geb.getNextSample();
			GridworldExperiencePtr ge_ptr = ges.data();
			LOG(LERROR) << "Training sample : " << ge_ptr->s_t << " -> " << ge_ptr->a_t << " -> " << ge_ptr->s_t_prim;
		}//: for

		// Iterate through samples and create inputs_t_batch.
		for (size_t i=0; i<batch_size; i++) {
			GridworldExperienceSample ges = geb.getNextSample();
			GridworldExperiencePtr ge_ptr = ges.data();

			// Replay the experience.
			// "Simulate" moving player to position from state/time (t).
			state.movePlayerToPosition(ge_ptr->s_t);
			// Encode the state at time (t).
			mic::types::MatrixXfPtr encoded_state_t = state.encodeGrid();
			//float* state = encoded_state_t->data();

			// Copy the encoded state to inputs batch.
			inputs_t_batch->col(i) = encoded_state_t->col(0);
		}// for samples.

		// Get network responses.
		neural_net.forward(*inputs_t_batch);
		// Get predictions for all those states...
		MatrixXfPtr predictions_t_batch = neural_net.getPredictions();
		// ... and copy them to reward pointer - a container which we will modify.
		(*targets_t_batch) = (*predictions_t_batch);

		// Iterate through samples and create inputs_t_prim_batch.
		geb.setNextSampleIndex(0);
		for (size_t i=0; i<batch_size; i++) {
			GridworldExperienceSample ges = geb.getNextSample();
			GridworldExperiencePtr ge_ptr = ges.data();

			// Replay the experience.
			// "Simulate" moving player to position from state/time (t+1).
			state.movePlayerToPosition(ge_ptr->s_t_prim);
			// Encode the state at time (t+1).
			mic::types::MatrixXfPtr encoded_state_t = state.encodeGrid();
			//float* state = encoded_state_t->data();

			// Copy the encoded state to inputs batch.
			inputs_t_prim_batch->col(i) = encoded_state_t->col(0);
		}// for samples.

		// Get network responses.
		neural_net.forward(*inputs_t_prim_batch);
		// Get predictions for all those states...
		MatrixXfPtr predictions_t_prim_batch = neural_net.getPredictions();


		// Calculate the rewards, one by one.
		// Iterate through samples and create inputs_t_prim_batch.
		geb.setNextSampleIndex(0);
		for (size_t i=0; i<batch_size; i++) {
			GridworldExperienceSample ges = geb.getNextSample();
			GridworldExperiencePtr ge_ptr = ges.data();

			/*if (ge_ptr->s_t == ge_ptr->s_t_prim) {
				// The move was not possible! Learn that as well.
				(*targets_t_batch)((size_t)ge_ptr->a_t.getType(), i) = -0.1;
			} else*/
			if(state.isStateTerminal(ge_ptr->s_t_prim)) {
				// The position at (t+1) state appears to be terminal - learn the reward.
				(*targets_t_batch)((size_t)ge_ptr->a_t.getType(), i) = state.getStateReward(ge_ptr->s_t_prim);
			} else {
				MatrixXfPtr preds_t_prim (new MatrixXf(4, 1));
				preds_t_prim->col(0) = predictions_t_prim_batch->col(i);
				// Get best value for the NEXT state - position from (t+1) state.
				float max_q_st_prim_at_prim = computeBestValueForGivenStateAndPredictions(ge_ptr->s_t_prim, preds_t_prim->data());
				// If next state best value is finite.
				// Update running average for given action - Deep Q learning!
				if (std::isfinite(max_q_st_prim_at_prim))
					(*targets_t_batch)((size_t)ge_ptr->a_t.getType(), i) = step_reward + discount_rate*max_q_st_prim_at_prim;
			}//: else

		}//: for

		LOG(LWARNING) <<"Inputs batch:\n" << inputs_t_batch->transpose();
		LOG(LWARNING) <<"Targets batch:\n" << targets_t_batch->transpose();

		// Perform the Deep-Q-learning.
		LOG(LSTATUS) << "Network responses before training:" << std::endl << streamNetworkResponseTable();

		// Train network with rewards.
		float loss = neural_net.train (inputs_t_batch, targets_t_batch, learning_rate, nn_weight_decay);
		LOG(LSTATUS) << "Training loss:" << loss;

		//LOG(LSTATUS) << "Network responses after training:" << std::endl << streamNetworkResponseTable();

		// Finish the replay: move the player to REAL, CURRENT POSITION.
		state.movePlayerToPosition(player_pos_t_prim);
	}//: if enough experiences
	else
		LOG(LWARNING) << "Not enough samples in the experience replay memory!";

	LOG(LSTATUS) << "Network responses:" << std::endl << streamNetworkResponseTable();
	LOG(LSTATUS) << std::endl << state.streamGrid();

	// Check whether state t+1 is terminal - finish the episode.
	if(state.isStateTerminal(state.getPlayerPosition()))
		return false;

	return true;
}


} /* namespace encoders */
} /* namespace mic */
