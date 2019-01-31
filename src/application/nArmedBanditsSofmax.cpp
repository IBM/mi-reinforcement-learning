/*!
 * Copyright (C) tkornuta, IBM Corporation 2015-2019
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file nArmedBanditsSofmax.cpp
 * \brief 
 * \author tkornut
 * \date Mar 14, 2016
 */

#include <application/nArmedBanditsSofmax.hpp>

#include  <data_utils/RandomGenerator.hpp>

#include <math.h>

namespace mic {
namespace application {

/*!
 * \brief Registers the application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::application::nArmedBanditsSofmax);
}


nArmedBanditsSofmax::nArmedBanditsSofmax(std::string node_name_) : OpenGLApplication(node_name_),
		number_of_bandits("number_of_bandits", 10),
		tau("tau", 1.0),
		statistics_filename("statistics_filename","statistics_filename.csv")

	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(number_of_bandits);
	registerProperty(tau);
	registerProperty(statistics_filename);

	LOG(LINFO) << "Properties registered";
}


nArmedBanditsSofmax::~nArmedBanditsSofmax() {
	delete(w_reward);
}


void nArmedBanditsSofmax::initialize(int argc, char* argv[]) {
	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	reward_collector_ptr = std::make_shared < mic::data_io::DataCollector<std::string, float> >( );
	// Add containers to collector.
	reward_collector_ptr->createContainer("average_reward", 0, 10, mic::types::color_rgba(255, 0, 0, 180));
	reward_collector_ptr->createContainer("correct_arms_percentage", 0, 100, mic::types::color_rgba(0, 255, 0, 180));
	reward_collector_ptr->createContainer("best_possible_reward", 0, 10, mic::types::color_rgba(0, 0, 255, 180));

	// Create the visualization windows - must be created in the same, main thread :]
	w_reward = new WindowCollectorChart<float>("nBandits", 256, 256, 0, 0);
	w_reward->setDataCollectorPtr(reward_collector_ptr);

}

void nArmedBanditsSofmax::initializePropertyDependentVariables() {
	// Initialize random "arm" thresholds.
	arms.resize(number_of_bandits);
	for(size_t i=0; i<number_of_bandits; i++)
		arms[i] = RAN_GEN->uniRandReal();
	//std::cout << arms << std:: endl;

	// Find the best arm.
	best_arm = -1;
	best_arm_prob = -1;
	for (size_t i=0; i<number_of_bandits; i++) {
		if (arms[i] > best_arm_prob){
			best_arm_prob = arms[i];
			best_arm = i;
		}//: if
	}//: for

	// Initialize action values and counts.
	action_values.resize(number_of_bandits);
	action_counts.resize(number_of_bandits);
	action_values_softmax.resize(number_of_bandits);

	action_values.setZero();
	action_counts.setZero();

	// Initialize softmax.
	for(size_t i=0; i<number_of_bandits; i++)
		action_values_softmax[i] = 0.1;


}

short nArmedBanditsSofmax::calculateReward(float prob_) {
    short reward = 0;
	for(size_t i=0; i<number_of_bandits; i++) {
        if (RAN_GEN->uniRandReal() < prob_)
            reward += 1;
	}//: for
    return reward;
}


void nArmedBanditsSofmax::updateSoftmaxValues() {

    // For all possible arms - compute probability distribution.
	float sum = 0;
	for(size_t i=0; i<number_of_bandits; i++) {
		action_values_softmax(i) = exp(action_values(i) / (float)tau);
		sum += action_values_softmax(i);
	}//: for

	// Normalize the distribution.
	for(size_t i=0; i<number_of_bandits; i++) {
		action_values_softmax(i) /= sum;
	}//: for

}


bool nArmedBanditsSofmax::performSingleStep() {
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


	short choice=0;
	// Select the random arm using weighted probability distribution from softmax.
	// Random number from 0 to 1.
	float r = RAN_GEN->uniRandReal();
	// "Spin the random wheel" ;)
	for (size_t i=0; i< number_of_bandits; i++){
	    if (r < action_values_softmax(i)) {
	    	choice = i;
			std::cout<< "choice=" << choice << std::endl;
	    	break;
	    }//: if
	    r -= action_values_softmax(i);
	}//: for

	// Calculate reward.
	float reward = calculateReward(arms[choice]);
	std::cout<< "reward= " << reward << std::endl;

	// Update running average for given action - Q learning;)
	action_counts[choice] +=1;
	std::cout<< "action_values[choice]"  << action_values[choice] << "  (1.0/action_counts[choice])=" << (1.0/action_counts[choice]) << " (reward - action_values[choice])=" <<  (reward - action_values[choice]) << std::endl;

	action_values[choice] =  action_values[choice] + (1.0/action_counts[choice]) * (reward - action_values[choice]);
	std::cout<< "action_values[choice] po = "  << action_values[choice] << std::endl;

	// Update softmax.
	updateSoftmaxValues();

	// Calculate the percentage the correct arm is chosen.
	float correct_arms_percentage = 100.0*(action_counts[best_arm])/((float)iteration);
	std::cout<< "correct arm/choice=" << best_arm << std::endl;

	// Calculate the mean reward.
	float running_mean_reward = 0;
	for (size_t i=0; i<number_of_bandits; i++) {
		running_mean_reward += (float)action_values[i] * (float)action_counts[i];
	}//: for all action values
	running_mean_reward /= (float)iteration;

	// Add variables to container.
	reward_collector_ptr->addDataToContainer("average_reward",running_mean_reward);
	reward_collector_ptr->addDataToContainer("correct_arms_percentage",correct_arms_percentage);
	reward_collector_ptr->addDataToContainer("best_possible_reward",10.0*best_arm_prob);

	// Export reward "convergence" diagram.
	reward_collector_ptr->exportDataToCsv(statistics_filename);

	return true;
}
} /* namespace application */
} /* namespace mic */
