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
		step_reward("step_reward", 0.0),
		discount_rate("discount_rate", 0.9),
		learning_rate("learning_rate", 0.1),
		move_noise("move_noise",0.2),
		epsilon("epsilon", 0.1),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(step_reward);
	registerProperty(discount_rate);
	registerProperty(learning_rate);
	registerProperty(move_noise);
	registerProperty(epsilon);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


GridworldQLearning::~GridworldQLearning() {
	delete(w_chart);
}


void GridworldQLearning::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
	collector_ptr->createContainer("number_of_steps",  mic::types::color_rgba(255, 0, 0, 180));
	collector_ptr->createContainer("average_number_of_steps", mic::types::color_rgba(255, 255, 0, 180));
	collector_ptr->createContainer("collected_reward", mic::types::color_rgba(0, 255, 0, 180));
	collector_ptr->createContainer("average_collected_reward", mic::types::color_rgba(0, 255, 255, 180));

	sum_of_iterations = 0;
	sum_of_rewards = 0;

	// Create the visualization windows - must be created in the same, main thread :]
	w_chart = new WindowCollectorChart<float>("GridworldQLearning", 256, 256, 0, 0);
	w_chart->setDataCollectorPtr(collector_ptr);

}

void GridworldQLearning::initializePropertyDependentVariables() {
	// Initialize the gridworld.
	grid_env.initializeEnvironment();

	// Resize and reset the action-value table.
	qstate_table.resize({grid_env.getEnvironmentWidth(),grid_env.getEnvironmentHeight(),4});
	qstate_table.zeros();
	//qstate_table.setValue( -std::numeric_limits<float>::infinity() );

	LOG(LSTATUS) << std::endl << streamQStateTable();
}


void GridworldQLearning::startNewEpisode() {
	LOG(LSTATUS) << "Starting new episode " << episode;

	// Generate the gridworld (and move player to initial position).
	grid_env.initializeEnvironment();

	LOG(LSTATUS) << std::endl << streamQStateTable();
	LOG(LSTATUS) << std::endl << grid_env.environmentToString();

}


void GridworldQLearning::finishCurrentEpisode() {
	LOG(LTRACE) << "End current episode";

	float reward = grid_env.getStateReward(grid_env.getAgentPosition());
	sum_of_iterations += iteration;
	sum_of_rewards += reward;

	// Add variables to container.
	collector_ptr->addDataToContainer("number_of_steps",iteration);
	collector_ptr->addDataToContainer("average_number_of_steps",(float)sum_of_iterations/episode);
	collector_ptr->addDataToContainer("collected_reward", reward);
	collector_ptr->addDataToContainer("average_collected_reward", (float)sum_of_rewards/episode);

	// Export reward "convergence" diagram.
	collector_ptr->exportDataToCsv(statistics_filename);

}



