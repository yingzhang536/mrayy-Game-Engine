

/********************************************************************
created:	2014/03/25
created:	25:3:2014   15:29
filename: 	C:\Development\mrayEngine\Projects\NissanCamera\CalibHeadController.h
file path:	C:\Development\mrayEngine\Projects\NissanCamera
file base:	CalibHeadController
file ext:	h
author:		MHD Yamen Saraiji
	
purpose:	
*********************************************************************/

#ifndef __CalibHeadController__
#define __CalibHeadController__


#include "IHeadController.h"

namespace mray
{
namespace TBee
{


class CalibHeadController :public IHeadController
{
protected:
	IHeadController* m_otherController;

	struct CalibrationInfo
	{
		CalibrationInfo()
		{
			calibrated = false;
		}
		math::vector3d headPosition;
		math::vector3d staticHeadOffset;
		math::quaternion headOrintation;

		bool calibrated;

		void LoadFromXML(xml::XMLElement* e);
		void WriteToXML(xml::XMLElement* e);
	};

	CalibrationInfo m_calibration;
public:
	CalibHeadController(IHeadController* o) :m_otherController(o)
	{}
	virtual~CalibHeadController(){}


	virtual math::quaternion GetHeadOrientation();
	virtual math::vector3d GetHeadPosition();

	void Calibrate();

	void LoadFromXML(xml::XMLElement* e);
	void WriteToXML(xml::XMLElement* e);
};

}
}


#endif
