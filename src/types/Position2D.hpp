/*!
 * \file Position.hpp
 * \brief 
 * \author tkornut
 * \date Mar 18, 2016
 */

#ifndef SRC_TYPES_POSITION2D_HPP_
#define SRC_TYPES_POSITION2D_HPP_

#include <types/Action2D.hpp>

namespace mic {
namespace types {

/*!
 * \brief Class representing position in 2-D space.
 * \author tkornuta
 */
class Position2D {
public:

	/// The x coordinate.
	long x;

	/// The y coordinate.
	long y;

	/*!
	 * Default constructor. Sets position to (0,0).
	 */
	Position2D() : x(0), y(0) { };

	/*!
	 * Constructor. Sets position to (0,0).
	 */
	Position2D(long x_, long y_) : x(x_), y(y_) { };

	/*!
	 * Sets position coodrinates.
	 * @param x_ The x coordinate.
	 * @param y_ The y coordinate.
	 */
	void set(long x_, long y_) {
		x = x_;
		y = y_;
	}

	/*!
	 * Operator returning new position being the result of the taken action from given position.
	 * @param ac_ The action to be performed.
	 * @return New, resulting position.
	 */
	mic::types::Position2D operator+(Action2DInterface ac_) {
		mic::types::Position2D new_pos;
		new_pos.x = this->x + ac_.dx;
		new_pos.y = this->y + ac_.dy;
		return new_pos;
	}

	/*!
	 * Comparison operator.
	 * @param pos_ The compared position.
	 * @return True if positions are "equal".
	 */
	bool operator==(mic::types::Position2D pos_) {
		mic::types::Position2D new_pos;
		if ((pos_.x == this->x) && (pos_.y == this->y))
			return true;
		else
			return false;
	}

	/*!
	 * Performs "deterministic" move in 2D.
	 * @param ac_ The action to be performed.
	 * @return True if move was performed, false if it was not possible.
	 */
	bool move(Action2DInterface ac_) {
		x += ac_.dx;
		y += ac_.dy;
		return true;
	}

	/*!
	 * Performs "deterministic" move in 2D.
	 * Depending on the value of circular_world_ flag, exceeding the boundaries will result in moving to the "other side" of the space (if true) or return the information that the move was not possible.
	 *
	 * @param ac_ The action to be performed.
	 * @param width_ Width of the "2D space".
	 * @param height_ Height of the "2D space".
	 * @param circular_world_ Flag determining whether exceeding the boundaries will result in moving to the "other side" of the space (if true) or return the information that the move was not possible.
	 * @return True if move was performed, false if it was not possible.
	 */
	bool move(Action2DInterface ac_, size_t width_, size_t height_, bool circular_world_ = true) {
		if (circular_world_) {
			// Execute the "circular" move.
			x = (x + width_ +  ac_.dx) % width_;
			y = (y + height_ +  ac_.dy) % height_;
			return true;
		} else {
			// Execute the "truncated move.
			if (((x+ac_.dx) < 0) || ((x+ac_.dx) >= width_))
				return false;

			if (((y+ac_.dy) < 0) || ((y+ac_.dy) >= height_))
					return false;

			x += ac_.dx;
			y += ac_.dy;

			return true;
		}//: else
	}

	/*!
	 * Returns ostream containing description of given position.
	 * @param os_ Ostream.
	 * @param pos_ Position.
	 * @return Returned ostream object.
	 */
	friend std::ostream& operator<<(std::ostream& os_, const Position2D& pos_)
	{
	    os_ << "[x,y]: [" << pos_.x << ',' << pos_.y << "]";
	    return os_;
	}
};



} /* namespace types */
} /* namespace mic */

#endif /* SRC_TYPES_POSITION2D_HPP_ */
