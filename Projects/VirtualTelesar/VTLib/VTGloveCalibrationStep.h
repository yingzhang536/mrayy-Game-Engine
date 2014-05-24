



/********************************************************************
	created:	2014/05/15
	created:	15:5:2014   19:58
	filename: 	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib\VTGloveCalibrationStep.h
	file path:	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib
	file base:	VTGloveCalibrationStep
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __VTGloveCalibrationStep__
#define __VTGloveCalibrationStep__


#include "ICalibrationStep.h"

namespace mray
{
namespace VT
{
	class ICommunicationLayer;

	class VTGloveCalibrationStepImp;
class VTGloveCalibrationStep:public ICalibrationStep
{
protected:
	VTGloveCalibrationStepImp* m_imp;
public:
	VTGloveCalibrationStep(ICommunicationLayer* commLayer);
	virtual ~VTGloveCalibrationStep();


	virtual bool IsCompleted() const ;
	virtual void Update(float dt) ;

	virtual void LoadGUI(GUI::IGUIManager* m) ;
	virtual GUI::IGUIElement* GetGUIElement() ;
};

}
}

#endif
