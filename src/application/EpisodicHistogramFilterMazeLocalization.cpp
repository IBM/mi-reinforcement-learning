/*!
 * \file EpisodicEpisodicHistogramFilterMazeLocalization.cpp
 * \brief 
 * \author tkornut
 * \date Feb 19, 2016
 */

#include <application/EpisodicHistogramFilterMazeLocalization.hpp>
#include <application/ApplicationFactory.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::EpisodicHistogramFilterMazeLocalization);
}


EpisodicHistogramFilterMazeLocalization::EpisodicHistogramFilterMazeLocalization(std::string node_name_) : OpenGLEpisodicApplication(node_name_),
		hidden_maze_number("hidden_maze", 0),
		hidden_x("hidden_x", 0),
		hidden_y("hidden_y", 0),
		action("action", -1),
		epsilon("epsilon", 0.0),
		hit_factor("hit_factor", 0.6),
		miss_factor("miss_factor", 0.2),
		exact_move_probability("exact_move_probability", 1.0),
		overshoot_move_probability("overshoot_move_probability", 0.0),
		undershoot_move_probability("undershoot_move_probability", 0.0),
		max_number_of_iterations("max_number_of_iterations",100),
		min_maze_confidence("min_maze_confidence",0.99)
	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(hidden_maze_number);
	registerProperty(hidden_x);
	registerProperty(hidden_y);

	registerProperty(action);
	registerProperty(epsilon);

	registerProperty(hit_factor);
	registerProperty(miss_factor);
	registerProperty(exact_move_probability);
	registerProperty(overshoot_move_probability);
	registerProperty(undershoot_move_probability);

	registerProperty(max_number_of_iterations);
	registerProperty(min_maze_confidence);

	LOG(LINFO) << "Properties registered";
}


EpisodicHistogramFilterMazeLocalization::~EpisodicHistogramFilterMazeLocalization() {

}


void EpisodicHistogramFilterMazeLocalization::initialize(int argc, char* argv[]) {
	LOG(LSTATUS) << "In here you should initialize Glut and create all OpenGL windows";

	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	// Create the visualization windows - must be created in the same, main thread :]
	w_localization_time_chart = new WindowChart("Current_maze", 256, 256, 0, 0);
}

void EpisodicHistogramFilterMazeLocalization::initializePropertyDependentVariables() {

	// Import mazes.
	if ((!importer.importData()) || (importer.getData().size() == 0)){
		LOG(LERROR) << "The dataset must consists of at least one maze!";
		exit(0);
	}//: if

	// Show mazes.
	LOG(LNOTICE) << "Loaded mazes";
	for (size_t m=0; m<importer.getData().size(); m++) {
		// Display results.
		LOG(LNOTICE) << "maze(" <<m<<"):\n" << (importer.getData()[m]);
	}//: for


	hf.setMazes(importer.getData(), 10);


	// Add containers to chart.
	w_localization_time_chart->createDataContainer("Iteration", mic::types::color_rgba(255, 0, 0, 180));
	w_localization_time_chart->createDataContainer("Converged", mic::types::color_rgba(0, 255, 0, 180));
	w_localization_time_chart->createDataContainer("Max(Pm)", mic::types::color_rgba(0, 0, 255, 180));

}


void EpisodicHistogramFilterMazeLocalization::startNewEpisode() {
	LOG(LWARNING) << "Start new episode";

	// Assign initial probabilities to all variables (uniform distribution).s
	hf.assignInitialProbabilities();

	// Set hidden state to "original one".
	hf.setHiddenPose(hidden_maze_number, hidden_x, hidden_y);

	// Get first observation.
	hf.sense(hit_factor, miss_factor);

	// Update aggregated probabilities.
	hf.updateAggregatedProbabilities();
}


void EpisodicHistogramFilterMazeLocalization::finishCurrentEpisode() {
	LOG(LWARNING) << "End current episode";

	w_localization_time_chart->addDataToContainer("Iteration", (double)iteration/max_number_of_iterations);
	w_localization_time_chart->addDataToContainer("Max(Pm)", max_pm);

	if (iteration >= max_number_of_iterations)
		w_localization_time_chart->addDataToContainer("Converged", 0);
	else
		w_localization_time_chart->addDataToContainer("Converged", 1);

}


bool EpisodicHistogramFilterMazeLocalization::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	short tmp_action = action;

	// Check epsilon-greedy action selection.
	if ((double)epsilon > 0) {
		if (RAN_GEN->uniRandReal() < (double)epsilon)
				tmp_action = -3;
	}//: if

	// Determine action.
	mic::types::Action2DInterface act;
	switch(tmp_action){
	case (short)-3:
			LOG(LINFO) << "Random action selection";
			act = A_RANDOM; break;
	case (short)-2:
			LOG(LINFO) << "Sum Of Most Unique Patches action selection";
			act = hf.sumOfMostUniquePatchesActionSelection(); break;
	case (short)-1:
			LOG(LINFO) << "Most Unique Patch action selection";
			act = hf.mostUniquePatchActionSelection(); break;
	default:
		act = mic::types::NESWAction((mic::types::NESW_action_type_t) (short)tmp_action);
	}//: switch action

	// Perform move.
	hf.probabilisticMove(act, exact_move_probability, overshoot_move_probability, undershoot_move_probability);


	// Get current observation.
	hf.sense(hit_factor, miss_factor);

	// Update state.
	hf.updateAggregatedProbabilities();


	// Check terminal condition(s).

	// 1. Check iteration number.
	if (iteration >= max_number_of_iterations)
		return false;

	// 2. Check max maze probability.
	max_pm = 0;
	for (size_t m=0; m<importer.getData().size(); m++) {
		max_pm = ( hf.maze_probabilities[m] > max_pm ) ? hf.maze_probabilities[m] : max_pm;
	}//: for
	if (max_pm > min_maze_confidence)
		return false;

	return true;
}


} /* namespace application */
} /* namespace mic */
