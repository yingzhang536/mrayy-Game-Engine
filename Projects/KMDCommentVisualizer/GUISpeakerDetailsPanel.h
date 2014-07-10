



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
#include "GUIDockedElement.h"
#include "AppData.h"


namespace mray
{
namespace kmd
{
	class SessionDetails;
}
namespace GUI
{

class GUISessionSidePanel;

class GUISpeakerDetailsPanel :public IGUIPanelElement, public GUISpeakerDetailsPanelLayout, public GUIDockedElement, public kmd::ISubProjectChangeListener
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
	bool m_active;

	kmd::SessionDetails* m_details;

public:
	GUISpeakerDetailsPanel(IGUIManager* m);
	virtual ~GUISpeakerDetailsPanel();

	void SetSidePanel(GUISessionSidePanel* p);


	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
	virtual void _OnSubProjectChange(kmd::CSubProject* sp);
};
DECLARE_ELEMENT_FACTORY(GUISpeakerDetailsPanel);

}
}

#endif
