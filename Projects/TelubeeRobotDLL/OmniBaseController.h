
#ifndef OmniBaseController_h__
#define OmniBaseController_h__

#include "IBaseController.h"
#include "Tserial_event.h"


namespace mray
{
	
class OmniBaseController:public IBaseController
{
protected:
	Tserial_event *comROBOT;	// Serial Port
	bool connected;
public:
	OmniBaseController();
	virtual ~OmniBaseController();

	Tserial_event *GetComEvent(){ return comROBOT; }


	virtual bool Connect(const core::string& port) ;
	virtual bool IsConnected() ;
	virtual void Disconnect() ;

	virtual void Drive(const math::vector2di& speed, int rotationSpeed);
	virtual void DriveStop();

	virtual int GetSensorCount() { return 0; }
	virtual float GetSensorValue(int s) { return 0; }
	virtual int GetBatteryLevel() { return 100; }
};

}

#endif // OmniBaseController_h__


