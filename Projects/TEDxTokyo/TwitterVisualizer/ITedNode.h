



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
public:
	ITedNode() :m_phNode(0) {}
	virtual ~ITedNode(){}

	void SetPhysics(msa::physics::Particle2D* n){ m_phNode = n; }
	msa::physics::Particle2D* GetPhysics(){ return m_phNode ; }
	virtual void Update(float dt){}
	virtual void Draw(){}
};

}
}

#endif
