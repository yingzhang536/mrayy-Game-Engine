
#include "stdafx.h"
#include "SessionRenderer.h"

#include "SessionContainer.h"
#include "CSpeaker.h"
#include "SessionDetails.h"

#include "SpeakerNode.h"
#include "TweetNode.h"
#include "IThreadManager.h"

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

	m_dataMutex = OS::IThreadManager::getInstance().createMutex();
}

SessionRenderer::~SessionRenderer()
{
	delete m_physics;
	delete m_dataMutex;
}


void SessionRenderer::SetSessions(ted::SessionContainer*sessions)
{
	m_sessions = sessions;

	if (!m_sessions)
		return;

	msa::physics::Particle2D* root = new msa::physics::Particle2D(math::vector2d(400, 400));
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

void SessionRenderer::AddTweetsNodes(const std::vector<TweetNode*> &nodes)
{
	for (int i = 0; i < nodes.size();++i)
	{
		int s = math::Randomizer::rand(m_speakers.size());
		msa::physics::Particle2D *ph = m_speakers[s]->GetPhysics();
		math::vector2d pos = ph->getPosition();
		float a = math::Randomizer::rand01() * 360;
		pos.x += math::cosd(a) * 300;
		pos.y += math::sind(a) * 300;
		msa::physics::Particle2D* n = new msa::physics::Particle2D(pos);
		m_physics->addParticle(n);
		msa::physics::Spring2D* spr = m_physics->makeSpring(ph, n, 0.1, math::Randomizer::rand01()*25+25);
		nodes[i]->SetPhysics(n);
		m_tweets.push_back(nodes[i]);

	}
	m_dataMutex->lock();
	m_renderNodes.insert(m_renderNodes.end(), nodes.begin(), nodes.end());
	m_dataMutex->unlock();
}

void SessionRenderer::_OnSpeakerChanged(ted::CSpeaker*s)
{
}




void SessionRenderer::Update(float dt)
{
	m_physics->update();
	m_dataMutex->lock();
	std::list<ITedNode*>::iterator it = m_renderNodes.begin();
	for (; it != m_renderNodes.end(); ++it)
	{
		(*it)->Update(dt);
	}
	m_dataMutex->unlock();
}
void SessionRenderer::Draw()
{
	m_dataMutex->lock();
	std::list<ITedNode*>::iterator it = m_renderNodes.begin();
	for (; it != m_renderNodes.end();++it)
	{
		(*it)->Draw();
	} 
	m_dataMutex->unlock();
}

}
}
