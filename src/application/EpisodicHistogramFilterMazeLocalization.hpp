/*!
 * \file EpisodicHistogramFilterMazeLocalization.hpp
 * \brief 
 * \author tkornut
 * \date Feb 19, 2016
 */

#ifndef SRC_APPLICATION_EPISODICHISTOGRAMFILTERMAZELOCALIZATION_HPP_
#define SRC_APPLICATION_EPISODICHISTOGRAMFILTERMAZELOCALIZATION_HPP_

#include <opengl/application/OpenGLEpisodicApplication.hpp>

#include <opengl/visualization/WindowChart.hpp>
using namespace mic::opengl::visualization;

#include <types/MatrixTypes.hpp>

#include <data_io/MazeMatrixImporter.hpp>

#include <types/Action.hpp>

#include <algorithms/MazeHistogramFilter.hpp>


namespace mic {
namespace application {

/*!
 * \brief Application for episodic testing of convergence of histogram filter based maze-of-digits localization.
 * \author tkornuta
 */
class EpisodicHistogramFilterMazeLocalization: public mic::opengl::application::OpenGLEpisodicApplication {
public:
	/*!
	 * Default Constructor. Sets the application/node name, default values of variables, initializes classifier etc.
	 * @param node_name_ Name of the application/node (in configuration file).
	 */
	EpisodicHistogramFilterMazeLocalization(std::string node_name_ = "application");

	/*!
	 * Destructor.
	 */
	virtual ~EpisodicHistogramFilterMazeLocalization();

protected:
	/*!
	 * Initializes all variables that are property-dependent (input patches, SDRs etc.).
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

protected:

	/*!
	 * Method called at the beginning of new episode (goal: to reset the statistics etc.) - abstract, to be overridden.
	 */
	virtual void startNewEpisode();

	/*!
	 * Method called when given episode ends (goal: export collected statistics to file etc.) - abstract, to be overridden.
	 */
	virtual void finishCurrentEpisode();


private:

	/// Window for displaying chart with statistics on current maze number.
	WindowChart* w_localization_time_chart;

	/// Importer responsible for loading mazes from file.
	mic::data_io::MazeMatrixImporter importer;

	/// Histogram filter.
	mic::algorithms::MazeHistogramFilter hf;

	/// Property: variable denoting in which maze are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_maze_number;

	/// Property: variable denoting the x position are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_x;

	/// Property: variable denoting the y position are we right now (unknown, to be determined).
	mic::configuration::Property<short> hidden_y;

	/// Property: performed action (0-3: NESW, -3: random, -2: sumOfMostUniquePatchesActionSelection, -1: mostUniquePatchActionSelection).
	mic::configuration::Property<short> action;

	/// Property: variable denoting epsilon in aciton selection (the probability "below" which a random action will be selected).
	mic::configuration::Property<double> epsilon;

	/// Property: variable denoting the hit factor (the gain when the observation coincides with current position).
	mic::configuration::Property<double> hit_factor;

	/// Property: variable denoting the miss factor (the gain when the observation does not coincide with current position).
	mic::configuration::Property<double> miss_factor;

	/// Property: variable storing the probability that we made the exact move (x+dx).
	mic::configuration::Property<double> exact_move_probability;

	/// Property: variable storing the probability that we made the "overshoot" move (d+dx+1).
	mic::configuration::Property<double> overshoot_move_probability;

	/// Property: variable storing the probability that we made the "undershoot" move (d+dx-1).
	mic::configuration::Property<double> undershoot_move_probability;

	/// Property: stop condition 1: maximal number of iterations - if exceeded, we finish the episode claiming that that it was a failure.
	mic::configuration::Property<short> max_number_of_iterations;

	/// Property: stop condition 2: minimal maze confidence - if exceeded, we finish the episode claiming that that it was a success.
	mic::configuration::Property<double> min_maze_confidence;

	/// Maximum maze probability for given episode.
	double max_pm;
};


} /* namespace application */
} /* namespace mic */

#endif /* SRC_APPLICATION_EPISODICHISTOGRAMFILTERMAZELOCALIZATION_HPP_ */
