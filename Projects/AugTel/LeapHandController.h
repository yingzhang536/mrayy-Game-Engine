
#ifndef LeapHandController_h__
#define LeapHandController_h__



namespace mray
{
	namespace scene
	{
		class IMovable;
	}

	class LeapHand;
	class LeapMotionController;
	class LeapHandController;

	class ILeapHandControllerListener
	{
	public:
		virtual void OnHandCreated(LeapHandController* c,LeapHand* hand){}
		virtual void OnHandRemoved(LeapHandController* c, LeapHand* hand){}
	};

class LeapHandController:public ListenerContainer<ILeapHandControllerListener*>
{
protected:
	math::vector3d m_handMovementScale;
	scene::IMovable* m_transform;

	LeapMotionController* m_controller;

	typedef std::map<int, LeapHand*> HandsMap;
	HandsMap m_hands;

	LeapHand* m_leftHand;
	LeapHand* m_rightHand;

	DECLARE_FIRE_METHOD(OnHandCreated, (LeapHandController* c, LeapHand* hand), (c,hand));
	DECLARE_FIRE_METHOD(OnHandRemoved, (LeapHandController* c, LeapHand* hand), (c, hand));
public:
	LeapHandController(LeapMotionController* c);
	virtual ~LeapHandController();

	void SetTransform(scene::IMovable* t){ m_transform=t; }
	scene::IMovable* GetTransform(){ return m_transform; }

	math::vector3d GetHandMovementScale(){ return m_handMovementScale; }

	LeapHand* GetleftHand(){ return m_leftHand; }
	LeapHand* GetRightHand(){ return m_rightHand; }

	void Update(float dt);
};

}

#endif // LeapHandController_h__

