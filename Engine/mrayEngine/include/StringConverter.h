 
#ifndef ___StringConverter___
#define ___StringConverter___

#include <iostream>
#include <string>
#include <sstream>
#include "mString.h"


#include "Point4d.h"
#include "SColor.h"








#include "UTFString.h"

namespace mray{
namespace core{

	
class MRAY_DLL StringConverter
{
#ifdef UNICODE
	typedef std::wostringstream osstream;
#else
	typedef std::ostringstream osstream;
#endif

public:
	//Convert To String Methods
	static core::string toString(const float&v,int precision=8) ;
	static core::string toString(const bool&v,bool trueFalse=0) ;
	static core::string toString(const math::Point4d<int>&v,int precision=8)  ;
	static core::string toString(const math::Point4d<float>&v,int precision=8)  ;
	static core::string toString(const math::Point3d<float>&v,int precision=8)  ;
	static core::string toString(const math::Point2d<float>&v,int precision=8) ;
	static core::string toString(const math::line2d&v) ;
	static core::string toString(const math::line3d&v) ;
	static core::string toString(const math::box3d&v) ;
	static core::string toString(const math::Plane&v) ;
	static core::string toString(const video::SColor &v)  ;
	static core::string toString(const math::quaternion &v)  ;
	static core::string toString(const math::matrix4x4 &v)  ;
	static core::string toString(const math::matrix3x3 &v)  ;
	static core::string toString(const UTFString &v)  ;

	static const core::string&toString(int v,const core::string&enumType);

	template <class T>
	static core::string toString(const T&v) 
	{
		osstream stream;
		stream << v;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const math::Point3d<T>&v) 
	{
		osstream stream;
		stream << v.x <<mT(",")<<v.y<<mT(",")<<v.z;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const math::Point2d<T>&v) 
	{
		osstream stream;
		stream << v.x <<mT(",")<<v.y;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const  wchar_t*&v) 
	{
		osstream stream;
		stream << v;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const char*&v) 
	{
		osstream stream;
		stream << v;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const math::line2d&v) 
	{
		osstream stream;
		stream << v.pStart.x <<mT(",")<<v.pStart.y<<mT(",")<<v.pEnd.x<<mT(",")<<v.pEnd.y;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const math::line3d&v) 
	{
		osstream stream;
		stream	<< v.pStart.x <<mT(",")<<v.pStart.y<<mT(",")<<v.pStart.z
			<<mT(",")<<v.pEnd.x<<mT(",")<<v.pEnd.y<<mT(",")<<v.pEnd.z;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const math::rect<T>&v) 
	{
		osstream stream;
		stream	<< v.ULPoint.x <<mT(",")<<v.ULPoint.y<<mT(",")
			<<v.BRPoint.x<<mT(",")<<v.BRPoint.y;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const math::box3d&v) 
	{
		osstream stream;
		stream	<< v.MinP.x <<mT(",")<<v.MinP.y<<mT(",")<<v.MinP.z<<mT(",")
			<<v.MaxP.x<<mT(",")<<v.MaxP.y<<mT(",")<<v.MaxP.z;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const math::Plane&v) 
	{
		osstream stream;
		stream	<< v.Normal.x <<mT(",")<<v.Normal.y<<mT(",")<<v.Normal.z<<mT(",")<<v.D;

		return stream.str().c_str();
	};
	template <class T>
	static core::string toString(const tstring<T>&v) 
	{
		osstream stream;
		stream	<< v.c_str();

		return stream.str().c_str();
	};

	template <class T>
	static core::string ArrayToString(const std::vector<T>& arr)
	{
		core::string ret;
		for (int i=0;i<arr.size();++i)
		{
			ret+=toString(arr[i]);
			if(i!=arr.size()-1)
				ret+=mT(",");
		}
		return ret;
	}

	/// Parse String Methods
	static int toInt(const core::string&str);
	static float toFloat(const core::string&str);
	static double toDouble(const core::string&str);
	static uint toUInt(const core::string&str);
	static long toLong(const core::string&str);
	static ulong toULong(const core::string&str);
	static bool toBool(const core::string&str);
	static math::vector2d toVector2d(const core::string&str);
	static math::vector3d toVector3d(const core::string&str);
	static math::vector4d toVector4d(const core::string&str);
	static video::SColor toColor(const core::string&str);
	static math::quaternion toQuaternion(const core::string&str);
	static math::matrix4x4 toMatrix(const core::string&str);
	static math::line2d toLine2d(const core::string&str);
	static math::line3d toLine3d(const core::string&str);
	static math::rectf toRect(const core::string&str);
	static math::box3d toBox3d(const core::string&str);
	static math::Plane toPlane(const core::string&str);
	static core::UTFString toUTFString(const core::string&str);

	static int toEnum(const core::string&enumType,const core::string&enumVal);

	static core::string toString(const core::stringc&str);
	static core::string toString(const core::stringw&str);
	static core::string toString(const mchar*str);

	static void parse(const core::string&str,char&v);
	static void parse(const core::string&str, wchar_t&v);
	static void parse(const core::string&str,int&v);
	static void parse(const core::string&str,double&v);
	static void parse(const core::string&str,float&v);
	static void parse(const core::string&str,uint&v);
	static void parse(const core::string&str,long&v);
	static void parse(const core::string&str,ulong&v);
	static void parse(const core::string&str,bool&v);
	static void parse(const core::string&str,math::vector2d&v);
	static void parse(const core::string&str,math::vector2di&v);
	static void parse(const core::string&str,math::vector3d&v);
	static void parse(const core::string&str,math::vector3di&v);
	static void parse(const core::string&str,math::vector4d&v);
	static void parse(const core::string&str,math::vector4di&v);
	static void parse(const core::string&str,video::SColor&v);
	static void parse(const core::string&str,math::quaternion&v);
	static void parse(const core::string&str,math::matrix4x4&v);
	static void parse(const core::string&str,math::line2d&v);
	static void parse(const core::string&str,math::line3d&v);
	static void parse(const core::string&str,math::rect<float>&v);
	static void parse(const core::string&str,math::box3d&v);
	static void parse(const core::string&str,math::Plane&v);
	static void parse(const core::string&str,string&v);

	static void parse(const core::string&str, UTFString &v) ;

	static void parse(const core::string&str,int&v,const core::string&enumType);
	template <class T>
	static void parse(const core::string&str,const T&v){}
	//	static void parse(const core::string&str,string&v){v=(str.c_str()); }

	template<class T>
	static std::vector<T> ParseArray(const core::string& arr)
	{
		std::vector<core::string> splitArr;
		core::string::split(arr.c_str(),splitArr,mT(","));
		std::vector<T> retArr;
		for(int i=0;i<splitArr.size();++i)
		{
			T v;
			parse(splitArr[i],v);
			retArr.push_back(v);
		}
		return retArr;
	}
};

}
}


#endif


