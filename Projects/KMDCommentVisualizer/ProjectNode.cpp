

#include "stdafx.h"
#include "ProjectNode.h"
#include "SessionDetails.h"
#include "ShaderResourceManager.h"

#include "CommentNode.h"
#include "NodeRenderer.h"
#include "KMDUser.h"

namespace mray
{
namespace scene
{

ProjectNode::ProjectNode(kmd::SessionDetails* s)
{
	m_project = s;
	m_glowFactor = 0;
}
ProjectNode::~ProjectNode()
{

}

core::string ProjectNode::GetProjectName()
{
	if (m_project )
		return m_project->GetSessionName();
	return "";
}

void ProjectNode::AddComment(CommentNode* t)
{
	m_subComments.push_back(t);
	t->SetParent(this);
}
void ProjectNode::Draw(NodeRenderer *r, const math::rectf& rc)
{
	if (rc.IsRectCollide(GetBoundingBox(false)))
		r->AddSpeaker(this, m_glowFactor);
	for (int i = 0; i < m_subComments.size(); ++i)
	{
		r->AddSpeakerCommentNode(this, m_subComments[i]);
	}

	for (int i = 0; i < m_subComments.size(); ++i)
	{
		m_subComments[i]->Draw(r,rc);
	}
}
void ProjectNode::Update(float dt)
{
#define DECREASE(x,l,v) {if(x>(l)){ x-=(v);} if(x<(l)){x=(l);}}
#define INCREASE(x,l,v) {if(x<(l)){ x+=(v);} if(x>(l)){x=(l);}}

	if (m_selected)
	{
		if (m_hovered)
		{
			INCREASE(m_glowFactor, 1, dt);
		}
		else 
		{
			if (m_glowFactor > 0.8)
			{
				DECREASE(m_glowFactor, 0.8, dt);
			}
			else
			{
				INCREASE(m_glowFactor, 0.8, dt);
			}
		}
	}
	else
	{
		DECREASE(m_glowFactor, 0, dt);
	}
	for (int i = 0; i < m_subComments.size(); ++i)
	{
		m_subComments[i]->Update(dt);
	}
}
IKMDNode* ProjectNode::GetNodeFromPoint(const math::vector2d& pos)
{
	for (int i = 0; i < m_subComments.size(); ++i)
	{
		IKMDNode* n= m_subComments[i]->GetNodeFromPoint(pos);
		if (n)
			return n;
	}
	return IKMDNode::GetNodeFromPoint(pos);
}

math::rectf ProjectNode::GetBoundingBox(bool includeChildren)
{
	math::rectf rc = IKMDNode::GetBoundingBox(false);
	if (!includeChildren)
		return rc;
	for (int i = 0; i < m_subComments.size(); ++i)
	{
		math::rectf r = m_subComments[i]->GetBoundingBox(true);
		rc.addPoint(r.ULPoint);
		rc.addPoint(r.BRPoint);
	}
	return rc;
}
}
}

