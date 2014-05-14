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
BOOST_PYTHON_MODULE(MyLib)
{
	boost::python::def("printlog", printLog, "Write to the log. \n Input: message string \n Output: NULL");

}
void setupPython()
{
	try {
		PyRun_SimpleString("class WritableObject:\n\tdef __init__(self):\n\t\tself.content = []\n\tdef write(self, string):\n\t\tprintlog(string)\n");
		PyRun_SimpleString("logwriter = WritableObject()");
		PyRun_SimpleString("sys.stdout = logwriter");
		PyRun_SimpleString("sys.stderr = logwriter");

		if (PyErr_Occurred())
			PyErr_Print();


		//LOG("before import os");
		PyRun_SimpleString("from os import *");
		//LOG("before import Smartbody");
		PyRun_SimpleString("from SmartBody import *");
		//LOG("before import pydoc");
		PyRun_SimpleString("from pydoc import *");
#if 1
		PyRun_SimpleString("scene = getScene()");
		PyRun_SimpleString("bml = scene.getBmlProcessor()");
		PyRun_SimpleString("sim = scene.getSimulationManager()");
		PyRun_SimpleString("assets = scene.getAssetManager()");
#endif
		//LOG("After import os");

		if (PyErr_Occurred())
			PyErr_Print();


	}
	catch (...) {
		PyErr_Print();
		//LOG("PyError Exception");
	}
}

void initPython(std::string pythonLibPath)
{
	std::string pythonHome = pythonLibPath + "/..";

	Py_SetPythonHome((char*)pythonHome.c_str());
	Py_Initialize();

	PyRun_SimpleString("import sys");
	// set the proper python path
	std::stringstream strstr;
	strstr << "sys.path.append(\"";
	strstr << pythonLibPath;
	strstr << "\");";
	PyRun_SimpleString(strstr.str().c_str());

	// add path to site-packages
	std::string pythonSitePackagePath = pythonLibPath + "/site-packages";
	strstr.str(std::string());
	strstr.clear();
	strstr << "sys.path.append(\"";
	strstr << pythonSitePackagePath;
	strstr << "\");";
	PyRun_SimpleString(strstr.str().c_str());

	// add path to DLLs
	std::string pythonDLLPath = pythonLibPath + "/../DLLs";
	strstr.str(std::string());
	strstr.clear();
	strstr << "sys.path.append(\"";
	strstr << pythonDLLPath;
	strstr << "\");";
	PyRun_SimpleString(strstr.str().c_str());
	initMyLib();
	if (PyErr_Occurred())
		PyErr_Print();

	setupPython();
	PyRun_SimpleString("printlog(\"hello world from python\"");;
}

int _tmain(int argc, _TCHAR* argv[])
{
	initPython("C:\\Users\\myamens\\Documents\\SmartBody\\python27\\lib");
	return 0;
}

