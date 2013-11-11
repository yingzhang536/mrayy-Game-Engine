#include "IGUISchemeBase.h"
#include "GUIBlinkingText.h"
#include "GUILoadingElement.h"
#include "GUIPanel.h"
#include "GUIStaticText.h"
namespace mray{

using namespace GUI;
class GUISeethroughScreen:public GUI::IGUISchemeBase
{

public:
	GUIStaticText* UserNameLbl;
	GUIStaticText* DateTime;
	GUIStaticText* LatLngLbl;
	GUIStaticText* AvailableLbl;
	GUIStaticText* RemoteTimeLbl;

public:

	GUISeethroughScreen():UserNameLbl(0),DateTime(0),LatLngLbl(0),AvailableLbl(0),RemoteTimeLbl(0)
	{		
		m_elementsMap["UserNameLbl"]=(IGUIElement**)&UserNameLbl;
		m_elementsMap["DateTime"]=(IGUIElement**)&DateTime;
		m_elementsMap["LatLngLbl"]=(IGUIElement**)&LatLngLbl;
		m_elementsMap["AvailableLbl"]=(IGUIElement**)&AvailableLbl;
		m_elementsMap["RemoteTimeLbl"]=(IGUIElement**)&RemoteTimeLbl;

	}

};
}