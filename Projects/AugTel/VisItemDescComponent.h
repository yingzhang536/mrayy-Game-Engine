

#ifndef VisItemDescComponent_h__
#define VisItemDescComponent_h__

#include "SceneComponent.h"

namespace mray
{
namespace game
{

class VisItemDescComponent :public SceneComponent
{
	DECLARE_RTTI
protected:
	core::string m_description;

	DECLARE_PROPERTY_TYPE(Description, core::string, );

public:
	VisItemDescComponent(game::GameEntityManager* m);
	virtual ~VisItemDescComponent();

	virtual bool InitComponent();

	virtual void Update(float dt);

	DECLARE_SETGET_PROP(Description, core::string);
};

DECLARE_GAMECOMPONENT_FACTORY(VisItemDescComponent)

}
}

#endif // VisItemDescComponent_h__
