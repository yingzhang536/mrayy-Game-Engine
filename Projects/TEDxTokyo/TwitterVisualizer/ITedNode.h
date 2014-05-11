



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

class ITedNode
{
protected:
	msa::physics::Particle2D* m_phNode;
	float m_sz;
public:
	ITedNode() :m_phNode(0),m_sz(30)
	{}
	virtual ~ITedNode(){}

	void SetSize(float s){ m_sz = s; }
	float GetSize(){ return m_sz; }

	void SetPhysics(msa::physics::Particle2D* n){ m_phNode = n; }
	msa::physics::Particle2D* GetPhysics(){ return m_phNode ; }
	virtual void Update(float dt){}
	virtual void Draw(){}

	virtual bool IsPointInside(const math::vector2d& pos)
	{
		if (!m_phNode)
			return false;
		math::rectf rc(m_phNode->getPosition() - math::vector2d(m_sz / 2), m_phNode->getPosition() + m_sz / 2);
		return rc.IsPointInside(pos);

	}
};

}
}

#endif
