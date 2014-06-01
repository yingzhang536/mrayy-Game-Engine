

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

#include "GUIScreenLayout_V2.h"
#include "AppData.h"


namespace mray
{
namespace GUI
{

class GUIScreenLayoutImplV2:public GUIScreenLayout_V2
{
protected:
public:
	GUIScreenLayoutImplV2(){}
	virtual~GUIScreenLayoutImplV2(){}

	virtual void DoneLoadingElements()
	{
		SessionDetails->SetSidePanel(SessionsBar);
		TweetDetails->SetSidePanel(SessionsBar);

		ScenePanel->SetLeftElement(SessionsBar);
		ScenePanel->SetTopElement(SessionDetails);
		//ScenePanel->SetBottomElement(TweetDetails);
	}
};

}
}

#endif

