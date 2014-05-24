

#include "stdafx.h"
#include "PythonScriptManager.h"

#include "boost/python.hpp"
#include "PythonScript.h"

#include <TraceManager.h>



namespace mray
{
namespace script
{

	void ExportCoreTypes();
	void ExportMathTypes();

	void PythonScriptManager::_exportEngine()
	{
		ExportCoreTypes();
		ExportMathTypes();
	}
PythonScriptManager::PythonScriptManager()
{
	std::string pythonHome =  "C:\\python27";
	Py_SetPythonHome((char*)pythonHome.c_str());
	Py_Initialize();

	_exportEngine();
}

PythonScriptManager::~PythonScriptManager()
{
	Py_Finalize();
}
IScript* PythonScriptManager::createScriptInternal()
{
	return new PythonScript(this);
}



void PythonScriptManager::ExecuteFile(OS::IStream* file)
{
	if (!file)
		return;
	traceFunction(Script);
	file->seek(0, OS::ESeek_End);
	int len = file->getPos() / sizeof(char);
	file->seek(0, OS::ESeek_Set);

	char *scriptTxt = new char[len + 1];

	file->read(scriptTxt, len);
	scriptTxt[len] = 0;

	PyRun_SimpleString(scriptTxt);
	if (PyErr_Occurred())
		PyErr_Print();

	delete[] scriptTxt;
}


bool PythonScriptManager::ExectuteScript(const core::string& cmd)
{

	PyRun_SimpleString(cmd.c_str());
	if (PyErr_Occurred())
	{

		PyErr_Print();
		return false;
	}
	return true;
}


}
}


