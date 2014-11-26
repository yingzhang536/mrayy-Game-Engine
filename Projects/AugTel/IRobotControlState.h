


#ifndef IRobotControlState_h__
#define IRobotControlState_h__

#include "IEyesRenderingBaseState.h"

namespace mray
{
namespace TBee
{
	
class IRobotControlState:public IEyesRenderingBaseState
{
protected:
	CRobotConnector* m_robotConnector;
public:
	IRobotControlState(const core::string& name);
	virtual ~IRobotControlState();

	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual void Update(float dt);
	
};

}
}

#endif // IRobotControlState_h__
