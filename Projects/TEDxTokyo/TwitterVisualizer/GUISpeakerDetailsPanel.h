



/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   17:39
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\GUISpeakerDetailsPanel.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	GUISpeakerDetailsPanel
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GUISpeakerDetailsPanel__
#define __GUISpeakerDetailsPanel__



#include "IGUIPanelElement.h"
#include "GUIElementFactoryDef.h"
#include "GUISpeakerDetailsPanelLayout.h"


namespace mray
{
namespace ted
{
	class SessionDetails;
}
namespace GUI
{

class GUISessionSidePanel;

class GUISpeakerDetailsPanel :public IGUIPanelElement, public GUISpeakerDetailsPanelLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
	bool m_active;

	GUI::GUISessionSidePanel* m_sidePanel;

	ted::SessionDetails* m_details;

	void _OnSpeakerChange(IObject* sender, PVOID param);
public:
	GUISpeakerDetailsPanel(IGUIManager* m);
	virtual ~GUISpeakerDetailsPanel();

	void SetSidePanel(GUISessionSidePanel* p);


	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};
DECLARE_ELEMENT_FACTORY(GUISpeakerDetailsPanel);

}
}

#endif
