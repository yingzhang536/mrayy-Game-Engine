#include "IGUISchemeBase.h"
#include "GUIPanel.h"
#include "GUIStaticImage.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUIRobotPanel:public GUI::IGUISchemeBase
{

public:
	GUIPanel* Root;
	GUIStaticText* NameLbl;
	GUIStaticImage* RobotVision;
	GUIStaticText* IPLbl;
	GUIStaticText* LatLngLbl;
	GUIStaticText* AvailableLbl;
	GUIStaticText* RemoteTimeLbl;

public:

	GUIRobotPanel():Root(0),NameLbl(0),RobotVision(0),IPLbl(0),LatLngLbl(0),AvailableLbl(0),RemoteTimeLbl(0)
	{		
		m_elementsMap["Root"]=(IGUIElement**)&Root;
		m_elementsMap["NameLbl"]=(IGUIElement**)&NameLbl;
		m_elementsMap["RobotVision"]=(IGUIElement**)&RobotVision;
		m_elementsMap["IPLbl"]=(IGUIElement**)&IPLbl;
		m_elementsMap["LatLngLbl"]=(IGUIElement**)&LatLngLbl;
		m_elementsMap["AvailableLbl"]=(IGUIElement**)&AvailableLbl;
		m_elementsMap["RemoteTimeLbl"]=(IGUIElement**)&RemoteTimeLbl;

	}

};
}