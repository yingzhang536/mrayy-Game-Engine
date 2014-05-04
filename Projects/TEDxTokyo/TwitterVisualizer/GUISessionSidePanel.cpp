

#include "stdafx.h"
#include "GUISessionSidePanel.h"
#include "GUIElementRegion.h"
#include "SessionContainer.h"
#include "SessionDetails.h"
#include "CSpeaker.h"

#include "TextureResourceManager.h"


namespace mray
{
namespace GUI
{


IMPLEMENT_RTTI(GUISessionSidePanel, IGUIElement);

const core::string GUISessionSidePanel::ElementType = "GUISessionSidePanel";

float GUISessionSidePanel_ShrinkSpeed = 150;

GUISessionSidePanel::GUISessionSidePanel(IGUIManager* m):
IGUIElement(ElementType,m)
{
	m_minWidth = 40;
	m_maxWidth = 100;
	m_state = Shrink;
	m_sessions = 0;
	m_szT = 0;
	m_currentSpeaker = 0;
}
GUISessionSidePanel::~GUISessionSidePanel()
{
}
void GUISessionSidePanel::SetSessionContainer(ted::SessionContainer* s)
{
	m_sessions = s;
	const std::vector<ted::SessionDetails*>& sessions= m_sessions->GetSessions();
	for (int i = 0; i < sessions.size(); ++i)
	{
		const std::vector<ted::CSpeaker*>& speakers= sessions[i]->GetSpeakers();
		for (int j = 0; j < speakers.size(); ++j)
		{
			SpeakerImageIfo ifo;
			ifo.speaker = speakers[j];
			ifo.texture = gTextureResourceManager.loadTexture2D(ifo.speaker->GetPicture());
			m_speakers.push_back(ifo);
		}
	}
}
bool GUISessionSidePanel::OnEvent(Event* e)
{
	if (e->getType() == ET_Mouse)
	{
		MouseEvent* evt = dynamic_cast<MouseEvent*>(e);
		const GUIElementRegion* r = GetDefaultRegion();
		if (r->GetClippedRect().IsPointInside(evt->pos))
		{
			m_state = Expand;
		}
		else 
			m_state=Shrink;
	}
	return false;
}

void GUISessionSidePanel::Update(float dt)
{
	IGUIElement::Update(dt);

#define DECREASE(x,l,v) if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}
#define INCREASE(x,l,v) if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}

	math::vector2d sz = this->GetSize();
	float a= GetAlpha();
	if (m_state == Shrink)
	{
		DECREASE(sz.x, m_minWidth, GUISessionSidePanel_ShrinkSpeed*dt);
		//DECREASE(a, 0.2, dt);
	}

	if (m_state == Expand)
	{
		//INCREASE(a, 1, dt);
		INCREASE(sz.x, m_maxWidth, GUISessionSidePanel_ShrinkSpeed*dt);
	}
	m_szT = ((sz.x - m_minWidth) / (m_maxWidth - m_minWidth));
	a = 0.2 + 0.8*m_szT;
	SetSize(sz);
	SetAlpha(a);
}
void GUISessionSidePanel::Draw(const math::rectf*vp)
{
	video::IVideoDevice* dev= Engine::getInstance().getDevice();

	const GUIElementRegion* r = GetDefaultRegion();


	dev->draw2DRectangle(r->GetRect(), GetColor());

	video::TextureUnit tex;

	if (m_szT > 0)
	{
		float sz = r->GetClippedRect().getWidth()*0.5f;

		float x = r->GetClippedRect().ULPoint.x + sz;
		float step = r->GetClippedRect().getHeight() / ((float)m_speakers.size() + 1);
		float y = step;
		math::rectf rc;
		for (int i = 0; i < m_speakers.size(); ++i)
		{
			float dst = (float)abs(m_currentSpeaker - i) / (float)m_speakers.size();
			tex.SetTexture(m_speakers[i].texture);
			dev->useTexture(0, &tex);

			float t = (powf(1 - dst,2.0f)*0.8 + 0.2);;
			rc.ULPoint.y = y - sz*0.5f*t;
			rc.BRPoint.y = y + sz*0.5f*t;

			rc.ULPoint.x = x - sz*0.5*t;
			rc.BRPoint.x = x + sz*0.5*t;

			y += step;
			float a = m_szT* t;

			dev->draw2DImage(rc, video::SColor(1, 1, 1, a));
		}
	}

}

IMPLEMENT_ELEMENT_FACTORY(GUISessionSidePanel);

}
}
