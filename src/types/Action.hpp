/*!
 * \file Action.hpp
 * \brief 
 * \author tkornut
 * \date Jan 30, 2016
 */

#ifndef SRC_TYPES_Action_HPP_
#define SRC_TYPES_Action_HPP_

#include <random>

namespace mic {
namespace types {

/*!
 * \brief Types of actions.
 * \author tkornuta
 */
enum NESW_action_type_t
{
	North = 0, ///< Action north.
	East, ///< Action east.
	South, ///< Action south.
	West, ///< Action west.
	None ///< Empty action
};


/*!
 * \brief Abstract interface class representing an action in 2-D space.
 * \author tkornuta
 */
class Action2DInterface {
public:

	/*!
	 * Returns increment along x axis.
	 * @return Increment along x.
	 */
	int dx() { return d_x; };

	/*!
	 * Returns increment along y axis.
	 * @return Increment along y.
	 */
	int dy() { return d_y; };

	/*!
	 * Default constructor. (Re)sets increments. Protected - to be used by derived classes.
	 */
	Action2DInterface() : d_x(0), d_y(0) { };

protected:
	/// Increment according to x axis.
	int d_x;

	/// Increment according to y axis.
	int d_y;

};


/*!
 * \brief Abstract template class representing an action in 2-D space.
 * \tparam actionType Template parameter defining action type.
 * \author tkornuta
 */
template <typename actionType>
class Action2D : public mic::types::Action2DInterface {
public:
	/*!
	 * Virtual destructor. Empty.
	 */
	virtual ~Action2D() { };

	/*!
	 * Abstract method - its implementations should set the increments according to action type.
	 * @param ActionType Type of action.
	 */
	virtual void setAction(actionType type_) = 0;

protected:
	/*!
	 * Default constructor. Calls default constructor of parent Action2DInterface class. Protected - to be used by derived classes.
	 */
	Action2D() : Action2DInterface() { };

};

/*!
 * \brief Class representing an N/E/S/W action.
 * \author tkornuta
 */
class NESWAction : public mic::types::Action2D<NESW_action_type_t> {
public:
	/*!
	 * Default NESW action constructor. Sets increments according to action type.
	 */
	NESWAction(NESW_action_type_t type_) : Action2D() { setAction(type_); };

	/*!
	 * Sets increments according to action value.
	 * @param ActionType Type of action.
	 */
	void setAction(NESW_action_type_t type_) {
		switch (type_) {
		case North:	d_y =-1; d_x =0; break;
		case East:	d_y =0; d_x =1; break;
		case South:	d_y =1; d_x =0; break;
		case West:	d_y =0; d_x =-1; break;
		case None:
		default:	d_y =0; d_x =0;
		}//: switch
	}

protected:
	/*!
	 * NESW action constructor. Empty. Protected - to be used by derived classes.
	 */
	NESWAction() : Action2D() { };

};


/*!
 * \brief Class representing a random N/E/S/W action.
 * \author tkornuta
 */
class RandomNESWAction : public mic::types::NESWAction {
public:
	/*!
	 * Default public random NESW action constructor. Randomly selects action type.
	 */
	RandomNESWAction() : NESWAction(),
		rng_mt19937_64(rd())
	{
		// Initialize uniform integer distribution.
		std::uniform_int_distribution<> index_dist(North, West);

		// Select a random action.
		setAction((NESW_action_type_t) index_dist(rng_mt19937_64));
	};
private:
	/*!
	 * Random device used for generation of random numbers.
	 */
	std::random_device rd;

	/*!
	 *  Mersenne Twister pseudo-random generator of 32-bit numbers with a state size of 19937 bits.
	 */
	std::mt19937_64 rng_mt19937_64;

};


/*!
 * \brief Macro returning NESWAction north.
 * \author tkornuta
 */
#define A_NORTH mic::types::NESWAction(mic::types::North)

/*!
 * \brief Macro returning NESWAction east.
 * \author tkornuta
 */
#define A_EAST mic::types::NESWAction(mic::types::East)

/*!
 * \brief Macro returning NESWAction south.
 * \author tkornuta
 */
#define A_SOUTH mic::types::NESWAction(mic::types::South)

/*!
 * \brief Macro returning NESWAction west.
 * \author tkornuta
 */
#define A_WEST mic::types::NESWAction(mic::types::West)

/*!
 * \brief Macro returning RandomNESWAction .
 * \author tkornuta
 */
#define A_RANDOM mic::types::RandomNESWAction()


} /* namespace types */
} /* namespace mic */

#endif /* SRC_TYPES_Action_HPP_ */
