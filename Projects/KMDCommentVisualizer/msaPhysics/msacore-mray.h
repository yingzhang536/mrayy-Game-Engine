#pragma once

//#include "cinder-lite/CinderMath.h"
//#include "cinder-lite/Color.h"
//#include "cinder-lite/Vector.h"

#include <string>
#include <vector>
#include <map>

#include <point2d.h>
#include <point3d.h>
#include <point4d.h>

using namespace mray;

//windows
#ifndef M_PI
#    define M_PI 3.14159265358979323846
#endif

using namespace std;

namespace msa {
	
#define MSA_HOST_SUFFIX		"-OF"
	
#if defined (TARGET_OSX)
#define MSA_TARGET_OSX
	
#elif defined (TARGET_LINUX)
#define MSA_TARGET_LINUX
	
#elif defined (TARGET_WIN32)
#define MSA_TARGET_WIN32
	
#elif defined (TARGET_IPHONE)
#define MSA_TARGET_IPHONE)
#endif
	
#if defined (TARGET_OPENGLES)
#define MSA_TARGET_OPENGLES
#endif

	class Vec2f:public math::vector2d
	{
	public:
		Vec2f(){}
		Vec2f(const math::vector2d& o) :math::vector2d(o){}
		Vec2f(float x, float y) :math::vector2d(x, y){}
		static const int DIM = 2;
		float operator [] (int i)const{
			if (i == 0)return x;
			if (i == 1)return y;
			return 0;
		}
		float& operator [] (int i){
			if (i == 0)return x;
			if (i == 1)return y;
		}
		Vec2f operator - ()const{
			return Vec2f(-x, -y);
		}
		Vec2f operator - (const Vec2f& o)const{
			return Vec2f(x-o.x, y-o.y);
		}

		void limit(float v)
		{
			if (x > v)x = v;
			if (y > v)y = v;
		}
	};
	class Vec3f :public math::vector3d
	{
	public:
		Vec3f(){}
		Vec3f(const math::vector3d& o) :math::vector3d(o){}
		Vec3f(float x, float y, float z) :math::vector3d(x, y, z){}
		static const int DIM = 3;

	};

	class Vec4f :public math::vector4d
	{
	public:
		static const int DIM = 4;
	};
	
    typedef video::SColor    Color;
    
    inline float clamp(float a, float min, float max) {
        return math::clamp<float>(a, min, max);
    }
    
    
	class Rand {
	public:
		
		static float randFloat()									{	return math::Randomizer::rand01(); }
		static float randFloat(float f)								{ return math::Randomizer::randRange(0, f); }
		static float randFloat(float a, float b)					{ return math::Randomizer::randRange(a, b); }
		
		//! returns a random Vec3f that represents a point on the unit circle
		static Vec3f randVec3f() {
			float phi = randFloat( (float)M_PI * 2.0f );
			float costheta = randFloat( -1.0f, 1.0f );
			
			float rho = sqrt( 1.0f - costheta * costheta ); 
			float x = rho * cos( phi );
			float y = rho * sin( phi );
			float z = costheta;
			
			return Vec3f( x, y, z );
		}
		
		//! returns a random Vec2f that represents a point on the unit circle
		static Vec2f randVec2f() {
			float theta = randFloat( (float)M_PI * 2.0f );
			return Vec2f( cos( theta ), sin( theta ) );
		}
	};
	
	
}