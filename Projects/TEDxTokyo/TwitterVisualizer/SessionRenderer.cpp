
#include "stdafx.h"
#include "SessionRenderer.h"

#include "SessionContainer.h"
#include "CSpeaker.h"
#include "SessionDetails.h"

#include "SpeakerNode.h"
#include "TweetNode.h"
#include "IThreadManager.h"
#include "NodeRenderer.h"
#include "TwitterTweet.h"

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
	m_hoverItem = 0;
	m_nodeRenderer = new NodeRenderer();
	m_dataMutex = OS::IThreadManager::getInstance().createMutex();
}

SessionRenderer::~SessionRenderer()
{
	delete m_physics;
	delete m_dataMutex;
	delete m_nodeRenderer;
}


void SessionRenderer::SetSessions(ted::SessionContainer*sessions)
{
	m_sessions = sessions;

	if (!m_sessions)
		return;

	msa::physics::Particle2D* root = new msa::physics::Particle2D(math::vector2d(400, 400));
	root->makeFixed();
	const std::vector<ted::SessionDetails*>& slist=m_sessions->GetSessions();

	int speakerCount = 0;
	for (int i = 0; i < slist.size(); ++i)
	{
		speakerCount += slist[i]->GetSpeakers().size();
	}
	float rad = 200;
	for (int i = 0; i < slist.size();++i)
	{
		const std::vector<ted::CSpeaker*>& speakers = slist[i]->GetSpeakers();
		for (int j = 0; j < speakers.size(); ++j)
		{
			SpeakerNode* s = new SpeakerNode(speakers[j]);
			math::vector2d pos = root->getPosition();
			pos.x += rad*math::cosd(m_speakers.size() * 360.0f / speakerCount);
			pos.y += rad*math::sind(m_speakers.size() * 360.0f / speakerCount);
			msa::physics::Particle2D* n = new msa::physics::Particle2D(pos);
			m_physics->addParticle(n);
			msa::physics::Spring2D* spr = m_physics->makeSpring(root, n,  0.005, rad);

			s->SetSize(40);
			n->setRadius(s->GetSize() );
			
			s->SetPhysics(n);
			m_speakers[s->GetUserDisplyName()] = s;

			m_renderNodes.push_back(s);
		}
	}

	float segment = rad*math::TwoPI32 / m_speakers.size();

	SpeakerMap::iterator  it = m_speakers.begin();

	if(false)
	for (int i=0; it != m_speakers.end(); ++it,++i)
	{
		int i2 = (i + 1) % m_speakers.size();
		SpeakerMap::iterator  it2 = m_speakers.begin();
		std::advance(it2, i2);
		msa::physics::Spring2D* spr = m_physics->makeSpring(it->second->GetPhysics(), it2->second->GetPhysics(), 0.01, segment);

	}
}

void SessionRenderer::AddTweetsNodes(const std::vector<TweetNode*> &nodes)
{
	if (!nodes.size())
		return;
	m_dataMutex->lock();
	for (int i = 0; i < nodes.size(); ++i)
	{
		
		m_tweets[nodes[i]->GetTweetID()] = nodes[i];

		float sz = 25;
		msa::physics::Particle2D* n = new msa::physics::Particle2D();
		n->setRadius(sz );
		m_physics->addParticle(n);
		nodes[i]->SetPhysics(n);
		//m_speakers.find(nodes[i]->GetSpeakerID());
	}

	for (int i = 0; i < nodes.size(); ++i)
	{
		 ted::TwitterTweet* t= nodes[i]->GetTweet()->replyToTweet;
		 ITedNode* target = 0;
		 if (t)
		 {
			 TweetMap::iterator it= m_tweets.find(t->ID);
			 if (it != m_tweets.end())
			 {
				 it->second->AddTweet(nodes[i]);
				 target = it->second;
			 }
		 }
		 if (!target)
		{
			int s = math::Randomizer::rand(m_speakers.size());
			SpeakerMap::iterator  it = m_speakers.begin();
			std::advance(it, s);
			it->second->AddTweet(nodes[i]);
			target = it->second;
		}

		 msa::physics::Particle2D *ph = target->GetPhysics();
		 math::vector2d pos = ph->getPosition();
		 float a = math::Randomizer::rand01() * 360;
		 msa::physics::Particle2D* nph= nodes[i]->GetPhysics();
		 float r = math::Randomizer::rand01() * 50 + 50 + nph->getRadius() + ph->getRadius();
		 float r2 = r + 300;
		 pos.x += math::cosd(a) * r2;
		 pos.y += math::sind(a) * r2;
		 nph->moveTo(pos, true);
		 msa::physics::Spring2D* spr = m_physics->makeSpring(ph, nph, 0.0001, r );


	}
	//m_renderNodes.insert(m_renderNodes.end(), nodes.begin(), nodes.end());
	m_dataMutex->unlock();
}

