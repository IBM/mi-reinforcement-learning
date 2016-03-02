/*!
 * \file HistogramFilterMazeLocalization.cpp
 * \brief File contains definition of methods of histogram filter based maze localization application.
 * \author tkornut
 * \date Jan 27, 2016
 */

#include <application/HistogramFilterMazeLocalization.hpp>
#include <application/ApplicationFactory.hpp>

#include <data_io/DataCollector.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::HistogramFilterMazeLocalization);
}


HistogramFilterMazeLocalization::HistogramFilterMazeLocalization(std::string node_name_) : OpenGLApplication(node_name_),
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
		statistics_filename("statistics_filename","statistics_filename.csv")

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

	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


HistogramFilterMazeLocalization::~HistogramFilterMazeLocalization() {

}


void HistogramFilterMazeLocalization::initialize(int argc, char* argv[]) {
	LOG(LSTATUS) << "In here you should initialize Glut and create all OpenGL windows";

	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	// Create the visualization windows - must be created in the same, main thread :]
	w_current_maze_chart = new WindowChart("Current_maze", 256, 256, 0, 0);
	w_current_coordinate_x = new WindowChart("Current_x", 256, 256, 0, 326);
	w_current_coordinate_y = new WindowChart("Current_y", 256, 256, 326, 326);

	w_max_probabilities_chart = new WindowFloatCollectorChart("Max_probabilities", 256, 256, 326, 0);
	max_probabilities_collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	w_max_probabilities_chart->setDataCollectorPtr(max_probabilities_collector_ptr);

}

void HistogramFilterMazeLocalization::initializePropertyDependentVariables() {

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

	hf.setHiddenPose(hidden_maze_number, hidden_x, hidden_y);

	// Assign initial probabilities to all variables (uniform distribution).
	hf.assignInitialProbabilities();

	// Export probabilities to file (truncate it).

	mic::data_io::DataCollector<std::string, int>::exportMatricesToCsv(statistics_filename, "mazes", importer.getData());

	std::vector<std::string> maze_pose_labels;
	for (size_t y=0; y < importer.getData()[0]->rows(); y++)
		for (size_t x=0; x < importer.getData()[0]->cols(); x++) {
			std::string label = "(" + std::to_string(y) + ";" + std::to_string(x) + ")";
			maze_pose_labels.push_back(label);
		}//: for
	mic::data_io::DataCollector<std::string, std::string>::exportVectorToCsv(statistics_filename, "maze pose labels",maze_pose_labels, true);


	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, "observation distribution P(o)", hf.maze_patch_probabilities, true);
	std::vector<int> obs_labels = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	mic::data_io::DataCollector<std::string, int>::exportVectorToCsv(statistics_filename, "observation labels",obs_labels, true);

	mic::data_io::DataCollector<std::string, double>::exportMatricesToCsv(statistics_filename, "initial P(p)", hf.maze_position_probabilities, true);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, "initial P(m)", hf.maze_probabilities, true);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, "initial P(x)", hf.maze_x_coordinate_probilities, true);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, "initial P(y)", hf.maze_y_coordinate_probilities, true);
	// Export hidden state
	mic::data_io::DataCollector<std::string, double>::exportValueToCsv(statistics_filename, "hidden_maze_number", hf.hidden_maze_number, true);
	mic::data_io::DataCollector<std::string, double>::exportValueToCsv(statistics_filename, "hidden_x", hf.hidden_x, true);
	mic::data_io::DataCollector<std::string, double>::exportValueToCsv(statistics_filename, "hidden_y", hf.hidden_y, true);

	// Create data containers - for visualization.
	createDataContainers();

	// Store the "zero" state.
	storeCurrentStateInDataContainers(true);

	// Get first observation.
	hf.sense(hit_factor, miss_factor);
	mic::data_io::DataCollector<std::string, short>::exportValueToCsv(statistics_filename, "First observation", hf.obs, true);

	// Update aggregated probabilities.
	hf.updateAggregatedProbabilities();

	// Export probabilities to file.
	mic::data_io::DataCollector<std::string, double>::exportMatricesToCsv(statistics_filename, "P(p) after first observation", hf.maze_position_probabilities, true);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, "P(m) after first observation", hf.maze_probabilities, true);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, "P(x) after first observation", hf.maze_x_coordinate_probilities, true);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, "P(y) after first observation", hf.maze_y_coordinate_probilities, true);

	// Store the first state.
	storeCurrentStateInDataContainers(true);

}



