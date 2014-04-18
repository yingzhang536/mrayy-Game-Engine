

#include "stdafx.h"
#include "CalibHeadController.h"






namespace mray
{
namespace TBee
{

void CalibHeadController::CalibrationInfo::LoadFromXML(xml::XMLElement* e)
{
	headOrintation = core::StringConverter::toQuaternion(e->getValueString("HeadOrientation"));
	headPosition = core::StringConverter::toVector3d(e->getValueString("HeadPosition"));
	staticHeadOffset = core::StringConverter::toVector3d(e->getValueString("StaticHeadOffset"));
	calibrated = true;
}
void CalibHeadController::CalibrationInfo::WriteToXML(xml::XMLElement* elem)
{
	xml::XMLElement* e = new xml::XMLElement("Calibration");
	elem->addSubElement(e);
	e->addAttribute("HeadOrientation", core::StringConverter::toString(headOrintation));
	e->addAttribute("HeadPosition", core::StringConverter::toString(headPosition));
	e->addAttribute("StaticHeadOffset", core::StringConverter::toString(staticHeadOffset));
}




math::quaternion CalibHeadController::GetHeadOrientation()
{
	
	return m_otherController->GetHeadOrientation()*m_calibration.headOrintation.inverse();;
}

math::vector3d CalibHeadController::GetHeadPosition()
{
	return m_otherController->GetHeadPosition() - m_calibration.headPosition - m_calibration.staticHeadOffset;

}


void CalibHeadController::Calibrate()
{
	m_calibration.calibrated = true;
	m_calibration.headPosition = m_otherController->GetHeadPosition();
	m_calibration.headOrintation = m_otherController->GetHeadOrientation();
}


void CalibHeadController::LoadFromXML(xml::XMLElement* e)
{
	m_calibration.LoadFromXML(e);
}

void CalibHeadController::WriteToXML(xml::XMLElement* e)
{
	m_calibration.WriteToXML(e);
}

}
}
