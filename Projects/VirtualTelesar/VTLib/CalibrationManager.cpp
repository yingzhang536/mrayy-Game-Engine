
#include "stdafx.h"
#include "CalibrationManager.h"

#include "ICalibrationStep.h"



namespace mray
{
namespace VT
{

CalibrationManager::CalibrationManager()
{
	m_currentStep = 0;
}
CalibrationManager::~CalibrationManager()
{
	for (int i = 0; i < m_calibSteps.size(); ++i)
		delete m_calibSteps[i];
	m_calibSteps.clear();
}

void CalibrationManager::_addCalibrationStep(ICalibrationStep* s)
{
	m_calibSteps.push_back(s);
}
void CalibrationManager::Reset()
{
	FIRE_LISTENR_METHOD(OnCalibrationReset, (this));
}
bool CalibrationManager::IsDone()
{
	return m_currentStep == m_calibSteps.size();
}
ICalibrationStep* CalibrationManager::GetCurrentCalibrationStep()
{
	if (m_currentStep < m_calibSteps.size())
		return m_calibSteps[m_currentStep];
	return 0;
}

void CalibrationManager::Update(float dt)
{
	ICalibrationStep* s = GetCurrentCalibrationStep();
	if (s)
	{
		s->Update(dt);
		if (s->IsCompleted())
		{
			m_currentStep++;
			if (m_currentStep == m_calibSteps.size())
			{
				FIRE_LISTENR_METHOD(OnCalibrationDone, (this));
			}
			else
			{
				FIRE_LISTENR_METHOD(OnCalibrationNextStep, (this,s));
			}
		}
	}
}

}
}
