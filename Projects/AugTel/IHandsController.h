

#ifndef IHandsController_h__
#define IHandsController_h__


#include "GameEntityManager.h"
#include "TBeeCommon.h"

namespace mray
{
namespace AugTel
{
	class AugTelSceneContext;

	enum class EHandType
	{
		Left,
		Right
	};

	enum class EFingerType
	{
		Thumb,
		Index,
		Middle,
		Ring,
		Little
	};

class IHandsController
{
protected:
public:
	IHandsController(){}
	virtual ~IHandsController(){}

	virtual void Init(AugTelSceneContext* context) = 0;
	virtual void Start(AugTelSceneContext* context) = 0;
	virtual void End(AugTelSceneContext* context) = 0;
	virtual void Update(float dt) = 0;
	virtual void DebugRender(scene::IDebugDrawManager* dbg, const math::rectf& rc, TBee::ETargetEye eye) = 0;
	virtual void RenderStart(const math::rectf& rc, TBee::ETargetEye eye) = 0;

	virtual math::vector3d GetHandPosition(EHandType hand) = 0;
	virtual math::quaternion GetHandRotation(EHandType hand) = 0;
	virtual math::vector3d GetFingerPosition(EHandType hand, EFingerType finger) = 0;

	virtual void LoadFromXML(xml::XMLElement* e) = 0;

	virtual core::string GetHandsType() = 0;

	virtual void SetEnabled(bool e) = 0;
	virtual bool IsEnabled() = 0;
	
};

}
}

#endif // IHandsController_h__

