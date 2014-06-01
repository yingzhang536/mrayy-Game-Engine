#ifndef GUISessionSidePanel_h__
#define GUISessionSidePanel_h__

#include "IGUIPanelElement.h"
#include "GUIElementFactoryDef.h"
#include "GUISessionSidePanelLayout.h"


namespace mray
{
namespace ted
{
	class SessionContainer;
	class CSpeaker;
}
namespace GUI
{

class GUISessionSidePanel :public IGUIPanelElement, public GUISessionSidePanelLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
	static const core::string ProfileRedneringShader;
protected:
	enum EState
	{
		Expand,
		Shrink
	};

	struct SpeakerImageIfo
	{
		SpeakerImageIfo() :t(0), speaker(0)
		{}
		ted::CSpeaker* speaker;
		video::ITexturePtr texture;
		float t;
	};

	std::vector<SpeakerImageIfo> m_speakers;

	EState m_state;

	float m_maxWidth;
	float m_minWidth;

	float m_szT;

	float m_speakerTransition;
	int m_currentSpeaker;

	ted::SessionContainer* m_sessions;

public:
	GUISessionSidePanel(IGUIManager* m);
	virtual~GUISessionSidePanel();

	ObjectEvent OnSpeakerChange;

	ted::CSpeaker* GetActiveSpeaker(){
		return m_speakers[m_currentSpeaker].speaker;
	}
	void SetSessionContainer(ted::SessionContainer* s);

	int GetCurrentSpeakerID(){ return m_currentSpeaker; }
	void SetCurrentSpeaker(int id);
	int GetSpeakersCount(){ return m_speakers.size(); }

	bool IsActive(){ return m_state == Expand; }

	virtual bool OnEvent(Event* e);

	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};
DECLARE_ELEMENT_FACTORY(GUISessionSidePanel);

}
}

#endif // GUISessionSidePanel_h__