std::string GridworldQLearning::streamQStateTable() {
	std::string rewards_table;
	std::string actions_table;

	rewards_table += "Action values:\n";
	actions_table += "Best actions:\n";
	for (size_t y=0; y<grid_env.getEnvironmentHeight(); y++){
		rewards_table += "| ";
		actions_table += "| ";
		for (size_t x=0; x<grid_env.getEnvironmentWidth(); x++) {
			// Iterate through actions and find the best one.
			float bestqval = -std::numeric_limits<float>::infinity();
			size_t best_action = -1;
			for (size_t a=0; a<4; a++) {
				float qval = qstate_table({x,y,a});
				if ( qstate_table({x,y,a}) == -std::numeric_limits<float>::infinity())
					rewards_table += "-INF";
				else
					rewards_table += std::to_string(qval);
				if (a==3)
					rewards_table += " | ";
				else
					rewards_table += " , ";

				// Remember the best value.
				if (grid_env.isStateAllowed(x,y) && (!grid_env.isStateTerminal(x,y)) && grid_env.isActionAllowed(x,y,a) && (qval > bestqval)){
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

	return rewards_table + actions_table;
}



float GridworldQLearning::computeBestValue(mic::types::Position2D pos_){
	float qbest_value = -std::numeric_limits<float>::infinity();
	// Check if the state is allowed.
	if (!grid_env.isStateAllowed(pos_))
		return qbest_value;

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the actions one by one.
	for(mic::types::NESWAction action : actions) {
		if(grid_env.isActionAllowed(pos_, action)) {
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
    float best_qvalue = -std::numeric_limits<float>::infinity();

	// Create a list of possible actions.
	std::vector<mic::types::NESWAction> actions;
	actions.push_back(A_NORTH);
	actions.push_back(A_EAST);
	actions.push_back(A_SOUTH);
	actions.push_back(A_WEST);

	// Check the actions one by one.
	for(mic::types::NESWAction action : actions) {
		if(grid_env.isActionAllowed(pos_, action)) {
			float qvalue = qstate_table({(size_t)pos_.x, (size_t)pos_.y, (size_t)action.getType()});
			std::cout << "  qvalue = " << qvalue << std::endl;
			if (qvalue > best_qvalue){
				best_qvalue = qvalue;
				best_action = action;
				std::cout << "  best_qvalue = " << best_qvalue << std::endl;
			}
		}//if is allowed
	}//: for

	return best_action;
}

bool GridworldQLearning::performSingleStep() {
	LOG(LSTATUS) << "Episode "<< episode << ": step " << iteration << "";

	// Get state s(t).
	mic::types::Position2D agent_pos_t = grid_env.getAgentPosition();

	// Check whether state is terminal.
	if(grid_env.isStateTerminal(agent_pos_t)) {
		// In the terminal state we can select only one special action: "terminate".
		// All "other" actions receive the same value related to the "reward".
		float final_reward = grid_env.getStateReward(agent_pos_t);
		for (size_t a=0; a<4; a++)
			qstate_table({(size_t)agent_pos_t.x,(size_t)agent_pos_t.y, a}) = final_reward;

		LOG(LINFO) << "Agent action = " << A_EXIT;
		LOG(LDEBUG) << "Agent position = " << agent_pos_t;
		LOG(LSTATUS) << std::endl << grid_env.environmentToString();
		LOG(LSTATUS) << std::endl << streamQStateTable();

		// Finish the episode.
		return false;
	}//: if terminal


	mic::types::NESWAction action;
	double eps = (double)epsilon;
	if ((double)epsilon < 0)
		eps = 1.0/(1.0+episode);
	LOG(LDEBUG) << "eps =" << eps;
	bool random = false;

	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > eps){
		// Select best action.
		action = selectBestAction(agent_pos_t);
		// If action could not be found.
		if (action.getType() == mic::types::NESW::None){
			action = A_RANDOM;
			random = true;
		}
	} else {
		// Random move - repeat until an allowed move is made.
		action = A_RANDOM;
		random = true;
	}//: if

	LOG(LINFO) << action  << action << ((random) ? " [Random]" : "");

	// Execture action - until success.
	grid_env.moveAgent(action);
/*	while (!move(action)) {
		// If action could not be performed - random.
		action = A_RANDOM;
		random = true;
	}//: while*/

	// Get new state s(t+1).
	mic::types::Position2D agent_pos_t_prim = grid_env.getAgentPosition();

	LOG(LINFO) << "Agent position at t+1: " << agent_pos_t_prim << " after performing the action = " << action << ((random) ? " [Random]" : "");


	// Update running average for given action - Q learning;)
	float q_st_at = qstate_table({(size_t)agent_pos_t.x, (size_t)agent_pos_t.y, (size_t)action.getType()});
	float r = step_reward;
	float max_q_st_prim_at_prim = computeBestValue(agent_pos_t_prim);
	LOG(LDEBUG) << "q_st_at = " << q_st_at;
	LOG(LDEBUG) << "agent_t_prim = " << agent_pos_t_prim;
	LOG(LDEBUG) << "step_reward = " << step_reward;
	LOG(LDEBUG) << "max_q_st_prim_at_prim = " << max_q_st_prim_at_prim;
	//if (std::isfinite(q_st_at) && std::isfinite(max_q_st_prim_at_prim))
	if (agent_pos_t == agent_pos_t_prim)
		qstate_table({(size_t)agent_pos_t.x, (size_t)agent_pos_t.y, (size_t)action.getType()}) = q_st_at + learning_rate * (2*r + discount_rate*max_q_st_prim_at_prim - q_st_at);
	else
		qstate_table({(size_t)agent_pos_t.x, (size_t)agent_pos_t.y, (size_t)action.getType()}) = q_st_at + learning_rate * (r + discount_rate*max_q_st_prim_at_prim - q_st_at);

	LOG(LSTATUS) << std::endl << streamQStateTable();
	LOG(LSTATUS) << std::endl << grid_env.environmentToString();

	return true;
}


} /* namespace application */
} /* namespace mic */
