
#include "stdafx.h"
#include "boost/python.hpp"

#include "StringUtil.h"

using namespace boost;
using namespace python;

using namespace mray;
using namespace core;

namespace mray
{
namespace script
{


	struct core_string_to_python_str
	{
		static PyObject* convert(core::string const& s)
		{
			return boost::python::incref(boost::python::object(s.c_str()).ptr());
		}
	};

	struct core_string_from_python_str
	{
		core_string_from_python_str()
		{
			boost::python::converter::registry::push_back(
				&convertible,
				&construct,
				boost::python::type_id<core::string>());
		}

		static void* convertible(PyObject* obj_ptr)
		{
			if (!PyString_Check(obj_ptr)) return 0;
			return obj_ptr;
		}

		static void construct(
			PyObject* obj_ptr,
			boost::python::converter::rvalue_from_python_stage1_data* data)
		{
			const char* value = PyString_AsString(obj_ptr);
			if (value == 0) boost::python::throw_error_already_set();
			void* storage = (
				(boost::python::converter::rvalue_from_python_storage<core::string>*)
				data)->storage.bytes;
			new (storage)core::string(value);
			data->convertible = storage;
		}
	};

BOOST_PYTHON_MODULE(Core)
{

// 	class_<string>("cstring")
// 		.def("Compare", &core::string::equals_ignore_case, "Compare this string with another string, ignoring case")
// 		.def("ReplaceChar", &core::string::replaceChar, "Replace characters with another characters")
// 		.def("FindLast", &core::string::findlast, "Find last occurrence for a character, and return the index")
// 		;
	//export string
	boost::python::to_python_converter<
		core::string,
		core_string_to_python_str>();
	core_string_from_python_str();


	//StringUtil
	def("SplitPathFileName", &StringUtil::SplitPathFileName, "Split a path to directory and a file name");
	def("SplitPathExt", &StringUtil::SplitPathExt, "Split a file name to path and extension");
	def("ToLower", &StringUtil::ToLower, "Convert string to lower case");
	def("BeginsWith", &StringUtil::BeginsWith, "Check if a string begin with another string");
	def("MakePathRelative", &StringUtil::MakePathRelative, "Make a path relative to root directory");
	def("NormalizePathSlashes", &StringUtil::NormalizePathSlashes, "Normalize path");
	def("Trim", &StringUtil::Trim, "Trim a string to remove certain characters");
	def("Split", &StringUtil::Split, "Split a string to list of strings");

}


void ExportCoreTypes()
{
	initCore();
}

}
}