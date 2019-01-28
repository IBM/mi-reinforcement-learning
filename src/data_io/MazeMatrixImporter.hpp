/*!
 * \file MazeMatrixImporter.hpp
 * \brief 
 * \author tkornut
 * \date Jan 30, 2016
 */

#ifndef SRC_DATA_IO_MAZEMATRIXIMPORTER_HPP_
#define SRC_DATA_IO_MAZEMATRIXIMPORTER_HPP_

#include <data_io/Importer.hpp>

#include <types/MatrixTypes.hpp>

namespace mic {
namespace data_io {

class MazeMatrixImporter : public mic::data_io::Importer< mic::types::MatrixXi , size_t> {
public:
	/*!
	 * Constructor. Sets MNIST image default properties. Registers properties.
	 * @param node_name_ Name of the node in configuration file.
	 */
	MazeMatrixImporter(std::string node_name_ = "maze_importer");

	/*!
	 * Virtual destructor. Empty.
	 */
	virtual ~MazeMatrixImporter() { };

	/*!
	 * Method responsible for importing/loading mazes from files.
	 * @return TRUE if data loaded successfully, FALSE otherwise.
	 */
	bool importData();

	/*!
	 * Method responsible for initialization of all variables that are property-dependent - here not required, yet empty.
	 */
	virtual void initializePropertyDependentVariables() { };

	/// Width of a maze.
	size_t maze_width;

	/// Height of a maze.
	size_t maze_height;

protected:
	/*!
	 * Property: directory/Name of file containing mazes.
	 */
	mic::configuration::Property<std::string> data_filename;

};

} /* namespace data_io */
} /* namespace mic */

namespace mic {
namespace types {

/*!
 * \brief The <MatrixXi-size_t> sample type used by Maze Importer.
 * \author tkornuta
 */
typedef mic::types::Sample<mic::types::MatrixXi, size_t> MazeSample;

/*!
 * \brief The <MatrixXi-size_t> batch type used by Maze Importer.
 * \author tkornuta
 */
typedef mic::types::Batch<mic::types::MatrixXi, size_t> MazeBatch;

}//: namespace types
}//: namespace mic


#endif /* SRC_DATA_IO_MAZEMATRIXIMPORTER_HPP_ */
