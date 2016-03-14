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

	reward_collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
	reward_collector_ptr->createContainer("average_reward", 0, 10, mic::types::color_rgba(255, 0, 0, 180));
	reward_collector_ptr->createContainer("correct_arms_percentage", 0, 100, mic::types::color_rgba(0, 255, 0, 180));

	// Create the visualization windows - must be created in the same, main thread :]
	w_reward = new WindowFloatCollectorChart("nBandits", 256, 256, 0, 0);
	w_reward->setDataCollectorPtr(reward_collector_ptr);

}

void nBanditsApplication::initializePropertyDependentVariables() {
	// Initialize random "arm" thresholds.
	arms.resize(number_of_bandits);
	for(int i=0; i<number_of_bandits; i++)
		arms[i] = RAN_GEN->uniRandReal();
	//std::cout << arms << std:: endl;

	// Initialize action value - add single row with random action index and value of 0.
	action_values.push_back(std::make_pair(RAN_GEN->uniRandInt(0, number_of_bandits-1), 0));

}

short nBanditsApplication::calculateReward(float prob_) {
    short reward = 0;
	for(size_t i=0; i<number_of_bandits; i++) {
        if (RAN_GEN->uniRandReal() < prob_)
            reward += 1;
	}//: for
    return reward;
}


short nBanditsApplication::selectBestArm() {

	// greedy method to select best arm based on memory array (historical results)
    short best_arm = 0;
    float best_mean = -1;
    // For all possible arms.
	for(size_t i=0; i<number_of_bandits; i++) {
		long sum = 0;
		long no_actions=0;
		for(auto av: action_values){
			if (av.first == i) {
				sum += av.second;
				no_actions ++;
			}
		}//: for all action values
		// Calculate mean reward for each action.
		float mean_reward = (float) sum/no_actions;
		//std::cout<< "mean_reward ["<< i <<"] = " << mean_reward <<std::endl;
		// Check if this one is better than the others.
		if (mean_reward > best_mean) {
			best_mean = mean_reward;
			best_arm = i;
			//std::cout<< "found best reward = " << best_mean <<" for arm" << best_arm <<std::endl;
		}//: if
	}//: for
	//std::cout<< "best arm = " << best_arm <<std::endl;
    return best_arm;
}


bool nBanditsApplication::performSingleStep() {
	LOG(LTRACE) << "Performing a single step (" << iteration << ")";

	short choice;
	// Epsilon-greedy action selection.
	if (RAN_GEN->uniRandReal() > (double)epsilon){
		// Select best action.
		choice = selectBestArm();
	} else {
		//std::cout << "Random action!" << std::endl;
		// Random arm selection.
        choice = RAN_GEN->uniRandInt(0, number_of_bandits-1);
	}//: if

	// Calculate reward.
	//std::cout << "choice = " << choice << " arms[choice]=" << arms[choice] << std::endl;
	short reward = calculateReward(arms[choice]);
	// Add results to the memory.
	action_values.push_back(std::make_pair(choice, reward));

	// Calculate the percentage the correct arm is chosen.
	short max_arm = -1;
	float max_arm_prob = -1;
	for (size_t i=0; i<number_of_bandits; i++) {
		if (arms[i] > max_arm_prob){
			max_arm_prob = arms[i];
			max_arm = i;
		}//: if
	}//: for
	long correct_arm =0;
	for(auto av: action_values){
		if (av.first == max_arm)
			correct_arm++;
	}//: for all action values
	float correct_arms_percentage = 100.0*correct_arm/(action_values.size()-1);

	// Calculate the mean reward.
	float running_mean_reward = 0;
	//std::cout << "action_values= \n";
	for(auto av: action_values){
		//std::cout << av.first << ", " << av.second << std::endl;
		running_mean_reward += av.second;
	}//: for all action values
	running_mean_reward /= (action_values.size()-1);

	// Add variables to container.
	reward_collector_ptr->addDataToContainer("average_reward",running_mean_reward);
	reward_collector_ptr->addDataToContainer("correct_arms_percentage",correct_arms_percentage);

	// Export reward "convergence" diagram.
	reward_collector_ptr->exportDataToCsv(statistics_filename);

	return true;
}



} /* namespace application */
} /* namespace mic */
