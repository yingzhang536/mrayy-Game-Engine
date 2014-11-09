

#ifndef LeapFunctions_h__
#define LeapFunctions_h__

#include "Leap/Leap.h"

namespace mray
{
	
	static inline math::vector3d LeapToVector3d(const Leap::Vector& vec)
	{
		return math::vector3d(vec.x*0.01f, vec.y*0.01f, vec.z*0.01f);
	}

	static inline void LeapToMatrix(const Leap::Matrix &inM, math::matrix4x4& outM)
	{
		outM.mat[0] = inM.xBasis.x;
		outM.mat[1] = inM.xBasis.y;
		outM.mat[2] = inM.xBasis.z;
		outM.mat[3] = 0;
		outM.mat[4] = inM.yBasis.x;
		outM.mat[5] = inM.yBasis.y;
		outM.mat[6] = inM.yBasis.z;
		outM.mat[7] = 0;
		outM.mat[8] = inM.zBasis.x;
		outM.mat[9] = inM.zBasis.y;
		outM.mat[10] = inM.zBasis.z;
		outM.mat[11] = 0;
		outM.mat[12] = inM.origin.x*0.01f;
		outM.mat[13] = inM.origin.y*0.01f;
		outM.mat[14] = inM.origin.z*0.01f;
		outM.mat[15] = 1;
	}

}

#endif // LeapFunctions_h__
