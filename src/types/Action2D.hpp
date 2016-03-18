/*!
 * \file Action.hpp
 * \brief 
 * \author tkornut
 * \date Jan 30, 2016
 */

#ifndef SRC_TYPES_Action_HPP_
#define SRC_TYPES_Action_HPP_

#include <random>
#include <iostream>

namespace mic {
namespace types {

/*!
 * \brief Enumeration of possible types of actions in 2D.
 * \author tkornuta
 */
enum class NESW : short
{
	North = 0, ///< Action north.
	East, ///< Action east.
	South, ///< Action south.
	West, ///< Action west.
	None, ///< Empty action
	Random, ///< Random action
	Exit ///< Exit action
};


/*!
 * \brief Interface class representing an action in 2-D space.
 * \author tkornuta
 */
class Action2DInterface {
public:

	/// Increment according to x axis.
	int dx;

	/// Increment according to y axis.
	int dy;

	/*!
	 * Default constructor. (Re)sets increments. Protected - to be used by derived classes.
	 */
	Action2DInterface() : dx(0), dy(0) { };

	/*!
	 * Returns ostream containing description of given position.
	 * @param os_ Ostream.
	 * @param pos_ Position.
	 * @return Returned ostream object.
	 */
	friend std::ostream& operator<<(std::ostream& os_, const Action2DInterface& ac_)
	{
	    os_ << "[dx,dy]: [" << ac_.dx << ',' << ac_.dy << "]";
	    return os_;
	}

protected:
};


/*!
 * \brief Abstract template class representing an action in 2-D space.
 * \tparam actionType Template parameter defining action type.
 * \author tkornuta
 */
template <typename ActionType>
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
	virtual void setAction(ActionType type_) = 0;

protected:
	/*!
	 * Type of the performed action.
	 */
	ActionType type;

	/*!
	 * Default constructor. Calls default constructor of parent Action2DInterface class. Protected - to be used by derived classes.
	 */
	Action2D() : Action2DInterface() { };

};

/*!
 * \brief Class representing an N/E/S/W action.
 * \author tkornuta
 */
class NESWAction : public mic::types::Action2D<NESW> {
public:
	/*!
	 * Default NESW action constructor. Sets increments according to action type.
	 */
	NESWAction(NESW type_) : Action2D() { setAction(type_); };

	/*!
	 * Sets increments according to action value.
	 * @param ActionType Type of action.
	 */
	void setAction(NESW type_) {
		type = type_;
		switch (type_) {
		case NESW::North:	dy =-1; dx =0; break;
		case NESW::East:	dy =0; dx =1; break;
		case NESW::South:	dy =1; dx =0; break;
		case NESW::West:	dy =0; dx =-1; break;
		case NESW::None:
		case NESW::Exit:
		default:	dy =0; dx =0;
		}//: switch
	}

protected:
	/*!
	 * NESW action constructor. Empty. Protected - to be used by derived classes.
	 */
	NESWAction() : Action2D()  { type = NESW::None; };

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
		type = NESW::Random;

		// Initialize uniform integer distribution.
		std::uniform_int_distribution<> index_dist((int)NESW::North, (int)NESW::West);

		// Select a random action.
		setAction((NESW) index_dist(rng_mt19937_64));
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
 * \brief Class representing an exit action.
 * \author tkornuta
 */
class ExitAction : public mic::types::NESWAction {
public:
	/*!
	 * Exit action constructor. Empty.
	 */
	ExitAction() : NESWAction() { type = NESW::Exit; };
};


/*!
 * \brief Macro returning NESWAction north.
 * \author tkornuta
 */
#define A_NORTH mic::types::NESWAction(mic::types::NESW::North)

/*!
 * \brief Macro returning NESWAction east.
 * \author tkornuta
 */
#define A_EAST mic::types::NESWAction(mic::types::NESW::East)

/*!
 * \brief Macro returning NESWAction south.
 * \author tkornuta
 */
#define A_SOUTH mic::types::NESWAction(mic::types::NESW::South)

/*!
 * \brief Macro returning NESWAction west.
 * \author tkornuta
 */
#define A_WEST mic::types::NESWAction(mic::types::NESW::West)

/*!
 * \brief Macro returning RandomNESWAction .
 * \author tkornuta
 */
#define A_RANDOM mic::types::RandomNESWAction()

/*!
 * \brief Macro returning RandomNESWAction .
 * \author tkornuta
 */
#define A_EXIT mic::types::ExitAction()



} /* namespace types */
} /* namespace mic */

#endif /* SRC_TYPES_Action_HPP_ */
