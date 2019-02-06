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
 * \file MazeMatrixImporter.hpp
 * \brief 
 * \author tkornut
 * \date Jan 30, 2016
 */

#ifndef SRC_importers_MAZEMATRIXIMPORTER_HPP_
#define SRC_importers_MAZEMATRIXIMPORTER_HPP_

#include <importers/Importer.hpp>

#include <types/MatrixTypes.hpp>

namespace mic {
namespace importers {

class MazeMatrixImporter : public mic::importers::Importer< mic::types::MatrixXi , size_t> {
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

} /* namespace importers */
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


#endif /* SRC_importers_MAZEMATRIXIMPORTER_HPP_ */
