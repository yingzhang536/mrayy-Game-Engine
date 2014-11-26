
#ifndef LeapHandComponent_h__
#define LeapHandComponent_h__



#include "ArmComponent.h"


namespace mray
{
	namespace game
	{
		class IMovableComponent;
	}

	class LeapHand;

namespace AugTel
{
	
class LeapHandComponent :public ArmComponent
{
	DECLARE_RTTI;
protected:
public:
	LeapHand* m_Hand;
public:

	LeapHandComponent(game::GameEntityManager*m);
	virtual~LeapHandComponent();

	virtual bool InitComponent();

	virtual void Update(float dt);

	void SetHand(LeapHand*h);
};
DECLARE_GAMECOMPONENT_FACTORY(LeapHandComponent);

}
}

#endif // LeapHandComponent_h__


