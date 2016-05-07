/*!
 * \file GridworldDRLExperienceReplayPOMDP.cpp
 * \brief 
 * \author tkornut
 * \date May 5, 2016
 */

#include <limits>
#include <data_utils/RandomGenerator.hpp>

#include <application/GridworldDRLExperienceReplayPOMDP.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::GridworldDRLExperienceReplayPOMDP);
}


GridworldDRLExperienceReplayPOMDP::GridworldDRLExperienceReplayPOMDP(std::string node_name_) : OpenGLEpisodicApplication(node_name_),
		step_reward("step_reward", 0.0),
		discount_rate("discount_rate", 0.9),
		learning_rate("learning_rate", 0.005),
		epsilon("epsilon", 0.1),
		step_limit("step_limit",0),
		statistics_filename("statistics_filename","drl_er_statistics.csv"),
		mlnn_filename("mlnn_filename", "drl_er_mlnn.txt"),
		mlnn_save("mlnn_save", false),
		mlnn_load("mlnn_load", false),
		experiences(10000,1)
	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(step_reward);
	registerProperty(discount_rate);
	registerProperty(learning_rate);
	registerProperty(epsilon);
	registerProperty(step_limit);
	registerProperty(statistics_filename);
	registerProperty(mlnn_filename);
	registerProperty(mlnn_save);
	registerProperty(mlnn_load);

	LOG(LINFO) << "Properties registered";
}


GridworldDRLExperienceReplayPOMDP::~GridworldDRLExperienceReplayPOMDP() {

}


void GridworldDRLExperienceReplayPOMDP::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
	collector_ptr->createContainer("number_of_steps",  mic::types::color_rgba(255, 0, 0, 180));
	collector_ptr->createContainer("number_of_steps_average", mic::types::color_rgba(255, 255, 0, 180));
	collector_ptr->createContainer("collected_reward", mic::types::color_rgba(0, 255, 0, 180));
	collector_ptr->createContainer("collected_reward_average", mic::types::color_rgba(0, 255, 255, 180));
	collector_ptr->createContainer("success_ratio",  mic::types::color_rgba(255, 255, 255, 180));

	sum_of_iterations = 0;
	sum_of_rewards = 0;
	number_of_successes = 0;

	// Create the visualization windows - must be created in the same, main thread :]
	w_chart = new WindowFloatCollectorChart("GridworldDRLExperienceReplayPOMDP", 256, 256, 0, 0);
	w_chart->setDataCollectorPtr(collector_ptr);

}

void GridworldDRLExperienceReplayPOMDP::initializePropertyDependentVariables() {
	// Hardcode batchsize - for fastening the display!
	batch_size = grid_env.getObservationWidth() * grid_env.getObservationHeight();

	// Try to load neural network from file.
	if ((mlnn_load) && (neural_net.load(mlnn_filename))) {
		// Do nothing ;)
	} else {
		// Create a simple neural network.
		// gridworld wxhx4 -> 100 -> 4 -> regression!.
		neural_net.addLayer(new Linear((size_t) grid_env.getObservationSize(), 250, batch_size));
		neural_net.addLayer(new ReLU(250, 250, batch_size));
		neural_net.addLayer(new Linear(250, 100, batch_size));
		neural_net.addLayer(new ReLU(100, 100, batch_size));
		neural_net.addLayer(new Linear(100, 4, batch_size));
		neural_net.addLayer(new Regression(4, 4, batch_size));
		LOG(LINFO) << "Generated new neural network";
	}//: else

	// Set batch size in experience replay memory.
	experiences.setBatchSize(batch_size);
}


void GridworldDRLExperienceReplayPOMDP::startNewEpisode() {
	LOG(LSTATUS) << "Starting new episode " << episode;

	// Generate the gridworld (and move player to initial position).
	grid_env.initializePropertyDependentVariables();

	LOG(LSTATUS) << "Network responses: \n" <<  streamNetworkResponseTable();
	LOG(LSTATUS) << "Observation: \n"  << grid_env.observationToString();
	LOG(LSTATUS) << "Environment: \n" << grid_env.environmentToString();
}


