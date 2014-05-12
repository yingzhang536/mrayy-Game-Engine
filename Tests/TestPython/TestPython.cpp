// TestPython.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string>
#include <sstream>
#include <boost/python.hpp>

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

	if (PyErr_Occurred())
		PyErr_Print();
	setupPython();
}

int _tmain(int argc, _TCHAR* argv[])
{
	initPython("C:\\Users\\telesar\\Documents\\SmartBody\\python27\\lib");
	return 0;
}