void SessionRenderer::_OnSpeakerChanged(ted::CSpeaker*s)
{
}


ITedNode* SessionRenderer::GetNodeFromPosition(const math::vector2d& pos)
{
	math::vector2d p = ConvertToWorldSpace(pos);
	m_dataMutex->lock();
	ITedNode* ret = 0;
	SpeakerMap::iterator  it = m_speakers.begin();

	for (; it != m_speakers.end();++it)
	{
		ret= it->second->GetNodeFromPoint(p);
		if (ret)
			break;;
	}
	m_dataMutex->unlock();
	return ret;
}


void SessionRenderer::Update(float dt)
{
	m_dataMutex->lock();
	m_physics->update(1);
	SpeakerMap::iterator  it = m_speakers.begin();

	for (; it != m_speakers.end(); ++it)
	{
		it->second->Update(dt);
	}
	m_dataMutex->unlock();

	//m_translation.x += 20 * dt;
	//SetTransformation(m_translation, 0, 1);
}
void SessionRenderer::Draw()
{
	math::matrix4x4 oldT;
	Engine::getInstance().getDevice()->getTransformationState(video::TS_WORLD,oldT);
	Engine::getInstance().getDevice()->setTransformationState(video::TS_WORLD, m_transformation);

	m_dataMutex->lock();
	m_nodeRenderer->Clear();

	SpeakerMap::iterator  it = m_speakers.begin();
	for (; it != m_speakers.end(); ++it)
	{
		it->second->Draw(m_nodeRenderer);
	}
	m_dataMutex->unlock();
	m_nodeRenderer->RenderAll(this);
	Engine::getInstance().getDevice()->setTransformationState(video::TS_WORLD, oldT);
}

void SessionRenderer::SetHoverdItem(ITedNode* node)
{
	if (m_hoverItem)
		m_hoverItem->OnHoverOff();
	m_hoverItem = node;
	if (m_hoverItem)
		m_hoverItem->OnHoverOn();
}


void SessionRenderer::SetTransformation(const math::vector2d& pos, float angle, const math::vector2d& scale)
{
	m_translation = pos;
	m_scale = scale;
	m_transformation.setTranslation(math::vector3d(pos.x, pos.y, 0));
	m_transformation.rotateZ(angle);
	if (scale != 1)
	{
		math::matrix4x4 o;
		o.setScale(math::vector3d(scale.x, scale.y, 1));
		m_transformation = m_transformation*o;
	}
}

math::vector2d SessionRenderer::ConvertToWorldSpace(const math::vector2d& pos)
{
	math::vector3d p= m_transformation.inverseTransformVector(math::vector3d(pos.x, pos.y, 0));
	return math::vector2d(p.x, p.y);
}
math::vector2d SessionRenderer::ConvertToScreenSpace(const math::vector2d& pos)
{
	math::vector3d p = m_transformation*math::vector3d(pos.x, pos.y, 0);
	return math::vector2d(p.x, p.y);
}
}
}
