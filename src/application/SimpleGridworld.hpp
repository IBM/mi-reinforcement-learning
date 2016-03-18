/*!
 * \file SimpleGridworld.hpp
 * \brief 
 * \author tkornut
 * \date Mar 17, 2016
 */

#ifndef SRC_APPLICATION_SIMPLEGRIDWORLD_HPP_
#define SRC_APPLICATION_SIMPLEGRIDWORLD_HPP_

#include <vector>
#include <string>

#include <types/TensorTypes.hpp>

#include <opengl/application/OpenGLApplication.hpp>
#include <opengl/visualization/WindowFloatCollectorChart.hpp>
using namespace mic::opengl::visualization;

#include <types/Position2D.hpp>

namespace mic {
namespace application {

/*!
 * \brief Gridworld channels
 * \author tkornuta
 */
enum class GridworldChannels : std::size_t
{
	Goal = 0, ///< Channel storing the goal(s)
	Pit = 1, ///< Channel storing the pits(s)
	Wall = 2, ///< Channel storing the walls(s)
	Player = 3 ///< Channel storing the player pose
};


/*!
 * \brief Class implementing a simple gridworld application with reinforcement learning used for solving the task.
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

	/// Tensor storing the 3D Gridworld (x + y + each "depth" channel representing the
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
	void initExemplaryGrid();

	/*!
	 * Initializes the classic cliff gridworld.
	 * [[' ',' ',' ',' ',' '],
	 *  ['S',' ',' ',' ',10],
	 *  [-100,-100, -100, -100, -100]]
	 */
	void initClassicCliffGrid();


	/*!
	 * Returns the (flattened, i.e. 2D) grid of characters.
	 * @return Flattened grid of chars.
	 */
	mic::types::Tensor<char> flattenGrid();

	/*!
	 * Steams the current state of the gridworld.
	 * @return Ostream.
	 */
	std::string streamGrid();

	/*!
	 * Calculates the player position.
	 * @return Player position.
	 */
	mic::types::Position2D getPlayerPosition();

	/*!
	 * Calculates the reward for being in given state.
	 * @return Reward for being in given state (r).
	 */
	short calculateReward();

	/*!
	 * Performs "deterministic" move. It is assumed that the move is truncated by the gridworld boundaries (no circular world assumption).
	 * @param ac_ The action to be performed.
	 * @return True if move was performed, false if it was not possible.
	 */
	bool move (mic::types::Action2DInterface ac_);

	/*!
	 * Checks if position is allowed, i.e. within the gridworld boundaries and there is no wall at that place.
	 * @param x The x coordinate.
	 * @param y The y coordinate.
	 * @return True if the possition is allowed, false othervise.
	 */
	bool isPositionAllowed(long x, long y);

};


} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_SIMPLEGRIDWORLD_HPP_ */
