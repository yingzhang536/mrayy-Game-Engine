



/********************************************************************
	created:	2014/05/14
	created:	14:5:2014   22:14
	filename: 	F:\Development\mrayEngine\Plugins\mrayPython\PythonScript.h
	file path:	F:\Development\mrayEngine\Plugins\mrayPython
	file base:	PythonScript
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __PythonScript__
#define __PythonScript__

#include <IScript.h>


namespace mray
{
namespace script
{

	class PythonScriptManager;

class PythonScript :public IScript
{
protected:
	core::string m_script;
	virtual uint calcSizeInternal();

public:

	PythonScript(PythonScriptManager* manager);
	virtual ~PythonScript();

	virtual int SetScript(OS::IStream* file);
	virtual int SetScript(const core::string& s);

	virtual int ExecuteScript();

};

}
}

#endif
