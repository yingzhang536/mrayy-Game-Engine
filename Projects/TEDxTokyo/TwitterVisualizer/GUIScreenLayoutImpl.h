

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

#include "GUIScreenLayout.h"
#include "AppData.h"


namespace mray
{
namespace GUI
{

class GUIScreenLayoutImpl:public GUIScreenLayout
{
protected:
public:
	GUIScreenLayoutImpl(){}
	virtual~GUIScreenLayoutImpl(){}

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

