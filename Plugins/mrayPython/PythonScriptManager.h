



/********************************************************************
	created:	2014/05/14
	created:	14:5:2014   22:10
	filename: 	F:\Development\mrayEngine\Plugins\mrayPython\PythonScriptManager.h
	file path:	F:\Development\mrayEngine\Plugins\mrayPython
	file base:	PythonScriptManager
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __PythonScriptManager__
#define __PythonScriptManager__


#include <IScriptManager.h>


namespace mray
{
namespace script
{
class PythonScriptManager :public IScriptManager
{
protected:
	core::string m_lastErrorString;


protected:
	virtual IScript* createScriptInternal();

	void _exportEngine();
public:

	PythonScriptManager();
	virtual~PythonScriptManager();
	
	virtual void ExecuteFile(OS::IStream* file);

	virtual bool ExectuteScript(const core::string& cmd);
	
};


}
}

#endif
