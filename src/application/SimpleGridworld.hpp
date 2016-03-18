/*!
 * \file SimpleGridworld.hpp
 * \brief 
 * \author tkornut
 * \date Mar 17, 2016
 */

#ifndef SRC_APPLICATION_SIMPLEGRIDWORLD_HPP_
#define SRC_APPLICATION_SIMPLEGRIDWORLD_HPP_

#include <vector>

#include <types/TensorTypes.hpp>

#include <opengl/application/OpenGLApplication.hpp>
#include <opengl/visualization/WindowFloatCollectorChart.hpp>
using namespace mic::opengl::visualization;

#include <types/Action.hpp>

namespace mic {
namespace application {


/*!
 * \brief Class implementing a simple gridworld application with reinforcement learning used for solveing the task.
 * \author tkornuta
 */
class SimpleGridworld: public mic::opengl::application::OpenGLApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	SimpleGridworld(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~SimpleGridworld();

protected:
	/*!
	 * Initializes all variables that are property-dependent.
	 */
	virtual void initializePropertyDependentVariables();

	/*!
	 * Method initializes GLUT and OpenGL windows.
	 * @param argc Number of application parameters.
	 * @param argv Array of application parameters.
	 */
	virtual void initialize(int argc, char* argv[]);

	/*!
	 * Performs single step of computations.
	 */
	virtual bool performSingleStep();

private:

	/// Window for displaying ???.
	WindowFloatCollectorChart* w_chart;

	/// Data collector.
	mic::data_io::DataCollectorPtr<std::string, float> collector_ptr;

	/// n Bandit arms.
	mic::types::TensorXf gridworld;

	/// Property: type of initialization:
	/// 0: stationary grid and all items (wall, goal and pit, player) placed deterministically
	/// 1: stationary grid, all items placed (wall, goal and pit) deterministically except player (placed randomly)
	/// 2: stationary grid, all items (wall, goal and pit, player) placed randomly
	mic::configuration::Property<short> init_type;

	/// Property: width of gridworld.
	mic::configuration::Property<size_t> width;

	/// Property: height of gridworld.
	mic::configuration::Property<size_t> height;


	/// Property: name of the file to which the statistics will be exported.
	mic::configuration::Property<std::string> statistics_filename;

	/// Method initializes the stationary grid, i.e. all items are placed deterministically.
	void initGrid();

	/// Displays grid in terminal.
	void displayGrid();

	/*!
	 * Calculates the player position.
	 * @return Player position.
	 */
	std::pair<size_t, size_t> getPlayerPosition();

	/*!
	 * Calculates the reward for being in given state.
	 * @return Reward for being in given state (r).
	 */
	short calculateReward();

	/*!
	 * Performs "deterministic" move. It is assumed that the move is truncated by the gridworld boundaries (no circular world assumption).
	 * @param ac_ Performed action.
	 */
	void move (mic::types::Action2DInterface ac_);

};


} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_SIMPLEGRIDWORLD_HPP_ */
