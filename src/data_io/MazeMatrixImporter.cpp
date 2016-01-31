/*!
 * \file MazeMatrixImporter.cpp
 * \brief 
 * \author tkornut
 * \date Jan 30, 2016
 */

#include <data_io/MazeMatrixImporter.hpp>

#include <fstream>

#include <string>
#include <iostream>
#include <sstream>

namespace mic {
namespace data_io {

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

	int maze_width, maze_height;

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


		LOG(LINFO) << "maze_width=" << maze_width << " maze_height=" << maze_height ;

		// Read third line - second header.
	    line.clear();
		std::getline(data_file, line);
		LOG(LINFO) << "2nd header : " << line ;

		// Read mazes.
	    while (std::getline(data_file, line)) {
			std::cout << line << '\n';

			// Create new matrix of MNIST image size.
			std::shared_ptr< mic::types::Matrix<int> > mat (new mic::types::Matrix<int>(maze_height, maze_width));

			int value;
			size_t i = 0;
			std::stringstream ss(line);
			// Parse line and get consecutire values..
			while (ss >> value) {
				std::cout<< value;
				// Compute matrix index.
				unsigned row = i / maze_width;
				unsigned col = i % maze_height;
				// Set value
				(*mat)(row, col) = value;
				// Increment index.
				i++;

				// Skip comas.
				if (ss.peek() == ',')
					ss.ignore();
			}//: while i in line

			LOG(LINFO) << *mat;
			// Add matrix do vector.
			data.push_back(mat);
		}//: while line


		LOG(LINFO) << "Imported " << data.size() << " mazes";
		LOG(LINFO) << "Data import finished";
		data_file.close();

		return true;
	} else {
		LOG(LFATAL) << "Oops! Couldn't find file: " << data_filename;
		return false;
	}//: else
}

} /* namespace data_io */
} /* namespace mic */
