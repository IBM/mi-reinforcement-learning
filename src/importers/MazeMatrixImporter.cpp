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
 * \file MazeMatrixImporter.cpp
 * \brief 
 * \author tkornut
 * \date Jan 30, 2016
 */

#include <importers/MazeMatrixImporter.hpp>

#include <fstream>

#include <string>
#include <iostream>
#include <sstream>

namespace mic {
namespace importers {

MazeMatrixImporter::MazeMatrixImporter(std::string node_name_) : Importer (node_name_),
		data_filename("data_filename","mazes.csv")
{
	// Register properties - so their values can be overridden (read from the configuration file).
	registerProperty(data_filename);
}

bool MazeMatrixImporter::importData(){
	LOG(LSTATUS) << "Importing mazes from file: " << data_filename;

	std::ifstream data_file(data_filename);

	std::string line;

	// If managed to open the file properly.
	if (data_file.is_open()) {

		// Read first line - header.
	    line.clear();
		std::getline(data_file, line);

		// Try to read the maze dimensions.
	    line.clear();
		std::getline(data_file, line);

		std::stringstream ss(line);

		ss >> maze_width;
		ss.ignore();// ignore coma (,)
		ss >> maze_height;


		LOG(LDEBUG) << "maze_width=" << maze_width << " maze_height=" << maze_height ;

		// Read third line - second header.
	    line.clear();
		std::getline(data_file, line);
		LOG(LDEBUG)  << "2nd header : " << line ;

		// Read mazes.
	    while (std::getline(data_file, line)) {
	    	LOG(LDEBUG)  << line << '\n';

			// Create new matrix of MNIST image size.
			mic::types::MatrixXiPtr mat (new mic::types::MatrixXi(maze_height, maze_width));

			int value;
			size_t i = 0;
			std::stringstream ss(line);
			// Parse line and get consecutire values.
			while (ss >> value) {
				// Compute matrix index.
				unsigned col = i / maze_width;
				unsigned row = i - ( col * maze_width);
				// Set value
				(*mat)(col, row) = value;
				LOG(LDEBUG) << " " << i <<"("<< col<<","<< row <<")|" << value;
				// Increment index.
				i++;

				// Skip comas.
				if (ss.peek() == ',')
					ss.ignore();
			}//: while i in line

			LOG(LDEBUG) << *mat;
			// Add matrix do vector.
			sample_data.push_back(mat);
		}//: while line


		LOG(LINFO) << "Imported " << sample_data.size() << " mazes of size (h x w) = " << maze_height << " x " << maze_width;

		// Fill the labels and indices tables.
		for (size_t i=0; i < sample_data.size(); i++ ){
			sample_labels.push_back( std::make_shared <size_t> (i) );
			sample_indices.push_back(i);
		}

		LOG(LINFO) << "Data import finished";
		data_file.close();

		return true;
	} else {
		LOG(LFATAL) << "Oops! Couldn't find file: " << data_filename;
		return false;
	}//: else
}

} /* namespace importers */
} /* namespace mic */
