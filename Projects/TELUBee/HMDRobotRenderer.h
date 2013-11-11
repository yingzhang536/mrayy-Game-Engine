



/********************************************************************
	created:	2013/04/03
	created:	3:4:2013   17:55
	filename: 	F:\Development\mrayEngine\Projects\TELUBee\HMDRobotRenderer.h
	file path:	F:\Development\mrayEngine\Projects\TELUBee
	file base:	HMDRobotRenderer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __HMDRobotRenderer__
#define __HMDRobotRenderer__



#include "IHMDRenderer.h"


namespace mray
{
namespace TBee
{
	class IVideoProvider;
class HMDRobotRenderer:public IHMDRenderer
{
protected:
	IVideoProvider* m_provider;
	video::TextureUnit m_tUnit;
	video::ITexturePtr m_remoteTex[2];
public:
	HMDRobotRenderer();
	virtual ~HMDRobotRenderer();

	IVideoProvider* GetProvider(){return m_provider;}

	void Grab();
	virtual void Init(Application*app);
	virtual void Render(const math::rectf &rc,ETargetEye eye);
	virtual void Update(float dt);

	virtual void Enter();
	virtual void Exit();

	virtual void LoadFromXML(xml::XMLElement* e);
};
}
}

#endif