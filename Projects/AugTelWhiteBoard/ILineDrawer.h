#ifndef ILineDrawer_h__
#define ILineDrawer_h__


#include "StreamReader.h"
#include "IUDPClient.h"
#include "SurfaceWrapper.h"

namespace mray
{

	class MovingAverage;
	class UDPLineReceivingThread;
class ILineDrawer
{
protected:

	OS::IMutex* m_mutex;
	network::IUDPClient* m_rcvClient;
	UDPLineReceivingThread* m_rcvThreadFunc;
	OS::IThread* m_rcvThread;

	AugTel::SurfaceWrapper m_wrapper;
	MovingAverage* m_movingAverage;

	bool m_drawing;

	int m_calibPoint;
	math::vector2d m_calibPoints[4];

	friend class UDPLineReceivingThread;

	virtual void _ReceiveData(OS::IStream* stream)=0;

	
	bool _getProjPos(math::vector2d& pos);

public:
	ILineDrawer();
	virtual ~ILineDrawer();

	virtual void AddGroup(int group) = 0;

	virtual void RemoveAt(const math::vector2d& pos) = 0;

	virtual bool RemoveLast(int group) = 0;

	virtual void DrawAt(const math::vector2d& pos, int group, const video::SColor& clr) = 0;

	virtual bool OnEvent(Event* e) = 0;

	virtual void StartReciver(int port) ;

	virtual void Draw(const math::rectf& vp) = 0;

	virtual void Update(float dt)=0;
};

}

#endif // ILineDrawer_h__
