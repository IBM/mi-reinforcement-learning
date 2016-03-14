/*!
 * \file nArmedBanditsSimpleQlearning.cpp
 * \brief 
 * \author tkornut
 * \date Mar 14, 2016
 */

#include <application/nArmedBanditsSimpleQlearning.hpp>

#include  <data_utils/RandomGenerator.hpp>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::nArmedBanditsSimpleQlearning);
}


nArmedBanditsSimpleQlearning::nArmedBanditsSimpleQlearning(std::string node_name_) : OpenGLApplication(node_name_),
		number_of_bandits("number_of_bandits", 10),
		epsilon("epsilon", 0.1),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(number_of_bandits);
	registerProperty(epsilon);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


nArmedBanditsSimpleQlearning::~nArmedBanditsSimpleQlearning() {

}


void nArmedBanditsSimpleQlearning::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	reward_collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
	reward_collector_ptr->createContainer("average_reward", 0, 10, mic::types::color_rgba(255, 0, 0, 180));
	reward_collector_ptr->createContainer("correct_arms_percentage", 0, 100, mic::types::color_rgba(0, 255, 0, 180));

	// Create the visualization windows - must be created in the same, main thread :]
	w_reward = new WindowFloatCollectorChart("nBandits", 256, 256, 0, 0);
	w_reward->setDataCollectorPtr(reward_collector_ptr);

}

void nArmedBanditsSimpleQlearning::initializePropertyDependentVariables() {
	// Initialize random "arm" thresholds.
	arms.resize(number_of_bandits);
	for(int i=0; i<number_of_bandits; i++)
		arms[i] = RAN_GEN->uniRandReal();
	//std::cout << arms << std:: endl;

	// Initialize action values and counts.
	action_values.resize(number_of_bandits);
	action_counts.resize(number_of_bandits);

	action_values.setOnes();
	action_counts.setZero();
}

short nArmedBanditsSimpleQlearning::calculateReward(float prob_) {
    short reward = 0;
	for(size_t i=0; i<number_of_bandits; i++) {
        if (RAN_GEN->uniRandReal() < prob_)
            reward += 1;
	}//: for
    return reward;
}


short nArmedBanditsSimpleQlearning::selectBestArm() {
	// Greedy methods - returns the index of element with greatest value.
	short best_arm = 0;
    float best_value = -1;
    // For all possible arms.
	for(size_t i=0; i<number_of_bandits; i++) {
		// Check if this one is better than the others.
		if (action_values(i) > best_value) {
			best_value = action_values(i);
			best_arm = i;
		}//: if
	}//: for
    return best_arm;
}


bool nArmedBanditsSimpleQlearning::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	std::cout<< "hidden state (arms)=";
	for (size_t i=0; i<number_of_bandits; i++)
		std::cout << arms[i] << ", ";
	std::cout << std::endl;

	std::cout << "action_counts=" ;
	for (size_t i=0; i<number_of_bandits; i++)
		std::cout << action_counts[i] << ", ";
	std::cout << std::endl;

	std::cout<< "action_values=";
	for (size_t i=0; i<number_of_bandits; i++)
		std::cout << action_values[i] << ", ";
	std::cout << std::endl;


	short choice;
	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > (double)epsilon){
		// Select best action.
		choice = selectBestArm();
		std::cout<< "best choice=" << choice << std::endl;
	} else {
		//std::cout << "Random action!" << std::endl;
		// Random arm selection.
        choice = RAN_GEN->uniRandInt(0, number_of_bandits-1);
    	std::cout<< "random choice=" << choice << std::endl;
	}//: if


	// Calculate reward.
	float reward = calculateReward(arms[choice]);
	std::cout<< "reward= " << reward << std::endl;

	// Update running average for given action - Q learning;)
	action_counts[choice] +=1;
	std::cout<< "action_values[choice]"  << action_values[choice] << "  (1.0/action_counts[choice])=" << (1.0/action_counts[choice]) << " (reward - action_values[choice])=" <<  (reward - action_values[choice]) << std::endl;

	action_values[choice] =  action_values[choice] + (1.0/action_counts[choice]) * (reward - action_values[choice]);
	std::cout<< "action_values[choice] po = "  << action_values[choice] << std::endl;
	// Calculate the percentage the correct arm is chosen.
	short max_arm = -1;
	float max_arm_prob = -1;
	for (size_t i=0; i<number_of_bandits; i++) {
		if (arms[i] > max_arm_prob){
			max_arm_prob = arms[i];
			max_arm = i;
		}//: if
	}//: for
	float correct_arms_percentage = 100.0*(action_counts[max_arm])/((float)iteration);
	std::cout<< "correct arm/choice=" << max_arm << std::endl;

	// Calculate the mean reward.
	float running_mean_reward = 0;
	for (size_t i=0; i<number_of_bandits; i++) {
		running_mean_reward += (float)action_values[i] * (float)action_counts[i];
	}//: for all action values
	running_mean_reward /= (float)iteration;

	// Add variables to container.
	reward_collector_ptr->addDataToContainer("average_reward",running_mean_reward);
	reward_collector_ptr->addDataToContainer("correct_arms_percentage",correct_arms_percentage);

	// Export reward "convergence" diagram.
	reward_collector_ptr->exportDataToCsv(statistics_filename);

	return true;
}



} /* namespace application */
} /* namespace mic */
