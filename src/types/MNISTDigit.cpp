/*!
 * \file MNISTDigit.cpp
 * \brief 
 * \author tkornut
 * \date Jun 8, 2016
 */

#include <types/MNISTDigit.hpp>

namespace mic {
namespace environments {

MNISTDigit::MNISTDigit(std::string node_name_) : Environment(node_name_),
	mnist_importer("mnist_importer"),
	sample_number("sample_number", 0)
{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(sample_number);

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

	width = height = 28;

	// Load dataset.
	if (!mnist_importer.importData()) {
		//return;
		// Set proper size.
		environment_grid->resize({28,28,channels});
		// Put goal and agent.
		(*environment_grid)({10,10,(size_t)MNISTDigitChannels::Agent}) = (float)1;
		(*environment_grid)({14,14,(size_t)MNISTDigitChannels::Goals}) = 10;

	} else {
		// Random select sample from dataset.
		mic::types::MNISTSample sample = mnist_importer.getRandomSample();
		// TODO: decide whether random, or n-th sample etc.

		// Copy data from image to grid.
		environment_grid->resize({width, height, channels});

		for (size_t x=0; x<width; x++)
			for (size_t y=0; y<width; y++)
				(*environment_grid)({x,y,(size_t)MNISTDigitChannels::Pixels}) = (*sample.data())(y,x);
		//(*environment_grid) = *(sample.data());
		// Add "additional" channels.
		//environment_grid->resize({28,28,channels});

		// Put goal and agent.
		(*environment_grid)({10,10,(size_t)MNISTDigitChannels::Agent}) = (float)1;
		(*environment_grid)({14,14,(size_t)MNISTDigitChannels::Goals}) = 10;
	}
	// Check whether it is a POMDP or not.
	if (roi_size >0) {
		pomdp_flag = true;
		observation_grid->resize({roi_size, roi_size, channels});
	} else {
		observation_grid->resize({width, height, channels});
	}//: else

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
