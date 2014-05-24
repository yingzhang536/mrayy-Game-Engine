

#ifndef GUISceneSpacePanel_h__
#define GUISceneSpacePanel_h__

#include "IGUIPanelElement.h"
#include "GUIElementFactoryDef.h"
#include "GUIScenePanelLayout.h"
#include "AppData.h"

namespace mray
{
	namespace scene
	{
		class SessionRenderer;
	}
namespace GUI
{


class GUISceneSpacePanel :public IGUIPanelElement,public GUIScenePanelLayout,public ted::ISpeakerChangeListener
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:

	video::SColor m_targetColor;

	IGUIElement* m_topElement;
	IGUIElement* m_bottomElement;
	IGUIElement* m_leftElement;
	IGUIElement* m_rightElement;

	scene::SessionRenderer* m_sessionRenderer;

public:
	GUISceneSpacePanel(IGUIManager* m);
	virtual~GUISceneSpacePanel();

	virtual void _OnSpeakerChange(ted::CSpeaker* sp);

	void SetTopElement(IGUIElement* e){ m_topElement = e; }
	void SetBottomElement(IGUIElement* e){ m_bottomElement = e; }
	void SetLeftElement(IGUIElement* e){ m_leftElement = e; }
	void SetRightElement(IGUIElement* e){ m_rightElement = e; }

	void SetSessionRenderer(scene::SessionRenderer* s){
		m_sessionRenderer = s;
	}

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);


};
DECLARE_ELEMENT_FACTORY(GUISceneSpacePanel);

}
}

#endif // GUISceneSpacePanel_h__
