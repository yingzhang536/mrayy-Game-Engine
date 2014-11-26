

#ifndef ForwardArrowComponent_h__
#define ForwardArrowComponent_h__

#include "IGameComponent.h"

namespace mray
{
namespace AugTel
{
	
class ForwardArrowComponent:public game::IGameComponent
{
	DECLARE_RTTI;
protected:

	core::string m_objectName;
	core::string m_mtrlName;

	scene::ISceneNode* m_forwardArrow;
	video::RenderPass* m_mtrl;

public:
	DECLARE_PROPERTY_TYPE(ObjectName, core::string, );
	DECLARE_PROPERTY_TYPE(ForwardMaterial, core::string, );
public:
	ForwardArrowComponent(game::GameEntityManager*m);
	virtual ~ForwardArrowComponent();

	virtual bool InitComponent();

	virtual void Update(float dt);

	DECLARE_SETGET_PROP(ObjectName, core::string);
	DECLARE_SETGET_PROP(ForwardMaterial, core::string);
	
};

DECLARE_GAMECOMPONENT_FACTORY(ForwardArrowComponent);
}
}

#endif // ForwardArrowComponent_h__
