/*!
 * \file MNISTDigit.cpp
 * \brief 
 * \author tkornut
 * \date Jun 8, 2016
 */

#include <types/MNISTDigit.hpp>
#include  <data_utils/RandomGenerator.hpp>

namespace mic {
namespace environments {

MNISTDigit::MNISTDigit(std::string node_name_) : Environment(node_name_),
	mnist_importer("mnist_importer"),
	sample_number("sample_number", 0),
	agent_x("agent_x",-1),
	agent_y("agent_y",-1),
	goal_x("goal_x",-1),
	goal_y("goal_y",-1)
{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(sample_number);
	registerProperty(agent_x);
	registerProperty(agent_y);
	registerProperty(goal_x);
	registerProperty(goal_y);

	channels = (size_t)MNISTDigitChannels::Count;

}

MNISTDigit::~MNISTDigit() {
	// TODO Auto-generated destructor stub
}

mic::environments::MNISTDigit & MNISTDigit::operator= (const mic::environments::MNISTDigit & md_) {
	width = md_.width;
	height = md_.height;
	channels = md_.channels;
	initial_position = md_.initial_position;
	environment_grid = md_.environment_grid;
	observation_grid = md_.observation_grid;

	return *this;
}


// Initialize environment_grid.
void MNISTDigit::initializePropertyDependentVariables() {
	// Load dataset.
	if (!mnist_importer.importData()) {
		//return;
	}

	// Set environment size.
	width = height = 28;
	environment_grid->resize({width, height, channels});

	// Check whether it is a POMDP or not.
	if (roi_size >0) {
		pomdp_flag = true;
		observation_grid->resize({roi_size, roi_size, channels});
	} else {
		observation_grid->resize({width, height, channels});
	}//: else

}

void MNISTDigit::initializeEnvironment() {
	// Reset the grid.
	environment_grid->zeros();

	if (mnist_importer.size() > 0) {
		mic::types::MNISTSample sample;
//		LOG(LERROR) << "sample_number: " << sample_number;

		if ((sample_number < 0) || (sample_number >= mnist_importer.size()))
			// Random select sample from dataset.
			sample = mnist_importer.getRandomSample();
		else
			// Get given sample.
			sample = mnist_importer.getSample(sample_number);

		for (size_t x=0; x<width; x++)
			for (size_t y=0; y<height; y++)
				(*environment_grid)({x,y,(size_t)MNISTDigitChannels::Pixels}) = (*sample.data())(y,x);
		LOG(LINFO) << "Digit: " << (*sample.label());
	}

	// Put goal and agent.
	size_t ax,ay,gx,gy;

	// Set agent coordinates.
	ax = (agent_x < 0) ? RAN_GEN->uniRandInt(0,width) : agent_x;
	ay = (agent_y < 0) ? RAN_GEN->uniRandInt(0,height) : agent_y;
	// Set initial position
	initial_position.set(ax,ay);
	moveAgentToInitialPosition();

	// Set goal coordinates.
	gx = (goal_x < 0) ? RAN_GEN->uniRandInt(0,width) : goal_x;
	gy = (goal_y < 0) ? RAN_GEN->uniRandInt(0,height) : goal_y;
	(*environment_grid)({gx,gy,(size_t)MNISTDigitChannels::Goals}) = 10;

	// Calculate the optimal path length.
	optimal_path_length = abs((int)ax-(int)gx) + abs((int)ay-(int)gy);
}

std::string MNISTDigit::toString(mic::types::TensorXfPtr env_) {
	std::string s;
	// Add line.
	s+= "+";
	for (size_t x=0; x<env_->dim(0); x++)
		s+="---";
	s+= "+\n";

	for (size_t y=0; y<env_->dim(1); y++){
		s += "|";
		for (size_t x=0; x<env_->dim(0); x++) {
			// Check object occupancy.
			if ((*env_)({x,y, (size_t)MNISTDigitChannels::Agent}) != 0) {
				// Display agent.
				s += "<A>";
			} else if ((*env_)({x,y, (size_t)MNISTDigitChannels::Goals}) > 0) {
				// Display goal.
				s += " + ";
			} else {
				// Display "image patch" - pixel.
				//((*env_)({x,y, (size_t)MNISTDigitChannels::Pixels})
				s += "   ";
			}
		}//: for x
		s += "|\n";
	}//: for y

	// Add line.
	s+= "+";
	for (size_t x=0; x<env_->dim(0); x++)
		s+="---";
	s+= "+\n";
	return s;
}

std::string MNISTDigit::environmentToString() {
	return toString(environment_grid);
}

std::string MNISTDigit::observationToString() {
	if (pomdp_flag) {
		// Get observation.
		mic::types::TensorXfPtr obs = getObservation();
		return toString(obs);
	}
	else
		return toString(environment_grid);
}

mic::types::MatrixXfPtr MNISTDigit::encodeEnvironment() {
	// Temporarily reshape the environment_grid.
	environment_grid->conservativeResize({1, width * height * channels});
	// Create a matrix pointer and copy data from grid into the matrix.
	mic::types::MatrixXfPtr encoded_grid (new mic::types::MatrixXf(*environment_grid));
	// Back to the original shape.
	environment_grid->resize({width, height, channels});

	// Return the matrix pointer.
	return encoded_grid;
}

mic::types::MatrixXfPtr MNISTDigit::encodeObservation() {
	LOG(LDEBUG) << "encodeObservation()";
	if (pomdp_flag) {
		mic::types::Position2D p = getAgentPosition();
		LOG(LDEBUG) << p;

		// Get observation.
		mic::types::TensorXfPtr obs = getObservation();
		// Temporarily reshape the observation grid.
		obs->conservativeResize({1, roi_size * roi_size * channels});
		// Encode the observation.
		mic::types::MatrixXfPtr encoded_obs (new mic::types::MatrixXf(*obs));
		// Back to the original shape.
		obs->conservativeResize({roi_size, roi_size, channels});

		return encoded_obs;
	}
	else
		return encodeEnvironment();
}


mic::types::TensorXfPtr MNISTDigit::getObservation() {
	LOG(LDEBUG) << "getObservation()";
	// Reset.
	observation_grid->zeros();

	size_t delta = (roi_size-1)/2;
	mic::types::Position2D p = getAgentPosition();

	// Copy data.
	for (long oy=0, ey=(p.y-delta); oy<roi_size; oy++, ey++){
		for (long ox=0, ex=(p.x-delta); ox<roi_size; ox++, ex++) {
			// Check grid boundaries.
			if ((ex < 0) || (ex >= width) || (ey < 0) || (ey >= height)){
				// Do nothing...
				continue;
			}//: if
			// Else : copy data for all channels.
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)MNISTDigitChannels::Goals}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)MNISTDigitChannels::Goals});
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)MNISTDigitChannels::Pixels}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)MNISTDigitChannels::Pixels});
			(*observation_grid)({(size_t)ox,(size_t)oy, (size_t)MNISTDigitChannels::Agent}) = (*environment_grid)({(size_t)ex,(size_t)ey, (size_t)MNISTDigitChannels::Agent});
		}//: for x
	}//: for y

	//LOG(LDEBUG) << std::endl << toString(observation_grid);

	return observation_grid;
}



