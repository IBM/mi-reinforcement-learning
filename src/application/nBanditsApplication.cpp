/*!
 * \file nBanditsApplication.cpp
 * \brief 
 * \author tkornut
 * \date Mar 14, 2016
 */

#include <application/nBanditsApplication.hpp>

#include  <data_utils/RandomGenerator.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::nBanditsApplication);
}


nBanditsApplication::nBanditsApplication(std::string node_name_) : OpenGLApplication(node_name_),
		number_of_bandits("number_of_bandits", 10),
		epsilon("epsilon", 0.0),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(number_of_bandits);
	registerProperty(epsilon);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


nBanditsApplication::~nBanditsApplication() {

}


void nBanditsApplication::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	// Create the visualization windows - must be created in the same, main thread :]
	w_reward = new WindowFloatCollectorChart("Average_reward", 256, 256, 0, 0);

	reward_collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	w_reward->setDataCollectorPtr(reward_collector_ptr);

}

void nBanditsApplication::initializePropertyDependentVariables() {
	// Random "arm" thresholds.
	arms.resize(number_of_bandits);
	for(int i=0; i<number_of_bandits; i++)
		arms[i] = RAN_GEN->uniRandReal();
	std::cout << arms << std:: endl;
}

short nBanditsApplication::reward(float prob_) {
    short reward = 0;
	for(int i=0; i<number_of_bandits; i++) {
        if (RAN_GEN->uniRandReal() < prob_)
            reward += 1;
	}//: for
    return reward;
}

bool nBanditsApplication::performSingleStep() {
	LOG(LINFO) << "Performing a single step (" << iteration << ")";

	short tmp_action = 0;

	// Check epsilon-greedy action selection.
	if ((double)epsilon > 0) {
		if (RAN_GEN->uniRandReal() < (double)epsilon)
				tmp_action = -1;
	}//: if

/*	// Determine action.
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
*/
//	std:: string label = "Action d_x at " + std::to_string(iteration);
//	mic::data_io::DataCollector<std::string, int>::exportValueToCsv(statistics_filename, label, act.dx(), true);

/*	// Export convergence diagram.
	std::string filename = statistics_filename;
	std::string tmp = filename.substr(0, (filename.find('.'))) + "-convergence.csv";
	max_probabilities_collector_ptr->exportDataToCsv(tmp);*/

	return true;
}



} /* namespace application */
} /* namespace mic */
