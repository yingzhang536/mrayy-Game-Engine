#ifndef __GUICollisionLayout__
#define __GUICollisionLayout__
#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticImage.h"
namespace mray{

using namespace GUI;
class GUICollisionLayout:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIStaticImage* Left;
	GUIStaticImage* LeftFront;
	GUIStaticImage* LeftCenter;
	GUIStaticImage* RightCenter;
	GUIStaticImage* RightFront;
	GUIStaticImage* Right;

public:

	GUICollisionLayout():Root(0),Left(0),LeftFront(0),LeftCenter(0),RightCenter(0),RightFront(0),Right(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["Left"]=(IGUIElement**)&Left;
		m_elementsMap["LeftFront"]=(IGUIElement**)&LeftFront;
		m_elementsMap["LeftCenter"]=(IGUIElement**)&LeftCenter;
		m_elementsMap["RightCenter"]=(IGUIElement**)&RightCenter;
		m_elementsMap["RightFront"]=(IGUIElement**)&RightFront;
		m_elementsMap["Right"]=(IGUIElement**)&Right;

	}

};
}
#endif
