#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIProfilePicture.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUITweetDetailsPanelLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIProfilePicture* TwitterImage;
	GUIStaticText* TwitterID;
	GUIStaticText* Details;
	GUIStaticText* SessionTime;

public:

	GUITweetDetailsPanelLayout():Root(0),TwitterImage(0),TwitterID(0),Details(0),SessionTime(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["TwitterImage"]=(IGUIElement**)&TwitterImage;
		m_elementsMap["TwitterID"]=(IGUIElement**)&TwitterID;
		m_elementsMap["Details"]=(IGUIElement**)&Details;
		m_elementsMap["SessionTime"]=(IGUIElement**)&SessionTime;

	}

};
}