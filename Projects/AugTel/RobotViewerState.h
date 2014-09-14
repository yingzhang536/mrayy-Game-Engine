

#ifndef RobotViewerState_h__
#define RobotViewerState_h__


#include "IEyesRenderingBaseState.h"
#include "ViewPort.h"
#include "DataCommunicator.h"
#include "TBeeCommon.h"

namespace mray
{
namespace AugTel
{
	
class RobotViewerState :public TBee::IEyesRenderingBaseState, public scene::IViewportListener, public IDataCommunicatorListener
{
	typedef TBee::IEyesRenderingBaseState Parent;
protected:
	bool m_takeScreenShot;
	TBee::ICameraVideoSource* m_camVideoSrc;
public:
	RobotViewerState(TBee::ICameraVideoSource* src, TBee::IRobotCommunicator* comm, const core::string& name);
	virtual ~RobotViewerState();

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void InitState();
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual video::IRenderTarget* Render(const math::rectf& rc, TBee::ETargetEye eye);
	virtual void Update(float dt);
	//virtual video::IRenderTarget* GetLastFrame(ETargetEye eye){ return m_viewport->getRenderTarget(); }
	virtual bool CanSleep(){ return false; }
};

}
}

#endif // RobotViewerState_h__