void HistogramFilterMazeLocalization::createDataContainers() {
	// Random device used for generation of colors.
	std::random_device rd;
	std::mt19937_64 rng_mt19937_64(rd());
	// Initialize uniform index distribution - integers.
	std::uniform_int_distribution<> color_dist(50, 200);
	// Create a single container for each maze.
	for (size_t m=0; m<importer.getData().size(); m++) {
		std::string label = "P(m" + std::to_string(m) +")";
		int r= color_dist(rng_mt19937_64);
		int g= color_dist(rng_mt19937_64);
		int b= color_dist(rng_mt19937_64);
		//std::cout << label << " g=" << r<< " g=" << r<< " b=" << b;

		// Add container to chart.
		w_current_maze_chart->createDataContainer(label, mic::types::color_rgba(r, g, b, 180));

	}//: for

	// Create a single container for each x coordinate.
	for (size_t x=0; x<importer.maze_width; x++) {
		std::string label = "P(x" + std::to_string(x) +")";
		int r= color_dist(rng_mt19937_64);
		int g= color_dist(rng_mt19937_64);
		int b= color_dist(rng_mt19937_64);

		// Add container to chart.
		w_current_coordinate_x->createDataContainer(label, mic::types::color_rgba(r, g, b, 180));

	}//: for


	// Create a single container for each y coordinate.
	for (size_t y=0; y<importer.maze_height; y++) {
		std::string label = "P(y" + std::to_string(y) +")";
		int r= color_dist(rng_mt19937_64);
		int g= color_dist(rng_mt19937_64);
		int b= color_dist(rng_mt19937_64);

		// Add container to chart.
		w_current_coordinate_y->createDataContainer(label, mic::types::color_rgba(r, g, b, 180));
	}//: for

	// Create containers for three max probabilites.
	max_probabilities_collector_ptr->createContainer("Max(Pm)", mic::types::color_rgba(255, 0, 0, 180));
	max_probabilities_collector_ptr->createContainer("Max(Px)", mic::types::color_rgba(0, 255, 0, 180));
	max_probabilities_collector_ptr->createContainer("Max(Py)", mic::types::color_rgba(0, 0, 255, 180));

}

