

#ifndef math_h__
#define math_h__

#include <math.h>

const float RoundingValue = 0.00005f;
const double PI64 = 3.1415926535897932384626433832795028841971693993751;
const double TwoPI64 = PI64*2.0;
const double HalfPI64 = PI64*0.5;
const double OneFourthPI64 = PI64*0.25;


template <class T>
inline T Max(const T a, const T b)
{
	return  a < b ? b : a;
}
template <class T>
inline T Min(const T a, const T b)
{
	return a < b ? a : b;
}

inline double toRad(double a){ return a*PI64 / 180; }
inline double toDeg(double a){ return a * 180 / PI64; }

template <class T>
inline T clamp(T x, T low, T high)
{
	return Min(Max(x, low), high);
}
struct Vector2
{
	Vector2() :x(0), y(0), z(0)
	{
	}
	float  x, y, z;
};
struct Vector3
{
	Vector3() :x(0), y(0), z(0)
	{
	}
	float x, y, z;
};

struct Quaternion
{
	Quaternion() :x(0), y(0), z(0), w(1)
	{
	}
	float w, x, y, z;
	void fromEulerAngles(float pitch, float yaw, float roll)
	{

		float num = toRad(roll * 0.5f);
		float num2 = (float)sin((double)num);
		float num3 = (float)cos((double)num);
		float num4 = toRad(pitch * 0.5f);
		float num5 = (float)sin((double)num4);
		float num6 = (float)cos((double)num4);
		float num7 = toRad(yaw * 0.5f);
		float num8 = (float)sin((double)num7);
		float num9 = (float)cos((double)num7);
		x = num9 * num5 * num3 + num8 * num6 * num2;
		y = num8 * num6 * num3 - num9 * num5 * num2;
		z = num9 * num6 * num2 - num8 * num5 * num3;
		w = num9 * num6 * num3 + num8 * num5 * num2;

	}

	Vector3 toEulerAngles()const
	{
		double sqw = w*w;
		double sqx = x*x;
		double sqy = y*y;
		double sqz = z*z;
		Vector3 angles;
		angles.x = (float)(atan2(2.0 * (y*z + x*w), (-sqx - sqy + sqz + sqw)));

		// rotation about y-axis
		angles.y = (float)(asin(clamp<double>(2.0 * (x*z + y*w), -1, 1)));

		// rotation about z-axis
		angles.z = (float)(atan2(2.0 * (x*y + z*w), (sqx - sqy - sqz + sqw)));
		/*
		angles.y = (float)(asin(clamp<double>(2.0 * (x*z - y*w), -1, 1)));

		// rotation about y-axis

		angles.x = (float)(atan2(2.0 * (y*z + x*w), 1-2*(sqz + sqw)));


		// rotation about z-axis
		angles.z = (float)(atan2(2.0 * (x*y + z*w), 1 - 2 * (sqy + sqz)));*/

		angles.x = (float)toDeg(angles.x);
		angles.y = (float)toDeg(angles.y);
		angles.z = (float)toDeg(angles.z);

		return angles;
	}
};



#endif // math_h__