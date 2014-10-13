

#ifndef IBaseController_h__
#define IBaseController_h__

#include "point2d.h"

namespace mray
{
	
class IBaseController
{
protected:
public:
	IBaseController(){}
	virtual ~IBaseController(){}


	virtual bool Connect(const core::string& port) = 0;
	virtual bool IsConnected() = 0;
	virtual void Disconnect()=0;

	virtual void Drive(const math::vector2di& speed, int rotationSpeed)=0;
	virtual void DriveStop()=0;
};

}

#endif // IBaseController_h__

