#include "IGUISchemeBase.h"
#include "GUIEditBox.h"
#include "GUIListBox.h"
#include "GUIPanel.h"
#include "GUISessionDetailsTopPanel.h"
#include "GUISessionSidePanel.h"
#include "GUIStackPanel.h"
#include "GUIStaticImage.h"
#include "GUIStaticText.h"
#include "GUIUserProfile.h"
namespace mray{

using namespace GUI;
class GUIScreenLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIStaticImage* TEDxTokyo_Logo;
	GUIStackPanel* RootStack;
	GUIEditBox* UserTxt;
	GUIListBox* UsersList;
	GUIUserProfile* UserProfile;
	GUIStackPanel* TweetsList;

public:

	GUIScreenLayout():Root(0),TEDxTokyo_Logo(0),RootStack(0),UserTxt(0),UsersList(0),UserProfile(0),TweetsList(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["TEDxTokyo_Logo"]=(IGUIElement**)&TEDxTokyo_Logo;
		m_elementsMap["RootStack"]=(IGUIElement**)&RootStack;
		m_elementsMap["UserTxt"]=(IGUIElement**)&UserTxt;
		m_elementsMap["UsersList"]=(IGUIElement**)&UsersList;
		m_elementsMap["UserProfile"]=(IGUIElement**)&UserProfile;
		m_elementsMap["TweetsList"]=(IGUIElement**)&TweetsList;

	}

};
}