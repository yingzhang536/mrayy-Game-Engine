

#include "stdafx.h"

#include "boost/python.hpp"

#include "Point2d.h"
#include "Point3d.h"
#include "Point4d.h"

using namespace boost;
using namespace python;

using namespace mray::math;

namespace mray
{
namespace script
{


BOOST_PYTHON_MODULE(Math)
{
	class_<math::vector2d>("vec2d")
		.def(init<>())
		.def(init<float, float>())
		.def(init<const vector2d&>())
		.def("LengthSQ", &vector2d::LengthSQ,"")
		.def("Length", &vector2d::Length, "")
		.def("Dot", &vector2d::dotProduct, "")
		.def("Dist", &vector2d::getDist, "")
		.def("DistSQ", &vector2d::getDistSQ, "")
		.def("Normalize", &vector2d::Normalize, "")
		;

}


void ExportMathTypes()
{
	initMath();
}

}
}
