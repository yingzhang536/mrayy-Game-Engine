

#ifndef LeapMotionController_h__
#define LeapMotionController_h__

#include "leap/Leap.h"

namespace mray
{
	
class LeapMotionController
{
protected:
	Leap::Controller m_LeapController;
public:
	LeapMotionController();
	virtual ~LeapMotionController();


	void Init();

	void AddListener(Leap::Listener* l){ m_LeapController.addListener(*l); }
	void RemoveListener(Leap::Listener* l){ m_LeapController.removeListener(*l); }

	Leap::Controller* GetController(){ return &m_LeapController; }
	
};

}

#endif // LeapMotionController_h__

