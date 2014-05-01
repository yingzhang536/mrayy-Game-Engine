



/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   17:39
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\GUISessionDetailsTopPanel.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	GUISessionDetailsTopPanel
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GUISessionDetailsTopPanel__
#define __GUISessionDetailsTopPanel__



#include "IGUIPanelElement.h"
#include "GUIElementFactoryDef.h"
#include "GUISessionDetailsPanelLayout.h"


namespace mray
{
namespace ted
{
	class SessionDetails;
}
namespace GUI
{


class GUISessionDetailsTopPanel :public IGUIPanelElement,public GUISessionDetailsPanelLayout
{
	DECLARE_RTTI;
public:
	static const core::string ElementType;
protected:
	enum EState
	{
		Expand,
		Shrink
	};

	EState m_state;

	float m_maxWidth;
	float m_minWidth;

	ted::SessionDetails* m_details;
public:
	GUISessionDetailsTopPanel(IGUIManager* m);
	virtual ~GUISessionDetailsTopPanel();

	void SetSessionDetails(ted::SessionDetails* d);

	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};
DECLARE_ELEMENT_FACTORY(GUISessionDetailsTopPanel);

}
}

#endif