void GridworldDRLExperienceReplayPOMDP::finishCurrentEpisode() {
	LOG(LTRACE) << "End current episode";

	mic::types::Position2D current_position = grid_env.getAgentPosition();
	float reward = grid_env.getStateReward(current_position);
	sum_of_iterations += iteration;
	sum_of_rewards += reward;
	if (reward > 0)
			number_of_successes++;

	// Add variables to container.
	collector_ptr->addDataToContainer("number_of_steps",iteration);
	collector_ptr->addDataToContainer("number_of_steps_average",(float)sum_of_iterations/episode);
	collector_ptr->addDataToContainer("collected_reward", reward);
	collector_ptr->addDataToContainer("collected_reward_average", (float)sum_of_rewards/episode);
	collector_ptr->addDataToContainer("success_ratio", (float)number_of_successes/episode);


	// Export reward "convergence" diagram.
	collector_ptr->exportDataToCsv(statistics_filename);

	// Save nn to file.
	if (mlnn_save)
		neural_net.save(mlnn_filename);
}


std::string GridworldDRLExperienceReplayPOMDP::streamNetworkResponseTable() {
	LOG(LTRACE) << "streamNetworkResponseTable()";
	std::string rewards_table;
	std::string actions_table;

	// Remember the current state i.e. player position.
	mic::types::Position2D current_player_pos_t = grid_env.getAgentPosition();

	// Create new matrices for batches of inputs and targets.
	MatrixXfPtr inputs_batch(new MatrixXf(grid_env.getObservationSize(), batch_size));

	// Assume that the batch_size = grid_env.getWidth() * grid_env.getHeight()
	assert(grid_env.getObservationWidth()*grid_env.getObservationHeight() == batch_size);


	size_t dx = (grid_env.getObservationWidth()-1)/2;
	size_t dy = (grid_env.getObservationHeight()-1)/2;
	mic::types::Position2D p = grid_env.getAgentPosition();

	// Copy data.
	for (long oy=0, ey=(p.y-dy); oy<grid_env.getObservationHeight(); oy++, ey++){
		for (long ox=0, ex=(p.x-dx); ox<grid_env.getObservationWidth(); ox++, ex++) {

	//for (size_t y=0; y<grid_env.getObservationHeight(); y++){
//		for (size_t x=0; x<grid_env.getObservationWidth(); x++) {

			// Move the player to given state - disregarding whether it was successful or not, answers for walls/positions outside of the gridworld do not interes us anyway...
			if (!grid_env.moveAgentToPosition(Position2D(ex,ey)))
				LOG(LDEBUG) << "Failed!";
			// Encode the current state.
			mic::types::MatrixXfPtr encoded_state = grid_env.encodeObservation();
			// Add to batch.
			inputs_batch->col(oy*grid_env.getObservationWidth()+ox) = encoded_state->col(0);
		}//: for x
	}//: for y

	// Get rewards for the whole batch.
	neural_net.forward(*inputs_batch);
	// Get predictions for all those states - there is no need to create a copy.
	MatrixXfPtr predicted_batch = neural_net.getPredictions();


	rewards_table += "Action values:\n";
	actions_table += "Best actions:\n";
	// Generate all possible states and all possible rewards.
	for (long oy=0, ey=(p.y-dy); oy<grid_env.getObservationHeight(); oy++, ey++){
		rewards_table += "| ";
		actions_table += "| ";
		for (long ox=0, ex=(p.x-dx); ox<grid_env.getObservationWidth(); ox++, ex++) {
			float bestqval = -std::numeric_limits<float>::infinity();
			size_t best_action = -1;
			for (size_t a=0; a<4; a++) {
				float qval = (*predicted_batch)(a, oy*grid_env.getObservationWidth()+ox);

				rewards_table += std::to_string(qval);
				if (a==3)
					rewards_table += " | ";
				else
					rewards_table += " , ";

				// Remember the best value.
				if (grid_env.isStateAllowed(ex,ey) && (!grid_env.isStateTerminal(ex,ey)) && grid_env.isActionAllowed(ex,ey,a) && (qval > bestqval)){
					bestqval = qval;
					best_action = a;
				}//: if

			}//: for a(ctions)
			switch(best_action){
				case 0 : actions_table += "N | "; break;
				case 1 : actions_table += "E | "; break;
				case 2 : actions_table += "S | "; break;
				case 3 : actions_table += "W | "; break;
				default: actions_table += "- | ";
			}//: switch

		}//: for x
		rewards_table += "\n";
		actions_table += "\n";
	}//: for y

	// Move player to previous position.
	grid_env.moveAgentToPosition(current_player_pos_t);

	return rewards_table + actions_table;
}



