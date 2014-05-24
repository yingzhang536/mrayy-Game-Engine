// TestPython.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <sstream>
#include <boost/python.hpp>


void printLog(const std::string& message)
{
	printf("Python call: %s\n", message.c_str());
}

class TestHelloWorld
{
public:
	TestHelloWorld()
	{
		printf("Hello world()\n");
	}
	~TestHelloWorld()
	{
		printf("~Hello world()\n");
	}

	void TestFunction()
	{
		printf("TestFunction()\n");

	}
};

BOOST_PYTHON_MODULE(MyLib)
{
	using namespace boost::python;
	def("printlog", printLog, "Write to the log. \n Input: message string \n Output: NULL");
	class_<TestHelloWorld>("TestHW", init<>())
		.def("TestF", &TestHelloWorld::TestFunction);
}
void initPython(std::string pythonLibPath)
{
	std::string pythonHome = pythonLibPath + "/..";

	//Py_SetPythonHome((char*)pythonHome.c_str());
	Py_Initialize();

	initMyLib();
	if (PyErr_Occurred())
		PyErr_Print();
	PyRun_SimpleString("import MyLib ");;

	PyRun_SimpleString("MyLib.printlog(\"hello world from python\") ");;
	PyRun_SimpleString("h=MyLib.TestHW() ");;
	PyRun_SimpleString("h.TestF() ");;
	if (PyErr_Occurred())
		PyErr_Print();
}

int _tmain(int argc, _TCHAR* argv[])
{
	initPython("C:\\python27\\lib");
	return 0;
}

