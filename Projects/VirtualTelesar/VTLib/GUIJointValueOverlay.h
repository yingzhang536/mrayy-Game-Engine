#ifndef __JointValue__
#define __JointValue__
#include "IGUISchemeBase.h"
#include "Panel.h"
#include "SliderBar.h"
#include "StackPanel.h"
#include "StaticText.h"
namespace mray{

using namespace GUI;
class JointValue:public GUI::IGUISchemeBase
{

public:
	StaticText* MinTxt;
	SliderBar* ValueBar;
	StaticText* MaxTxt;

public:

	JointValue():MinTxt(0),ValueBar(0),MaxTxt(0)
	{		
		m_elementsMap["MinTxt"]=(IGUIElement**)&MinTxt;
		m_elementsMap["ValueBar"]=(IGUIElement**)&ValueBar;
		m_elementsMap["MaxTxt"]=(IGUIElement**)&MaxTxt;

	}

};
}
#endif
