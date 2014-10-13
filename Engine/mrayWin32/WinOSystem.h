/********************************************************************
	created:	2009/01/02
	created:	2:1:2009   18:08
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\WinOSystem.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	WinOSystem
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___WinOSystem___
#define ___WinOSystem___

#include <IOSystem.h>
#include "CompileConfig.h"
#include <IThreadManager.h>
#include <IFileSystem.h>
#include <IDllManager.h>
#include <IOSClipboard.h>
#include <IOSRegistry.h>

namespace mray{
namespace OS{

class MRAY_Win32_DLL WinOSystem:public IOSystem
{
private:
	GCPtr<IThreadManager> m_threadManager;
	GCPtr<IFileSystem> m_fileSystem;
	GCPtr<IDllManager> m_dllManager;
	GCPtr<IOSClipboard> m_clipboardManager;
	GCPtr<IOSRegistry> m_registryManager;
	GCPtr<ISerialPortService> m_serialportService;

	std::vector<uint> m_processorsAffineMask;
public:

	WinOSystem();

	virtual ~WinOSystem();

	virtual IDirOS* createDirSystem();
	virtual ITimer* createTimer() ;

	virtual IOSClipboard* getClipboard(){ return m_clipboardManager; }

	virtual bool getMemoryState(SMemoryState& state);
	virtual bool getPowerState(SPowerStatus& state);
	virtual int getProcessorsCount();
	virtual void AffinitizeThreadToProcessor(uint pid);

	virtual ISystemProcess* CreateSystemProcess();

	virtual ulong HandleWindowEvent(const OptionContainer& params);

	virtual IFileMonitor* CreateFileMonitor() ;

	virtual ISerialPortService* GetSerialPortService(){ return m_serialportService; }
};

}
}

#endif //___WinOSystem___