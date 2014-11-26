
#include "stdafx.h"
#include "ForwardArrowComponent.h"
#include "SceneComponent.h"
#include "ATAppGlobal.h"
#include "HeadMount.h"
#include "CRobotConnector.h"
#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"

namespace mray
{
	namespace AugTel
{

IMPLEMENT_SETGET_PROP(ForwardArrowComponent, ObjectName, core::string, m_objectName, "", , );
IMPLEMENT_SETGET_PROP(ForwardArrowComponent, ForwardMaterial, core::string, m_mtrlName, "", , );

ForwardArrowComponent::ForwardArrowComponent(game::GameEntityManager*m)
{
	m_mtrl = 0;
	m_forwardArrow = 0;
	CPropertieDictionary* dic;
	if (CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeObjectName::instance);
		dic->addPropertie(&PropertyTypeForwardMaterial::instance);
	}
}
ForwardArrowComponent::~ForwardArrowComponent()
{
}

bool ForwardArrowComponent::InitComponent()
{
	if (!game::IGameComponent::InitComponent())
		return false;

	game::SceneComponent* modelComp = game::IGameComponent::RetriveComponent<game::SceneComponent>(m_ownerComponent, m_objectName);
	if (modelComp)
	{
		modelComp->InitComponent();
		m_forwardArrow = modelComp->GetSceneNode();

		const scene::AttachNodesList& lst=m_forwardArrow->GetAttachedNodes();
		for (scene::AttachNodesList::const_iterator it = lst.begin(); it != lst.end(); ++it)
		{
			scene::IRenderable* r= *it;
			for (int i = 0; i < r->getMaterialCount(); ++i)
			{

				if (r->getMaterial(i)->GetName() == m_mtrlName)
				{
					m_mtrl = r->getMaterial(i)->GetTechniqueAt(0)->GetPassAt(0);
					break;
				}
			}
		}
	}

	return true;

}

void ForwardArrowComponent::Update(float dt)
{
	IGameComponent::Update(dt);
	if (m_forwardArrow)
	{

		math::vector3d pos = gAppData.headObject->getAbsolutePosition();;
		pos.y -= 0.1;
		m_forwardArrow->setPosition(pos);

		math::quaternion q;
		math::vector3d angles;
		gAppData.robotConnector->GetHeadRotation().toEulerAngles(angles);
		q.fromEulerAngles(angles.x, -angles.y, 0);

		m_mtrl->SetAlpha(math::Max<float>(0, gAppData.robotConnector->GetSpeed().x));

		m_forwardArrow->setOrintation(q);
	}
}


}
}



