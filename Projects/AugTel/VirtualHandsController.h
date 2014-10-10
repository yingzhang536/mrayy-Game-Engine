
#ifndef VirtualHandsController_h__
#define VirtualHandsController_h__


#include "IHandsController.h"

namespace mray
{
namespace AugTel
{
	class VirtualHandsStateImpl;
	
class VirtualHandsController:public IHandsController
{
protected:
	VirtualHandsStateImpl* m_data;
	core::string m_model;
	float m_lightMapTimer;
	bool m_enabled;
public:
	VirtualHandsController();
	virtual ~VirtualHandsController();

	virtual void Init(AugTelSceneContext* context);
	virtual void Start(AugTelSceneContext* context) ;
	virtual void End(AugTelSceneContext* context) ;
	virtual void Update(float dt) ;
	virtual void RenderStart(const math::rectf& rc, TBee::ETargetEye eye);
	virtual void DebugRender(const math::rectf& rc, TBee::ETargetEye eye);

	virtual math::vector3d GetHandPosition(EHandType hand) ;
	virtual math::quaternion GetHandRotation(EHandType hand) ;
	virtual math::vector3d GetFingerPosition(EHandType hand, EFingerType finger) ;

	virtual void LoadFromXML(xml::XMLElement* e);
	virtual core::string GetHandsType(){ return "Virtual"; }

	virtual void SetEnabled(bool e) ;
	virtual bool IsEnabled() { return m_enabled; }
};

}
}

#endif // VirtualHandsController_h__

