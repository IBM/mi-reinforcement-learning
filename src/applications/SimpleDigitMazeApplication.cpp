/*!
 * \file TestApplication.cpp
 * \brief File contains definition of methods of simple exemplary application.
 * \author tkornut
 * \date Jan 27, 2016
 */

#include <applications/SimpleDigitMazeApplication.hpp>

#include <application/ApplicationFactory.hpp>


namespace mic {
namespace application {

/*!
 * \brief Registers application.
 * \author tkornuta
 */
void RegisterApplication (void) {
	REGISTER_APPLICATION(mic::applications::SimpleDigitMazeApplication);
}

} /* namespace application */

namespace applications {

SimpleDigitMazeApplication::SimpleDigitMazeApplication(std::string node_name_) : ContinuousLearningApplication(node_name_),
		sdr_size("sdr_size", 8)
	{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(sdr_size);

	LOG(LINFO) << "Properties registered";

}


SimpleDigitMazeApplication::~SimpleDigitMazeApplication() {
	LOG(LINFO) << "Empty for now";
}


void SimpleDigitMazeApplication::initialize(int argc, char* argv[]) {
	LOG(LSTATUS) << "In here you should initialize Glut and create all OpenGL windows";

	// Initialize GLUT! :]
	VGL_MANAGER->initializeGLUT(argc, argv);

	// Create the visualization windows - must be created in the same, main thread :]
	w_chart = new WindowChart("Statistics", 256, 512, 0, 326);

	// Create data containers.
	w_chart->createDataContainer("Error", mic::types::color_rgba(255, 0, 0, 180));

}

void SimpleDigitMazeApplication::initializePropertyDependentVariables() {
	LOG(LSTATUS) << "In here you should initialize all variables that depend on properties";

	LOG(LINFO) << "You an for example allocate memory for structures)";

	// Load datasets.
	LOG(LINFO) << "Or import data with the use of importer-derived class.";
}


void SimpleDigitMazeApplication::performLearning() {

	LOG(LWARNING) << "Perform learning ";
}



void SimpleDigitMazeApplication::performTesting() {

	LOG(LSTATUS) << "Perform testing ";

}



} /* namespace applications */
} /* namespace mic */
