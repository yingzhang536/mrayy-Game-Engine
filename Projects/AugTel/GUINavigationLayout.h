#ifndef __GUINavigationLayout__
#define __GUINavigationLayout__
#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticImage.h"
namespace mray{

using namespace GUI;
class GUINavigationLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIStaticImage* LeftArrow;
	GUIStaticImage* RightArrow;
	GUIStaticImage* FrontArrow;
	GUIStaticImage* BackArrow;
	GUIStaticImage* HorArrow;
	GUIStaticImage* VerArrow;

public:

	GUINavigationLayout():Root(0),LeftArrow(0),RightArrow(0),FrontArrow(0),BackArrow(0),HorArrow(0),VerArrow(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["LeftArrow"]=(IGUIElement**)&LeftArrow;
		m_elementsMap["RightArrow"]=(IGUIElement**)&RightArrow;
		m_elementsMap["FrontArrow"]=(IGUIElement**)&FrontArrow;
		m_elementsMap["BackArrow"]=(IGUIElement**)&BackArrow;
		m_elementsMap["HorArrow"]=(IGUIElement**)&HorArrow;
		m_elementsMap["VerArrow"]=(IGUIElement**)&VerArrow;

	}

};
}
#endif
