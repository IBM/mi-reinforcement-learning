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
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(init_type);
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



}



bool SimpleGridworld::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

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
