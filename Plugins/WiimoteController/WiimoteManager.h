


/********************************************************************
	created:	2011/01/28
	created:	28:1:2011   22:22
	filename: 	i:\Programing\GameEngine\mrayEngine\Controllers\WiimoteController\WiimoteManager.h
	file path:	i:\Programing\GameEngine\mrayEngine\Controllers\WiimoteController
	file base:	WiimoteManager
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef WiimoteManager_h__
#define WiimoteManager_h__

#include "ISingleton.h"
#include <vector>
#include "CMutexVar.h"

class CWii;
class CWiimote;

namespace mray
{
	namespace OS
	{
		class IThread;
		class IThreadFunction;
		class IMutex;
	}
namespace controllers
{

class WiimoteManager:public ISingleton<WiimoteManager>
{
private:
protected:
	CWii* m_wiiMngr;
	OS::IMutex* m_eventMutex;
	bool m_eventHappened;
	OS::IThreadFunction* m_threadFunction;
	OS::IThread* m_thread;
public:
	WiimoteManager();
	virtual ~WiimoteManager();

	void SetMaxWiimotesCount(int c);
	int GetNumConnectedWiimotes();

	void RefreshWiimotes();

	CWiimote* GetByID(int ID, int Refresh=1);
	std::vector<CWiimote*>& GetWiimotes(int Refresh=1);

	//void SetBluetoothStack(BTStacks Type);
	void SetTimeout(int NormalTimeout, int ExpTimeout);

	int Find(int timeout);
	std::vector<CWiimote*>& ConnectWithAllMotes();
	CWiimote* Connect(int i);

	void DisconnectWithAllMotes();
	void Disconnect(int i);

	void PollEvents();

	void _PollEvents();

	CWii* GetInternalWiiManager();
};

}
}
#endif // WiimoteManager_h__