float GridworldDRLExperienceReplayPOMDP::computeBestValueForGivenStateAndPredictions(mic::types::Position2D player_position_, float* predictions_){
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
		if(grid_env.isActionAllowed(player_position_, action)) {
			float qvalue = predictions_[(size_t)action.getType()];
			if (qvalue > best_qvalue)
				best_qvalue = qvalue;
		}//if is allowed
	}//: for

	return best_qvalue;
}


mic::types::MatrixXfPtr GridworldDRLExperienceReplayPOMDP::getPredictedRewardsForGivenState(mic::types::Position2D player_position_) {
	LOG(LTRACE) << "getPredictedRewardsForGivenState()";
	// Remember the current state i.e. player position.
	mic::types::Position2D current_player_pos_t = grid_env.getAgentPosition();

	// Move the player to given state.
	grid_env.moveAgentToPosition(player_position_);

	// Encode the current state.
	mic::types::MatrixXfPtr encoded_state = grid_env.encodeObservation();

	// Create NEW matrix for the inputs batch.
	MatrixXfPtr inputs_batch(new MatrixXf(grid_env.getObservationSize(), batch_size));
	inputs_batch->setZero();

	// Set the first input - only this one interests us.
	inputs_batch->col(0) = encoded_state->col(0);

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
	grid_env.moveAgentToPosition(current_player_pos_t);

	// Return the predictions.
	return predictions_sample;
}

mic::types::NESWAction GridworldDRLExperienceReplayPOMDP::selectBestActionForGivenState(mic::types::Position2D player_position_){
	LOG(LTRACE) << "selectBestAction";

	// Greedy methods - returns the index of element with greatest value.
	mic::types::NESWAction best_action = A_RANDOM;
    float best_qvalue = -std::numeric_limits<float>::infinity();

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the results of actions one by one... (there is no need to create a separate copy of predictions)
	MatrixXfPtr predictions_sample = getPredictedRewardsForGivenState(player_position_);
	//LOG(LERROR) << "Selecting action from predictions:\n" << predictions_sample->transpose();
	float* pred = predictions_sample->data();

	for(size_t a=0; a<4; a++) {
		// Find the best action allowed.
		if(grid_env.isActionAllowed(player_position_, mic::types::NESWAction((mic::types::NESW)a))) {
			float qvalue = pred[a];
			if (qvalue > best_qvalue){
				best_qvalue = qvalue;
				best_action.setAction((mic::types::NESW)a);
			}
		}//if is allowed
	}//: for

	return best_action;
}

