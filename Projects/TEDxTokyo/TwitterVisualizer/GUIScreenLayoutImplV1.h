

/********************************************************************
	created:	2014/04/13
	created:	13:4:2014   17:45
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\GUIScreenLayoutImpl.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	GUIScreenLayoutImpl
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GUIScreenLayoutImpl__
#define __GUIScreenLayoutImpl__

#include "GUIScreenLayout_V1.h"
#include "AppData.h"


namespace mray
{
namespace GUI
{

	class GUIScreenLayoutImplV1 :public GUIScreenLayout_V1
{
protected:
public:
	GUIScreenLayoutImplV1(){}
	virtual~GUIScreenLayoutImplV1(){}

	virtual void DoneLoadingElements()
	{
		SessionDetails->SetSidePanel(SessionsBar);
		TweetDetails->SetSidePanel(SessionsBar);
		SessionsBar->SetSessionContainer(gAppData.sessions);
	}
};

}
}

#endif

