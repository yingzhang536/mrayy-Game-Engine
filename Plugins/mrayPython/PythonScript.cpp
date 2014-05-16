

#include "stdafx.h"
#include "PythonScript.h"
#include <TraceManager.h>
#include <ILogManager.h>

#include "boost/python.hpp"

namespace mray
{
namespace script
{

	
PythonScript::PythonScript(PythonScriptManager* manager)
{
}
PythonScript::~PythonScript()
{
}


uint PythonScript::calcSizeInternal()
{
	return sizeof(PythonScript);
}
int PythonScript::SetScript(OS::IStream* file)
{
	traceFunction(Script);
	if (!file)
		return -1;
	file->seek(0, OS::ESeek_End);
	int len = file->getPos() / sizeof(char);
	file->seek(0, OS::ESeek_Set);

	char *scriptTxt = new char[len + 1];

	file->read(scriptTxt, len);
	scriptTxt[len] = 0;
	int res = SetScript(scriptTxt);
	delete[] scriptTxt;
	return res;
}
int PythonScript::SetScript(const core::string& s)
{
	m_script = s;
	return 1;
}

int PythonScript::ExecuteScript()
{
	PyRun_SimpleString(m_script.c_str());

	if (PyErr_Occurred())
	{
		PyObject *ptype, *pvalue, *ptraceback;
		PyErr_Fetch(&ptype, &pvalue, &ptraceback);
		char *pStrErrorMessage = PyString_AsString(pvalue);
		gLogManager.log("PythonScript::ExecuteScript()- ", pStrErrorMessage, ELL_WARNING);
		return -1;
	}
	return 1;
}

}
}


