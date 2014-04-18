#include "stdafx.h"

#include "RenderSceneVisitor.h"

#include "ISceneNode.h"
#include "IRenderable.h"
#include "ISceneManager.h"

namespace mray
{
namespace scene
{


RenderSceneVisitor::RenderSceneVisitor(IRenderPass *pass)
{
	m_pass=pass;
	m_smngr=0;
}
RenderSceneVisitor::~RenderSceneVisitor()
{
}

void RenderSceneVisitor::Visit(ISceneManager*mngr)
{
	m_smngr=mngr;

	const ISceneManager::SceneNodeMap& lst=m_smngr->GetChildren();
	ISceneManager::SceneNodeMap::const_iterator it=lst.begin();
	for (;it!=lst.end();++it)
	{
		it->second->OnVisit(this);
	}
}

void RenderSceneVisitor::Visit(ISceneNode*node)
{
	if(!node->IsDrown())
		return;
	node->render(m_pass);
	const AttachNodesList& attached=node->GetAttachedNodes();
	for(int i=0;i<attached.size();++i)
		attached[i]->OnVisit(this);

	Visit((IMovable*)node);

}

void RenderSceneVisitor::Visit(CameraNode*c)
{
	//Visit((ISceneNode*)c);
}

void RenderSceneVisitor::Visit(LightNode*l)
{
	Visit((ISceneNode*)l);
}

void RenderSceneVisitor::Visit(IRenderable*r)
{
	if(!r->isCustomRender() || !m_pass->canRenderObject(r))
		return;
	r->render(m_pass);
}


void RenderSceneVisitor::Visit(IMovable*node)
{
	const MovableNodeList& nodes=node->getChildren();
	MovableNodeList::const_iterator it=nodes.begin();
	for(;it!=nodes.end();++it)
		(*it)->OnVisit(this);
}


}
}
