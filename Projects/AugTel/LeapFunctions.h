

#ifndef LeapFunctions_h__
#define LeapFunctions_h__

#include "Leap/Leap.h"

namespace mray
{
#define LEAP_SCALER 0.001f
	static inline math::quaternion QuatLookRotation(const math::vector3d& lookAt, math::vector3d upDirection);


	static inline math::vector3d LeapToVector3d(const Leap::Vector& vec)
	{
		return math::vector3d(vec.x*LEAP_SCALER, -vec.z*LEAP_SCALER, vec.y*LEAP_SCALER);
	}
	static inline math::vector3d LeapToVector3dFlipZ(const Leap::Vector& vec)
	{
		return math::vector3d(vec.x, vec.y, -vec.z);
	}
	static inline math::vector3d LeapToDirection(const Leap::Vector& vec)
	{
		return math::vector3d(vec.x, vec.y, vec.z);
	}

	static inline math::quaternion LeapToQuaternion(const Leap::Matrix &m)
	{
		math::vector3d up = LeapToVector3dFlipZ(m.transformDirection(Leap::Vector(0, 1, 0)));
		math::vector3d front = LeapToVector3dFlipZ(m.transformDirection(Leap::Vector(0, 0, -1)));
// 		math::quaternion q;
// 		math::vector3d side = up.crossProduct(front);
		return QuatLookRotation(front,up);// math::MathUtil::CreateLookAtMatrix(0, front, up);
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
		outM.mat[12] = 0;// inM.origin.x*0.01f;
		outM.mat[13] = 0;//inM.origin.y*0.01f;
		outM.mat[14] = 0;//-inM.origin.z*0.01f;
		outM.mat[15] = 1;

		math::vector3d up = outM*math::vector3d::YAxis;
		math::vector3d front = outM*(-math::vector3d::ZAxis);
		outM = math::MathUtil::CreateLookAtMatrix(0, front, up);
		outM.mat[12] =  inM.origin.x*LEAP_SCALER;
		outM.mat[13] = inM.origin.y*LEAP_SCALER;
		outM.mat[14] = -inM.origin.z*LEAP_SCALER;
	}

	static inline void OrthoNormalize(math::vector3d& n, math::vector3d& u)
	{
		math::vector3d v;
		n.Normalize();
		v = n.crossProduct(u);
		v.Normalize();
		u = v.crossProduct( n);
	}
	static inline math::quaternion QuatLookRotation(const math::vector3d& lookAt, math::vector3d upDirection) {
		/*Vector forward = lookAt.Normalized();
		Vector right = Vector::Cross(up.Normalized(), forward);
		Vector up = Vector::Cross(forward, right);*/
		math::vector3d forward = lookAt; 
		math::vector3d up = upDirection;
		OrthoNormalize(forward, up);
		math::vector3d right = up.crossProduct(forward);

		math::quaternion ret;
		ret.w = sqrtf(1.0f + right.x + up.y + forward.z) * 0.5f;
		float w4_recip = 1.0f / (4.0f * ret.w);
		ret.x = (forward.y - up.z) * w4_recip;
		ret.y = (right.z - forward.x) * w4_recip;
		ret.z = (up.x - right.y) * w4_recip;

		return ret;
	}
}

#endif // LeapFunctions_h__
