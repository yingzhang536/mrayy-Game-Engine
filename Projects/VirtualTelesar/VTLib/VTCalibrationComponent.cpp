

#include "stdafx.h"
#include "VTCalibrationComponent.h"
#include "VTCalibrationManager.h"

#include "RobotCommunicatorComponent.h"
#include "CommunicationManager.h"

namespace mray
{
namespace VT
{

VTCalibrationComponent::VTCalibrationComponent(game::GameEntityManager* m_)
{
	m_calibManager = new VTCalibrationManager();
}
VTCalibrationComponent::~VTCalibrationComponent()
{
	delete m_calibManager;
}

bool VTCalibrationComponent::InitComponent()
{
	if (!IVTComponent::InitComponent())
		return false;

	const std::list<IObjectComponent*>& lst= GetOwnerComponent()->GetComponent(RobotCommunicatorComponent::getClassRTTI());
	if (lst.size() == 0)
		return false;
	RobotCommunicatorComponent* commComp = dynamic_cast<RobotCommunicatorComponent*>(*lst.begin());
	ICommunicationLayer* layer= VT::CommunicationManager::getInstance().GetCommunicationLayer(commComp->GetName());
	if (layer)
		m_calibManager->Init(layer);

	return true;
}

	
void VTCalibrationComponent::Update(float dt)
{
	m_calibManager->Update(dt);
}

}
}

