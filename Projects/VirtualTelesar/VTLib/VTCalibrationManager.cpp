
#include "stdafx.h"
#include "VTCalibrationManager.h"
#include "ICommunicationLayer.h"
#include "VTGloveCalibrationStep.h"
#include "VTPostureCalibrationStep.h"

namespace mray
{
namespace VT
{

VTCalibrationManager::VTCalibrationManager()
{
}
VTCalibrationManager::~VTCalibrationManager()
{
}

void VTCalibrationManager::Init(ICommunicationLayer* comm)
{
	_addCalibrationStep(new VTGloveCalibrationStep(comm));
	_addCalibrationStep(new VTPostureCalibrationStep(comm));
	Reset();
}

}
}


