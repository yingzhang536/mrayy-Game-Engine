



/********************************************************************
	created:	2014/05/15
	created:	15:5:2014   20:12
	filename: 	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib\VTCalibrationManager.h
	file path:	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib
	file base:	VTCalibrationManager
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __VTCalibrationManager__
#define __VTCalibrationManager__


#include "CalibrationManager.h"

namespace mray
{
namespace VT
{
	class ICommunicationLayer;

class VTCalibrationManager:public CalibrationManager
{
protected:
public:
	VTCalibrationManager();
	virtual ~VTCalibrationManager();

	void Init(ICommunicationLayer* comm);
};

}
}

#endif
