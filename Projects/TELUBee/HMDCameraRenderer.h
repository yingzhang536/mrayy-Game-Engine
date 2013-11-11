


/********************************************************************
	created:	2013/04/03
	created:	3:4:2013   17:09
	filename: 	F:\Development\mrayEngine\Projects\TELUBee\HMDCameraRenderer.h
	file path:	F:\Development\mrayEngine\Projects\TELUBee
	file base:	HMDCameraRenderer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __HMDCameraRenderer__
#define __HMDCameraRenderer__

#include "IHMDRenderer.h"


namespace mray
{
namespace TBee
{

	class CameraRenderingState;
	struct HMDCameraInfo
	{
		int id;
		int w;
		int h;
		int fps;
	};
class HMDCameraRenderer:public IHMDRenderer
{
protected:
	CameraRenderingState* m_camera;
	video::TextureUnit m_tUnit;
	HMDCameraInfo m_info[2];

public:
	HMDCameraRenderer();
	virtual ~HMDCameraRenderer();

	void SetCameraInfo(ETargetEye eye,const HMDCameraInfo& ifo);
	const HMDCameraInfo& GetCameraInfo(ETargetEye eye)const{return m_info[eye==Eye_Left?0:1];}

	CameraRenderingState* GetCameraState(){return m_camera;}

	void Grab(const math::rectf &rc,ETargetEye eye);
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
