#include "IGUISchemeBase.h"
#include "GUISliderBar.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class JointValue:public GUI::IGUISchemeBase
{

public:
	GUIStaticText* MinTxt;
	GUISliderBar* ValueBar;
	GUIStaticText* MaxTxt;

public:

	JointValue():MinTxt(0),ValueBar(0),MaxTxt(0)
	{		
		m_elementsMap["MinTxt"]=(IGUIElement**)&MinTxt;
		m_elementsMap["ValueBar"]=(IGUIElement**)&ValueBar;
		m_elementsMap["MaxTxt"]=(IGUIElement**)&MaxTxt;

	}

};
}