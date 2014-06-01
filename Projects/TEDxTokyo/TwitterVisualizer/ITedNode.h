



/********************************************************************
	created:	2014/05/07
	created:	7:5:2014   18:08
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\ITedNode.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	ITedNode
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __ITedNode__
#define __ITedNode__




namespace mray
{
namespace scene
{
	class NodeRenderer;
class ITedNode
{
protected:
	msa::physics::Particle2D* m_phNode;
	float m_sz;
	bool m_hovered, m_selected;
	ITedNode* m_parent;
	float m_alpha;
public:
	ITedNode() :m_phNode(0), m_sz(30), m_hovered(false), m_parent(0), m_alpha(1), m_selected(false)
	{}
	virtual ~ITedNode(){}

	ITedNode* GetParent(){ return m_parent; }

	void SetSize(float s){ m_sz = s; }
	float GetSize(){ return m_sz; }
	virtual math::vector2d GetPosition()
	{
		return m_phNode ? m_phNode->getPosition() : math::vector2d(0);
	}

	void SetParent(ITedNode* p)
	{
		m_parent = p;
	}
	void SetPhysics(msa::physics::Particle2D* n){ m_phNode = n; }
	msa::physics::Particle2D* GetPhysics(){ return m_phNode ; }
	virtual void Update(float dt){}
	virtual void Draw(NodeRenderer *r, const math::rectf& rc){}

	virtual bool IsPointInside(const math::vector2d& pos)
	{
		return GetBoundingBox(false).IsPointInside(pos);
	}

	float GetAlpha(){ return m_alpha; }

	virtual ITedNode* GetNodeFromPoint(const math::vector2d& pos)
	{
		if (IsPointInside(pos))
			return this;
		return 0;
	}

	virtual math::rectf GetBoundingBox(bool includeChildren)
	{
		if (!m_phNode)
			return math::rectf(-m_sz*0.5f, m_sz*0.5f);
		return math::rectf (m_phNode->getPosition() - math::vector2d(m_sz / 2), m_phNode->getPosition() + m_sz / 2);
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
