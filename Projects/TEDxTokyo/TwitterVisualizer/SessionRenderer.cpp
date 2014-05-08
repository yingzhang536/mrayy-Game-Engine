
#include "stdafx.h"
#include "SessionRenderer.h"

#include "SessionContainer.h"
#include "CSpeaker.h"
#include "SessionDetails.h"

#include "SpeakerNode.h"
#include "TweetNode.h"

namespace mray
{
namespace scene
{

SessionRenderer::SessionRenderer()
{
	m_sessions = 0;
	{
		m_physics = new msa::physics::World2D();
		m_physics->setGravity(0);
	}
}

SessionRenderer::~SessionRenderer()
{
	delete m_physics;
}


void SessionRenderer::SetSessions(ted::SessionContainer*sessions)
{
	m_sessions = sessions;

	if (!m_sessions)
		return;

	msa::physics::Particle2D* root = new msa::physics::Particle2D(math::vector2d(300, 200));
	root->makeFixed();
	const std::vector<ted::SessionDetails*>& slist=m_sessions->GetSessions();
	for (int i = 0; i < slist.size();++i)
	{
		const std::vector<ted::CSpeaker*>& speakers = slist[i]->GetSpeakers();
		for (int j = 0; j < speakers.size(); ++j)
		{
			SpeakerNode* s = new SpeakerNode(speakers[j]);

			msa::physics::Particle2D* n = new msa::physics::Particle2D(msa::Vec2f(math::Randomizer::rand01() * 600, math::Randomizer::rand01() * 600));
			m_physics->addParticle(n);
			msa::physics::Spring2D* spr = m_physics->makeSpring(root, n,  0.5, 200);
			
			s->SetPhysics(n);
			m_speakers.push_back(s);

			m_renderNodes.push_back(s);
		}
	}
}


void SessionRenderer::_OnSpeakerChanged(ted::CSpeaker*s)
{
}




void SessionRenderer::Update(float dt)
{
	m_physics->update();
	std::list<ITedNode*>::iterator it = m_renderNodes.begin();
	for (; it != m_renderNodes.end(); ++it)
	{
		(*it)->Update(dt);
	}
}
void SessionRenderer::Draw()
{
	std::list<ITedNode*>::iterator it= m_renderNodes.begin();
	for (; it != m_renderNodes.end();++it)
	{
		(*it)->Draw();
	} 
}

}
}
