

/********************************************************************
	created:	2011/01/28
	created:	28:1:2011   23:03
	filename: 	i:\Programing\GameEngine\mrayEngine\Controllers\WiimoteController\WiiEvent.h
	file path:	i:\Programing\GameEngine\mrayEngine\Controllers\WiimoteController
	file base:	WiiEvent
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef WiiEvent_h__
#define WiiEvent_h__

#include "Event.h"

namespace mray
{
	enum EWiiEventType
	{
		EWiiEvent_Unkown,
		EWiiEvent_Event,
		EWiiEvent_Status,
		EWiiEvent_Connect,
		EWiiEvent_Disconnect,
		EWiiEvent_UnexpectedDisconnect,
		EWiiEvent_ReadData,
		EWiiEvent_NunchukInserted,
		EWiiEvent_NunchukRemoved,
		EWiiEvent_ClassicControlInserted,
		EWiiEvent_ClassicControlRemoved,
		EWiiEvent_GuitarHero3Inserted,
		EWiiEvent_GuitarHero3Removed
	};

class WiiEvent:public Event
{
private:
protected:
public:

	static const int EventID;

	WiiEvent();
	virtual ~WiiEvent();

	EWiiEventType event;

	int WiimoteNumber;
	
};

}
#endif // WiiEvent_h__
