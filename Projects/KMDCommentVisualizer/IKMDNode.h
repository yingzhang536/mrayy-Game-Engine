



/********************************************************************
	created:	2014/05/07
	created:	7:5:2014   18:08
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\IKMDNode.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	IKMDNode
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IKMDNode__
#define __IKMDNode__


#include "PhNode.h"

namespace mray
{
namespace scene
{
	class NodeRenderer;
class IKMDNode
{
protected:
	//msa::physics::Particle2D* m_phNode;
	physics::PhNode* m_phNode;
	float m_sz;
	bool m_hovered, m_selected;
	IKMDNode* m_parent;
	float m_alpha;
public:
	IKMDNode() :m_phNode(0), m_sz(30), m_hovered(false), m_parent(0), m_alpha(1), m_selected(false)
	{}
	virtual ~IKMDNode(){}

	IKMDNode* GetParent(){ return m_parent; }

	void SetSize(float s){ m_sz = s; }
	float GetSize(){ return m_sz; }
	virtual math::vector2d GetPosition()
	{
		return m_phNode ? m_phNode->GetPosition() : math::vector2d(0);
	}

	void SetParent(IKMDNode* p)
	{
		m_parent = p;
	}
	void SetPhysics(physics::PhNode* n){ m_phNode = n; }
	physics::PhNode* GetPhysics(){ return m_phNode; }
	virtual void Update(float dt){}
	virtual void Draw(NodeRenderer *r, const math::rectf& rc){}

	virtual bool IsPointInside(const math::vector2d& pos)
	{
		return GetBoundingBox(false).IsPointInside(pos);
	}

	float GetAlpha(){ return m_alpha; }

	virtual IKMDNode* GetNodeFromPoint(const math::vector2d& pos)
	{
		if (IsPointInside(pos))
			return this;
		return 0;
	}

	virtual math::rectf GetBoundingBox(bool includeChildren)
	{
		if (!m_phNode)
			return math::rectf(-m_sz*0.5f, m_sz*0.5f);
		return math::rectf (m_phNode->GetPosition() - math::vector2d(m_sz / 2), m_phNode->GetPosition() + m_sz / 2);
	}


	virtual void OnHoverOn(){ m_hovered = true; }
	virtual void OnHoverOff(){ m_hovered = false; }
	virtual void OnSelectedOn(){ m_selected = true; }
	virtual void OnSelectedOff(){ m_selected = false; }
	virtual bool IsHovered(bool checkParent=false){
		if (!m_hovered && checkParent && m_parent)
		{
			return m_parent->IsHovered(true);
		}

		return m_hovered; 
	}
};

}
}

#endif
