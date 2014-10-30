
#ifndef OldBaseController_h__
#define OldBaseController_h__

#include "IBaseController.h"
#include "Tserial_event.h"


namespace mray
{
	
class OldBaseController:public IBaseController
{
protected:
	Tserial_event *comROBOT;	// Serial Port
	bool connected;
public:
	OldBaseController();
	virtual ~OldBaseController();

	Tserial_event *GetComEvent(){ return comROBOT; }


	virtual bool Connect(const core::string& port) ;
	virtual bool IsConnected() ;
	virtual void Disconnect() ;

	virtual void Drive(const math::vector2di& speed, int rotationSpeed);
	virtual void DriveStop();

	virtual int GetSensorCount() { return 0; }
	virtual float GetSensorValue() { return 0; }
	virtual int GetBatteryLevel() { return 100; }
};

}

#endif // OldBaseController_h__


