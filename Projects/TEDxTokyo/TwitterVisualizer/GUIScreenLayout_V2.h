#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUISessionDetailsTopPanel.h"
#include "GUISessionSidePanel.h"
#include "GUIStaticImage.h"
#include "GUITweetDetailsPanel.h"
namespace mray{

using namespace GUI;
class GUIScreenLayout_V2:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUISessionSidePanel* SessionsBar;
	GUISessionDetailsTopPanel* SessionDetails;
	GUITweetDetailsPanel* TweetDetails;
	GUIStaticImage* TEDxTokyo_Logo;

public:

	GUIScreenLayout_V2():Root(0),SessionsBar(0),SessionDetails(0),TweetDetails(0),TEDxTokyo_Logo(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["SessionsBar"]=(IGUIElement**)&SessionsBar;
		m_elementsMap["SessionDetails"]=(IGUIElement**)&SessionDetails;
		m_elementsMap["TweetDetails"]=(IGUIElement**)&TweetDetails;
		m_elementsMap["TEDxTokyo_Logo"]=(IGUIElement**)&TEDxTokyo_Logo;

	}

};
}