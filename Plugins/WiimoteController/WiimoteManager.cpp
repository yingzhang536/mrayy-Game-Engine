
#include "WiimoteManager.h"
#include "wiiusecpp.h"
#include "EventQueue.h"
#include "WiiEvent.h"
#include "EventMemoryManager.h"

#include "IThreadFunction.h"
#include "IThreadManager.h"
#include "IThread.h"
#include "IMutex.h"


namespace mray
{
namespace controllers
{


	class WiimoteManagerPollThread:public OS::IThreadFunction
	{
		WiimoteManager* m_manager;
	public:
		WiimoteManagerPollThread(WiimoteManager* m)
		{
			m_manager=m;
		}
		virtual void setup()
		{
		}
		virtual void execute(OS::IThread*caller,void*arg)
		{
			while (caller->isActive())
			{
				m_manager->_PollEvents();
				OS::IThreadManager::getInstance().sleep(10);//sleep for 10 ms
			}
		}
	};

WiimoteManager::WiimoteManager():m_eventHappened(false)
{
	m_wiiMngr=new CWii();

	m_eventMutex=OS::IThreadManager::getInstance().createMutex();
	m_threadFunction=new WiimoteManagerPollThread(this);

	m_thread=OS::IThreadManager::getInstance().createThread(m_threadFunction);
	m_thread->start(0);
}
WiimoteManager::~WiimoteManager()
{
	OS::IThreadManager::getInstance().killThread(m_thread);
	delete m_thread;
	delete m_threadFunction;
	delete m_eventMutex;
	delete m_wiiMngr;
}

void WiimoteManager::SetMaxWiimotesCount(int c)
{
	m_wiiMngr->SetMaxWiimotesCount(c);
}
int WiimoteManager::GetNumConnectedWiimotes()
{
	return m_wiiMngr->GetNumConnectedWiimotes();
}

void WiimoteManager::RefreshWiimotes()
{
	 m_wiiMngr->RefreshWiimotes();
}

CWiimote* WiimoteManager::GetByID(int ID, int Refresh)
{
	return m_wiiMngr->GetByID(ID,Refresh);
}
std::vector<CWiimote*>& WiimoteManager::GetWiimotes(int Refresh)
{
	return m_wiiMngr->GetWiimotes(Refresh);
}

//void SetBluetoothStack(BTStacks Type);
void WiimoteManager::SetTimeout(int NormalTimeout, int ExpTimeout)
{
	 m_wiiMngr->SetTimeout(NormalTimeout,ExpTimeout);
}

int WiimoteManager::Find(int timeout)
{
	return m_wiiMngr->Find(timeout);
}
std::vector<CWiimote*>& WiimoteManager::ConnectWithAllMotes()
{
	return m_wiiMngr->ConnectWithAllMotes();
}
CWiimote* WiimoteManager::Connect(int i)
{
	return m_wiiMngr->Connect(i);
}

void WiimoteManager::DisconnectWithAllMotes()
{
	m_wiiMngr->DisconnectWithAllMotes();
}
void WiimoteManager::Disconnect(int i)
{
	m_wiiMngr->Disconnect(i);
}

void WiimoteManager::PollEvents()
{
	if(m_eventMutex->tryLock()){
		if(!m_eventHappened)
		{
			m_eventMutex->unlock();
			return;
		}
		m_eventHappened=false;
		m_eventMutex->unlock();
	}else
		return;
	std::vector<CWiimote*>& vec=m_wiiMngr->GetWiimotes(false);
	for (int i=0;i<vec.size();++i)
	{
		CWiimote*w=vec[i];
		if(!w)continue;
		CWiimote::EventTypes e= w->GetEvent();
		EWiiEventType type=EWiiEvent_Unkown;
		switch (e)
		{
		case CWiimote::EVENT_NONE:break;
		case CWiimote::EVENT_EVENT:
			type=EWiiEvent_Event;
			break;
		case CWiimote::EVENT_STATUS:
			type=EWiiEvent_Status;
			break;
		case CWiimote::EVENT_CONNECT:
			type=EWiiEvent_Connect;
			break;
		case CWiimote::EVENT_DISCONNECT:
			type=EWiiEvent_Disconnect;
			break;
		case CWiimote::EVENT_UNEXPECTED_DISCONNECT:
			type=EWiiEvent_UnexpectedDisconnect;
			break;
		case CWiimote::EVENT_READ_DATA:
			type=EWiiEvent_ReadData;
			break;
		case CWiimote::EVENT_NUNCHUK_INSERTED:
			type=EWiiEvent_NunchukInserted;
			break;
		case CWiimote::EVENT_NUNCHUK_REMOVED:
			type=EWiiEvent_NunchukRemoved;
			break;
		case CWiimote::EVENT_CLASSIC_CTRL_INSERTED:
			type=EWiiEvent_ClassicControlInserted;
			break;
		case CWiimote::EVENT_CLASSIC_CTRL_REMOVED:
			type=EWiiEvent_ClassicControlRemoved;
			break;
		case CWiimote::EVENT_GUITAR_HERO_3_CTRL_INSERTED:
			type=EWiiEvent_GuitarHero3Inserted;
			break;
		case CWiimote::EVENT_GUITAR_HERO_3_CTRL_REMOVED:
			type=EWiiEvent_GuitarHero3Removed;
			break;
		}
		if(type==EWiiEvent_Unkown)
			continue;

		WiiEvent* evt=(WiiEvent*)EventMemoryManager::getInstance().createEvent(WiiEvent::EventID).pointer();
		if(!evt)
			evt=new WiiEvent();
		evt->event=type;
		evt->WiimoteNumber=i;
		EventQueue::getInstance().pushEvent(evt);
	}
}

void WiimoteManager::_PollEvents()
{

	if(m_wiiMngr->Poll()==0)
		return;
	m_eventMutex->lock();
	m_eventHappened=true;
	m_eventMutex->unlock();
}

CWii* WiimoteManager::GetInternalWiiManager()
{
	return m_wiiMngr;
}



}
}