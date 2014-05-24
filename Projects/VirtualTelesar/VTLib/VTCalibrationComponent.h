



/********************************************************************
	created:	2014/05/15
	created:	15:5:2014   21:12
	filename: 	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib\VTCalibrationComponent.h
	file path:	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib
	file base:	VTCalibrationComponent
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __VTCalibrationComponent__
#define __VTCalibrationComponent__


#include "IVTComponent.h"


namespace mray
{
namespace VT
{
	class VTCalibrationManager;

class VTCalibrationComponent :public IVTComponent
{
	DECLARE_RTTI;
protected:
	VTCalibrationManager* m_calibManager;
public:
	VTCalibrationComponent(game::GameEntityManager* m);
	virtual ~VTCalibrationComponent();

	virtual bool InitComponent();

	VTCalibrationManager* GetCalibrationManager(){ return m_calibManager; }

	virtual void Update(float dt);
};

DECLARE_GAMECOMPONENT_FACTORY(VTCalibrationComponent);


}
}

#endif
