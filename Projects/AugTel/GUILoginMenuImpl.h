

#ifndef GUILoginMenuImpl_h__
#define GUILoginMenuImpl_h__

#include "GUILoginMenu.h"
#include "TBRobotInfo.h"

namespace mray
{
	
class GUILoginMenuImpl :public GUILoginMenu
{
protected:
public:
	GUILoginMenuImpl();
	virtual ~GUILoginMenuImpl();


	void UpdateRobotList();
	TBee::TBRobotInfo* UpdateSelectedRobot();
};

}

#endif // GUILoginMenuImpl_h__
