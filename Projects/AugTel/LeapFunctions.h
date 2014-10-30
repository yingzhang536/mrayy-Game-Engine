

#ifndef LeapFunctions_h__
#define LeapFunctions_h__

#include "Leap/Leap.h"

namespace mray
{
	
	math::vector3d LeapToVector3d(const Leap::Vector& vec)
	{
		return math::vector3d(vec.x, vec.y, vec.z);
	}

}

#endif // LeapFunctions_h__