bool GridworldDRLExperienceReplayPOMDP::performSingleStep() {
	LOG(LSTATUS) << "Episode "<< episode << ": step " << iteration << "";

	// TMP!
	double 	nn_weight_decay = 0;

	// Get player pos at time t.
	mic::types::Position2D player_pos_t= grid_env.getAgentPosition();
	LOG(LINFO) << "Agent position at state t: " << player_pos_t;

	// Select the action.
	mic::types::NESWAction action;
	//action = A_NORTH;
	double eps = (double)epsilon;
	if ((double)epsilon < 0)
		eps = 1.0/(1.0+sqrt(episode));
	if (eps < 0.1)
		eps = 0.1;
	LOG(LDEBUG) << "eps = " << eps;
	bool random = false;

	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > eps){
		// Select best action.
		action = selectBestActionForGivenState(player_pos_t);
	} else {
		// Random action.
		action = A_RANDOM;
		random = true;
	}//: if

	// Execute action - do not monitor the success.
	grid_env.moveAgent(action);

	// Get new state s(t+1).
	mic::types::Position2D player_pos_t_prim = grid_env.getAgentPosition();
	LOG(LINFO) << "Agent position at t+1: " << player_pos_t_prim << " after performing the action = " << action << ((random) ? " [Random]" : "");

	// Collect the experience.
	SpatialExperiencePtr exp(new SpatialExperience(player_pos_t, action, player_pos_t_prim));
	// Create an empty matrix for rewards - this will be recalculated each time the experience will be replayed anyway.
	MatrixXfPtr rewards (new MatrixXf(4 , batch_size));
	// Add experience to experience table.
	experiences.add(exp, rewards);


	// Deep Q learning - train network with random sample from the experience memory.
	if (experiences.size() >= 2*batch_size) {
		// Create new matrices for batches of inputs and targets.
		MatrixXfPtr inputs_t_batch(new MatrixXf(grid_env.getObservationSize(), batch_size));
		MatrixXfPtr inputs_t_prim_batch(new MatrixXf(grid_env.getObservationSize(), batch_size));
		MatrixXfPtr targets_t_batch(new MatrixXf(4, batch_size));

		// Get the random batch.
		SpatialExperienceBatch geb = experiences.getRandomBatch();

		// Debug purposes.
		geb.setNextSampleIndex(0);
		for (size_t i=0; i<batch_size; i++) {
			SpatialExperienceSample ges = geb.getNextSample();
			SpatialExperiencePtr ge_ptr = ges.data();
			LOG(LDEBUG) << "Training sample : " << ge_ptr->s_t << " -> " << ge_ptr->a_t << " -> " << ge_ptr->s_t_prim;
		}//: for

		// Iterate through samples and create inputs_t_batch.
		for (size_t i=0; i<batch_size; i++) {
			SpatialExperienceSample ges = geb.getNextSample();
			SpatialExperiencePtr ge_ptr = ges.data();

			// Replay the experience.
			// "Simulate" moving player to position from state/time (t).
			grid_env.moveAgentToPosition(ge_ptr->s_t);
			// Encode the state at time (t).
			mic::types::MatrixXfPtr encoded_state_t = grid_env.encodeObservation();
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
			SpatialExperienceSample ges = geb.getNextSample();
			SpatialExperiencePtr ge_ptr = ges.data();

			// Replay the experience.
			// "Simulate" moving player to position from state/time (t+1).
			grid_env.moveAgentToPosition(ge_ptr->s_t_prim);
			// Encode the state at time (t+1).
			mic::types::MatrixXfPtr encoded_state_t = grid_env.encodeObservation();
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
			SpatialExperienceSample ges = geb.getNextSample();
			SpatialExperiencePtr ge_ptr = ges.data();

			if (ge_ptr->s_t == ge_ptr->s_t_prim) {
				// The move was not possible! Learn that as well.
				(*targets_t_batch)((size_t)ge_ptr->a_t.getType(), i) = 3*step_reward;
			} else if(grid_env.isStateTerminal(ge_ptr->s_t_prim)) {
				// The position at (t+1) state appears to be terminal - learn the reward.
				(*targets_t_batch)((size_t)ge_ptr->a_t.getType(), i) = grid_env.getStateReward(ge_ptr->s_t_prim);
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

		LOG(LDEBUG) <<"Inputs batch:\n" << inputs_t_batch->transpose();
		LOG(LDEBUG) <<"Targets batch:\n" << targets_t_batch->transpose();

		// Perform the Deep-Q-learning.
		LOG(LDEBUG) << "Network responses before training:" << std::endl << streamNetworkResponseTable();

		// Train network with rewards.
		float loss = neural_net.train (inputs_t_batch, targets_t_batch, learning_rate, nn_weight_decay);
		LOG(LDEBUG) << "Training loss:" << loss;

		//LOG(LDEBUG) << "Network responses after training:" << std::endl << streamNetworkResponseTable();

		// Finish the replay: move the player to REAL, CURRENT POSITION.
		grid_env.moveAgentToPosition(player_pos_t_prim);
	}//: if enough experiences
	else
		LOG(LWARNING) << "Not enough samples in the experience replay memory!";

	LOG(LSTATUS) << "Network responses: \n" << streamNetworkResponseTable();
	LOG(LSTATUS) << "Observation: \n"  << grid_env.observationToString();
	LOG(LSTATUS) << "Environment: \n"  << grid_env.environmentToString();

	// Check whether state t+1 is terminal - finish the episode.
	if(grid_env.isStateTerminal(grid_env.getAgentPosition()))
		return false;

	// Check whether we reached maximum number of iterations.
	if ((step_limit>0) && (iteration >= step_limit))
		return false;


	return true;
}

} /* namespace application */
} /* namespace mic */
