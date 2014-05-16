



/********************************************************************
	created:	2014/05/15
	created:	15:5:2014   20:10
	filename: 	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib\PostureCalibrationStep.h
	file path:	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib
	file base:	PostureCalibrationStep
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __PostureCalibrationStep__
#define __PostureCalibrationStep__



#include "ICalibrationStep.h"


namespace mray
{
namespace VT
{
	class ICommunicationLayer;

	class VTPostureCalibrationStepImp; 
class VTPostureCalibrationStep :public ICalibrationStep
{
protected:
	VTPostureCalibrationStepImp* m_imp;
public:
	VTPostureCalibrationStep(ICommunicationLayer* commLayer);
	virtual ~VTPostureCalibrationStep();


	virtual bool IsCompleted() const;
	virtual void Update(float dt);

	virtual void LoadGUI(GUI::IGUIManager* m);
	virtual GUI::IGUIElement* GetGUIElement();
};

}
}

#endif
