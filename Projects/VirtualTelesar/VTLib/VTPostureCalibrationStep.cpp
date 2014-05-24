
#include "stdafx.h"
#include "VTPostureCalibrationStep.h"
#include "ICommunicationLayer.h"

#include "IGUIManager.h"
#include "IGUIElement.h"


namespace mray
{
namespace VT
{

class VTPostureCalibrationStepImp
{
public:
	VTPostureCalibrationStepImp()
	{
		root = 0;
		commLayer = 0;
	}
	ICommunicationLayer* commLayer;

	GUI::IGUIElement* root;
};

VTPostureCalibrationStep::VTPostureCalibrationStep(ICommunicationLayer* commLayer)
	:ICalibrationStep("Posture")
{
	m_imp = new VTPostureCalibrationStepImp();
	m_imp->commLayer = commLayer;
}

VTPostureCalibrationStep::~VTPostureCalibrationStep()
{
	delete m_imp;
}



bool VTPostureCalibrationStep::IsCompleted() const
{
	return m_imp->commLayer->InjectCommand("done?", "posture").equals_ignore_case("yes");
}

void VTPostureCalibrationStep::Update(float dt)
{
}


void VTPostureCalibrationStep::LoadGUI(GUI::IGUIManager* m)
{
}

GUI::IGUIElement* VTPostureCalibrationStep::GetGUIElement()
{
	return m_imp->root;
}

}
}