mic::types::Position2D MNISTDigit::getAgentPosition() {
	mic::types::Position2D position;
	for (size_t y=0; y<height; y++){
		for (size_t x=0; x<width; x++) {
			if ((*environment_grid)({x,y, (size_t)MNISTDigitChannels::Agent}) == 1) {
				position.x = x;
				position.y = y;
				return position;
			}// if
		}//: for x
	}//: for y
	// Remove warnings...
	return position;
}

bool MNISTDigit::moveAgentToPosition(mic::types::Position2D pos_) {
	LOG(LDEBUG) << "New agent position = " << pos_;

	// Check whether the state is allowed.
	if (!isStateAllowed(pos_))
		return false;

	// Clear old.
	mic::types::Position2D old = getAgentPosition();
	(*environment_grid)({(size_t)old.x, (size_t)old.y, (size_t)MNISTDigitChannels::Agent}) = 0;
	// Set new.
	(*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MNISTDigitChannels::Agent}) = 1;

	return true;
}



float MNISTDigit::getStateReward(mic::types::Position2D pos_) {
	// Check rewards.
	if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MNISTDigitChannels::Goals}) != 0)
		return (*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MNISTDigitChannels::Goals});
	else
        return 0;
}


bool MNISTDigit::isStateAllowed(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

	return true;
}


bool MNISTDigit::isStateTerminal(mic::types::Position2D pos_) {
	if ((pos_.x < 0) || (pos_.x >= width))
		return false;

	if ((pos_.y < 0) || (pos_.y >= height))
			return false;

	// Check reward - goal.
	if ((*environment_grid)({(size_t)pos_.x, (size_t)pos_.y, (size_t)MNISTDigitChannels::Goals}) != 0)
        return true;
    else

        return false;
}

} /* namespace environments */
} /* namespace mic */
