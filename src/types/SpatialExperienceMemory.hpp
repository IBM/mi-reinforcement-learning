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
 * \file EperienceReplayTable.hpp
 * \brief 
 * \author tkornut
 * \date Apr 26, 2016
 */

#ifndef SRC_ALGORITHMS_EPERIENCEREPLAYTABLE_HPP_
#define SRC_ALGORITHMS_EPERIENCEREPLAYTABLE_HPP_

#include <types/Batch.hpp>
#include <types/Position2D.hpp>
#include <types/MatrixTypes.hpp>


namespace mic {
namespace types {

/*!
 * \brief Structure storing a spatial experience - a triplet of position in time t, executed action and position in time t+1.
 * \author tkornuta
 */
struct SpatialExperience {
	/// State at time t.
	mic::types::Position2D s_t;

	/// Action at time t.
	mic::types::NESWAction a_t;

	/// State at time t+1 (t prim).
	mic::types::Position2D s_t_prim;

	/*!
	 * Default constructor.
	 * @param s_t_ State at time t.
	 * @param a_t_ Action at time t.
	 * @param s_t_prim_ State at time t+1 (t prim).
	 */
	SpatialExperience(mic::types::Position2D s_t_, mic::types::NESWAction a_t_, mic::types::Position2D s_t_prim_) {
		s_t = s_t_;
		a_t = a_t_;
		s_t_prim = s_t_prim_;
	}

};

/*!
 * \brief Shared pointer to spatial experience object.
 * \author tkornuta
 */
typedef std::shared_ptr < mic::types::SpatialExperience> SpatialExperiencePtr;

/*!
 * \brief Spatial experience replay sample.
 * \author tkornuta
 */
typedef  mic::types::Sample<mic::types::SpatialExperience, mic::types::MatrixXf> SpatialExperienceSample;

/*!
 * \brief Spatial experience replay batch.
 * \author tkornuta
 */
typedef  mic::types::Batch<mic::types::SpatialExperience, mic::types::MatrixXf> SpatialExperienceBatch;


/*!
 * \brief Class representing the spatial experience memory - used in memory replay.
 * Derived from the Batch class.
 * \author tkornuta
 *
 */
class SpatialExperienceMemory : public mic::types::SpatialExperienceBatch{
public:

	/*!
	 * Default constructor.
	 * @param number_of_experiences_ The size of the experience table.
	 * @param batch_size_ The batch size.
	 */
	SpatialExperienceMemory(size_t number_of_experiences_, size_t batch_size_) : Batch(batch_size_), number_of_experiences(number_of_experiences_) {

	}

	/*!
	 * Virtual destructor. Empty.
	 */
	virtual ~SpatialExperienceMemory() { }

	/*!
	 * Adds sample to the batch.
	 * If size is exceeded it removes a random sample from the set.
	 * @param input_ Sample input.
	 * @param target_ Sample target.
	 */
	virtual void add(std::shared_ptr<mic::types::SpatialExperience> input_, std::shared_ptr<mic::types::MatrixXf> target_) {

		if (sample_data.size() == number_of_experiences) {
			// Initialize uniform index distribution - integers.
			std::uniform_int_distribution<> index_dist(0, this->sample_data.size()-1);

			// Pick an index.
			unsigned int tmp_index= index_dist(rng_mt19937_64);

			// Erase the sample from all three lists.
			sample_data.erase (sample_data.begin()+tmp_index);
			sample_labels.erase (sample_labels.begin()+tmp_index);
			sample_indices.erase (sample_indices.begin()+tmp_index);
		}

		// Add sample to vectors.
		sample_data.push_back(input_);
		sample_labels.push_back(target_);
		sample_indices.push_back(sample_indices.size());
	}

protected:

	/// Size of the experience table (maximum number of stored experiences).
	size_t number_of_experiences;
};



} /* namespace types */
} /* namespace mic */

#endif /* SRC_ALGORITHMS_EPERIENCEREPLAYTABLE_HPP_ */
