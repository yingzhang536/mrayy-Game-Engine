



/********************************************************************
created:	2014/04/30
created:	30:4:2014   17:39
filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\GUITweetDetailsPanel.h
file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
file base:	GUITweetDetailsPanel
file ext:	h
author:		MHD Yamen Saraiji

purpose:
*********************************************************************/

#ifndef __GUITweetDetailsPanel__
#define __GUITweetDetailsPanel__



#include "IGUIPanelElement.h"
#include "GUIElementFactoryDef.h"
#include "GUITweetDetailsPanelLayout.h"


namespace mray
{
namespace ted
{
class SessionDetails;
}
namespace GUI
{

class GUISessionSidePanel;

class GUITweetDetailsPanel :public IGUIPanelElement, public GUITweetDetailsPanelLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
	bool m_active;

	GUI::GUISessionSidePanel* m_sidePanel;

public:
	GUITweetDetailsPanel(IGUIManager* m);
	virtual ~GUITweetDetailsPanel();


	void SetSidePanel(GUISessionSidePanel* p){ m_sidePanel = p; }

	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};
DECLARE_ELEMENT_FACTORY(GUITweetDetailsPanel);

}
}

#endif