void HistogramFilterMazeLocalization::storeCurrentStateInDataContainers(bool synchronize_) {

	double max_pm = 0;
	double max_px = 0;
	double max_py = 0;

	if (synchronize_)
	{ // Enter critical section - with the use of scoped lock from AppState!
		APP_DATA_SYNCHRONIZATION_SCOPED_LOCK();

		// Add data to chart windows.
		for (size_t m=0; m<importer.getData().size(); m++) {
			std::string label = "P(m" + std::to_string(m) +")";
			w_current_maze_chart->addDataToContainer(label, hf.maze_probabilities[m]);
			max_pm = ( hf.maze_probabilities[m] > max_pm ) ? hf.maze_probabilities[m] : max_pm;
		}//: for

		for (size_t x=0; x<importer.maze_width; x++) {
			std::string label = "P(x" + std::to_string(x) +")";
			w_current_coordinate_x->addDataToContainer(label, hf.maze_x_coordinate_probilities[x]);
			max_px = ( hf.maze_x_coordinate_probilities[x] > max_px ) ? hf.maze_x_coordinate_probilities[x] : max_px;
		}//: for

		for (size_t y=0; y<importer.maze_height; y++) {
			std::string label = "P(y" + std::to_string(y) +")";
			w_current_coordinate_y->addDataToContainer(label, hf.maze_y_coordinate_probilities[y]);
			max_py = ( hf.maze_y_coordinate_probilities[y] > max_py ) ? hf.maze_y_coordinate_probilities[y] : max_py;
		}//: for

		max_probabilities_collector_ptr->addDataToContainer("Max(Pm)", max_pm);
		max_probabilities_collector_ptr->addDataToContainer("Max(Px)", max_px);
		max_probabilities_collector_ptr->addDataToContainer("Max(Py)", max_py);

	}//: end of critical section.
	else {
		// Add data to chart windows.
		for (size_t m=0; m<importer.getData().size(); m++) {
			std::string label = "P(m" + std::to_string(m) +")";
			w_current_maze_chart->addDataToContainer(label, hf.maze_probabilities[m]);
			max_pm = ( hf.maze_probabilities[m] > max_pm ) ? hf.maze_probabilities[m] : max_pm;
		}//: for

		for (size_t x=0; x<importer.maze_width; x++) {
			std::string label = "P(x" + std::to_string(x) +")";
			w_current_coordinate_x->addDataToContainer(label, hf.maze_x_coordinate_probilities[x]);
			max_px = ( hf.maze_x_coordinate_probilities[x] > max_px ) ? hf.maze_x_coordinate_probilities[x] : max_px;
		}//: for

		for (size_t y=0; y<importer.maze_height; y++) {
			std::string label = "P(y" + std::to_string(y) +")";
			w_current_coordinate_y->addDataToContainer(label, hf.maze_y_coordinate_probilities[y]);
			max_py = ( hf.maze_y_coordinate_probilities[y] > max_py ) ? hf.maze_y_coordinate_probilities[y] : max_py;
		}//: for

		max_probabilities_collector_ptr->addDataToContainer("Max(Pm)", max_pm);
		max_probabilities_collector_ptr->addDataToContainer("Max(Px)", max_px);
		max_probabilities_collector_ptr->addDataToContainer("Max(Py)", max_py);
	}//: else
}



bool HistogramFilterMazeLocalization::performSingleStep() {
	LOG(LINFO) << "Performing a single step (" << iteration << ")";

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
			act = A_RANDOM; break;
	case (short)-2:
			act = hf.sumOfMostUniquePatchesActionSelection(); break;
	case (short)-1:
			act = hf.mostUniquePatchActionSelection(); break;
	default:
		act = mic::types::NESWAction((mic::types::NESW_action_type_t) (short)tmp_action);
	}//: switch action

	std:: string label = "Action d_x at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, int>::exportValueToCsv(statistics_filename, label, act.dx(), true);
	label = "Action d_y at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, int>::exportValueToCsv(statistics_filename, label, act.dy(), true);

	// Perform move.
	hf.probabilisticMove(act, exact_move_probability, overshoot_move_probability, undershoot_move_probability);

	// Get current observation.
	hf.sense(hit_factor, miss_factor);

	label = "Observation (after motion) at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, short>::exportValueToCsv(statistics_filename, label, hf.obs, true);

	// Update state.
	hf.updateAggregatedProbabilities();

	// Export probabilities to file.
	label = "P(p) at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, double>::exportMatricesToCsv(statistics_filename, label, hf.maze_position_probabilities, true);
	label = "P(m) at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, label, hf.maze_probabilities, true);
	label = "P(x) at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, label, hf.maze_x_coordinate_probilities, true);
	label = "P(y) at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, double>::exportVectorToCsv(statistics_filename, label, hf.maze_y_coordinate_probilities, true);

	label = "hidden_maze_number at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, double>::exportValueToCsv(statistics_filename, label, hf.hidden_maze_number, true);
	label = "hidden_x at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, double>::exportValueToCsv(statistics_filename, label, hf.hidden_x, true);
	label = "hidden_y at " + std::to_string(iteration);
	mic::data_io::DataCollector<std::string, double>::exportValueToCsv(statistics_filename, label, hf.hidden_y, true);



	// Export convergence diagram.
	std::string filename = statistics_filename;
	std::string tmp = filename.substr(0, (filename.find('.'))) + "-convergence.csv";
	max_probabilities_collector_ptr->exportDataToCsv(tmp);


	// Store collected data for visualization/export.
	storeCurrentStateInDataContainers(false);

	return true;
}






} /* namespace application */
} /* namespace mic */
