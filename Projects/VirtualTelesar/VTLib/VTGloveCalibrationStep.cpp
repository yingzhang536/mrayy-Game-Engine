

#include "stdafx.h"
#include "VTGloveCalibrationStep.h"
#include "ICommunicationLayer.h"

#include "IGUIManager.h"
#include "IGUIElement.h"


namespace mray
{
namespace VT
{

	class VTGloveCalibrationStepImp
	{
	public:
		VTGloveCalibrationStepImp()
		{
			root = 0;
			commLayer = 0;
		}
		ICommunicationLayer* commLayer;

		GUI::IGUIElement* root;
	};

VTGloveCalibrationStep::VTGloveCalibrationStep(ICommunicationLayer* commLayer)
:ICalibrationStep("Glove")
{
	m_imp = new VTGloveCalibrationStepImp();
	m_imp->commLayer = commLayer;
}

VTGloveCalibrationStep::~VTGloveCalibrationStep()
{
	delete m_imp;
}



bool VTGloveCalibrationStep::IsCompleted() const
{
	return m_imp->commLayer->InjectCommand("done?", "glove").equals_ignore_case("yes");
}

void VTGloveCalibrationStep::Update(float dt)
{
}


void VTGloveCalibrationStep::LoadGUI(GUI::IGUIManager* m)
{
}

GUI::IGUIElement* VTGloveCalibrationStep::GetGUIElement()
{
	return m_imp->root;
}

}
}
