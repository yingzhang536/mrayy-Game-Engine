#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUISceneSpacePanel.h"
#include "GUISessionSidePanel.h"
#include "GUISpeakerDetailsPanel.h"
#include "GUITweetDetailsPanel.h"
namespace mray{

using namespace GUI;
class GUIScreenLayout_V2:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUISceneSpacePanel* ScenePanel;
	GUISessionSidePanel* SessionsBar;
	GUISpeakerDetailsPanel* SessionDetails;
	GUITweetDetailsPanel* TweetDetails;

public:

	GUIScreenLayout_V2():Root(0),ScenePanel(0),SessionsBar(0),SessionDetails(0),TweetDetails(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["ScenePanel"]=(IGUIElement**)&ScenePanel;
		m_elementsMap["SessionsBar"]=(IGUIElement**)&SessionsBar;
		m_elementsMap["SessionDetails"]=(IGUIElement**)&SessionDetails;
		m_elementsMap["TweetDetails"]=(IGUIElement**)&TweetDetails;

	}

};
}