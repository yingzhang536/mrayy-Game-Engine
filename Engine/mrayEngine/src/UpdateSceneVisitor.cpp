#include "stdafx.h"


#include "UpdateSceneVisitor.h"

#include "ISceneNode.h"
#include "IRenderable.h"

#include "LightNode.h"
#include "CameraNode.h"
#include "ISceneManager.h"

namespace mray
{
namespace scene
{


UpdateSceneVisitor::UpdateSceneVisitor(float dt,ThreadJobManager* jobsManager)
{
	m_dt=dt;
	m_jobsManager=jobsManager;
}
UpdateSceneVisitor::~UpdateSceneVisitor()
{
}

void UpdateSceneVisitor::Visit(ISceneManager*mngr)
{
/*	const ISceneManager::SceneNodeMap& lst=mngr->GetChildren();
	ISceneManager::SceneNodeMap::const_iterator it=lst.begin();
	for (;it!=lst.end();++it)
	{
		it->second->OnVisit(this);
	}*/

	if (!mngr->getRootNode())
		return;
	mngr->getRootNode()->OnVisit(this);


	for (int i = 0; i < m_cameras.size(); ++i)
	{
		if (m_cameras[i]->isVisible())
			m_cameras[i]->update(m_dt);
	}
	m_cameras.clear();
}

void UpdateSceneVisitor::Visit(ISceneNode*node)
{
//	if(!node->IsDrown())
//		return;
	Visit((IMovable*)node);
	{

		const AttachNodesList& lst=node->GetAttachedNodes();
		for (int i=0;i<lst.size();++i)
		{
			lst[i]->OnVisit(this);
		}
	}
}

void UpdateSceneVisitor::Visit(CameraNode*c)
{
	Visit((ISceneNode*)c);
	m_cameras.push_back(c);

// 	if(c->isVisible())
// 		c->update(m_dt);
}

void UpdateSceneVisitor::Visit(LightNode*l)
{
	Visit((ISceneNode*)l);
	if(l->isEnabled())
		l->update(m_dt);
}

void UpdateSceneVisitor::Visit(IRenderable*r)
{
	r->update(m_dt);
	r->AddUpdateJobs(m_jobsManager,m_dt);
}


void UpdateSceneVisitor::Visit(IMovable*node)
{
	node->update(m_dt);
	/*
	{

		const MovableNodeList& lst=node->getChildren();
		MovableNodeList::const_iterator it=lst.begin();
		for (;it!=lst.end();++it)
		{
			(*it)->OnVisit(this);
		}
	}*/
}


}
}

