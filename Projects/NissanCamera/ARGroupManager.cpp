
#include "stdafx.h"
#include "ARGroupManager.h"
#include "MeshBufferData.h"
#include "MeshResourceManager.h"
#include "RenderTechnique.h"
#include "RenderPass.h"




namespace mray
{
namespace NCam
{

ARGroupManager::ARGroupManager()
{
	m_sceneManager = 0;
	m_vehicle = 0;
	m_arRoot = 0;
}

ARGroupManager::~ARGroupManager()
{

}

scene::IRenderable* ARGroupManager::GenerateMeshObject(ARMesh* obj)
{
	if (!obj->verticies.size())
		return 0;
	scene::MeshRenderableNode* node = new scene::MeshRenderableNode(0);
	scene::SMesh* mesh= node->getMesh();
	scene::MeshBufferData* bufData= mesh->addNewBuffer();
	scene::IMeshBuffer* buffer = bufData->getMeshBuffer();

	buffer->setRenderType((video::EMeshRenderType)obj->meshType);

	int vertCount = obj->verticies.size();

	video::IHardwareStreamBuffer* posStream = buffer->createStream(0, video::EMST_Position, video::ESDT_Point3f, vertCount, video::IHardwareBuffer::EUT_Dynamic, false, true);
	posStream->writeData(0, sizeof(math::vector3d)*vertCount, &obj->verticies[0]);

	if (obj->normalType == EARNormalType::EVertex)
	{
		video::IHardwareStreamBuffer* normalStream = buffer->createStream(0, video::EMST_Normal, video::ESDT_Point3f, vertCount, video::IHardwareBuffer::EUT_Dynamic, false, true);
		normalStream->writeData(0, sizeof(math::vector3d)*vertCount, &obj->normals[0]);
	}
	else if (obj->normalType == EARNormalType::EOverall)
	{

		video::IHardwareStreamBuffer* normalStream = buffer->createStream(0, video::EMST_Normal, video::ESDT_Point3f, vertCount, video::IHardwareBuffer::EUT_Dynamic, false, true);
		math::vector3d* n = (math::vector3d*)normalStream->lock(0, vertCount, video::IHardwareBuffer::ELO_Discard);
		for (int i = 0; i < vertCount; ++i)
		{
			*n = obj->normals[0];
			++n;
		}
		normalStream->unlock();
	}


	if (obj->colorType == EARColorType::EVertex)
	{
		video::IHardwareStreamBuffer* colorStream = buffer->createStream(0, video::EMST_Color, video::ESDT_Point4f, vertCount, video::IHardwareBuffer::EUT_Dynamic, false, true);
		colorStream->writeData(0, sizeof(video::SColor)*vertCount, &obj->colors[0]);
		
	}
	else if (obj->colorType == EARColorType::EOverall)
	{
		video::IHardwareStreamBuffer* colorStream = buffer->createStream(0, video::EMST_Color, video::ESDT_Point4f, vertCount, video::IHardwareBuffer::EUT_Dynamic, false, true);
		video::SColor* clr = (video::SColor*)colorStream->lock(0, vertCount, video::IHardwareBuffer::ELO_Discard);
		for (int i = 0; i < vertCount;++i)
		{
			*clr = obj->colors[0];
			++clr;
		}
		colorStream->unlock();
	}
	gLogManager.log("AR custom mesh object was loaded: vertices["+core::StringConverter::toString(vertCount)+"]", ELL_SUCCESS);

	bufData->setMaterial(gMaterialResourceManager.getMaterial("DefaultAR_Mtrl"));

	return node;
}

scene::IRenderable* ARGroupManager::LoadMeshObject(ARPredef* obj)
{
	scene::SMeshPtr mesh = gMeshResourceManager.loadMesh(obj->name,true);
	if (!mesh)
	{

		gLogManager.log("Failed to load AR Predefined mesh object: " + obj->name, ELL_WARNING);
		return 0;

	}
	scene::MeshRenderableNode* node = new scene::MeshRenderableNode(mesh);
	gLogManager.log("AR Predefined mesh object was loaded: " + obj->name, ELL_SUCCESS);

	return node;
}
scene::IRenderable* ARGroupManager::LoadStringObject(ARString* mesh)
{
	gLogManager.log("AR String was loaded", ELL_SUCCESS);

	return 0;
}

scene::IRenderable* ARGroupManager::LoadVehicleObject(ARVehicle* v)
{
	/*scene::SMeshPtr mesh = gMeshResourceManager.loadMesh("ARObjects\\Car.3ds", true);
	if (!mesh)
	{
		return 0;

	}
	scene::MeshRenderableNode* node = new scene::MeshRenderableNode(mesh);
	*/
	gLogManager.log("AR Vehicle was loaded" , ELL_SUCCESS);

	return 0;
//	return node;
}

void ARGroupManager::_UpdateGroup(ARSceneGroup* grp, ARGroup* src)
{
	delete grp->group;
	grp->group = src;
	for (int i = 0; i < src->objects.size(); ++i)
	{
		std::map<uint, ARSceneObject*>::iterator it= grp->objects.find(src->objects[i]->id);
		if (it != grp->objects.end())
		{
			scene::ISceneNode* sceneNode = it->second->sceneNode;
			sceneNode->setPosition(src->objects[i]->pos);
			sceneNode->setScale(src->objects[i]->scale);
			sceneNode->setOrintation(src->objects[i]->dir);

			it->second->group = grp;
			it->second->obj = src->objects[i];
		}
	}

}
ARSceneGroup* ARGroupManager::AddGroup(ARGroup* group)
{
	ARSceneGroup* ret = GetGroupListByID(group->groupID);
	if (ret)
	{
 		_UpdateGroup(ret, group);
 		return  ret;
	}
	ret = new ARSceneGroup();
	ret->group = 0;
	for (int i = 0; i < group->objects.size(); ++i)
	{
		scene::IRenderable* r=0;
		switch (group->objects[i]->objectType)
		{
		case EARObjectType::EARMesh:
			r = GenerateMeshObject((ARMesh*)group->objects[i]);
			break;
		case EARObjectType::EARPredef:
			r = LoadMeshObject((ARPredef*)group->objects[i]);
			break;
		case EARObjectType::EARString:
			r = LoadStringObject((ARString*)group->objects[i]);
			break;
		case EARObjectType::EARVehicle:
			r = LoadVehicleObject((ARVehicle*)group->objects[i]);
		default:
			break;
		}


		scene::ISceneNode* sceneNode = m_sceneManager->createSceneNode("ARObject@" + core::StringConverter::toString(group->groupID) + "#" + core::StringConverter::toString(i));

		if (group->objects[i]->objectType == EARObjectType::EARVehicle)
			m_vehicle = sceneNode;

		if (r)
			sceneNode->AttachNode(r);

		if (m_arRoot)
			m_arRoot->addChild(sceneNode);

	//	sceneNode->setCullingType(scene::SCT_NONE);
		sceneNode->setDrawBoundingBox(true);
		ARSceneObject* obj = new ARSceneObject;
		obj->id = group->objects[i]->id;
		obj->obj = 0; //later will be updated
		obj->sceneNode = sceneNode;
		ret->objects[obj->id]=(obj);
	}
	_UpdateGroup(ret, group);
	m_groups[group->groupID] = ret;
	return ret;
}

ARSceneGroup* ARGroupManager::GetGroupListByID(uint id)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
		return it->second;
	return 0;

}

bool ARGroupManager::RemoveGroup(uint id)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
	{
		ARSceneGroup* g = it->second;;
		std::map<uint, ARSceneObject*>::iterator it2= g->objects.begin();

		for (; it2 != g->objects.end();++it2)
		{
			m_sceneManager->removeSceneNode(it2->second->sceneNode);
		}
		delete g;
		m_groups.erase(it);
		return true;
	}
	return false;

}

bool ARGroupManager::HideGroup(uint id)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
	{
		ARSceneGroup* g = it->second;;
		std::map<uint, ARSceneObject*>::iterator it2 = g->objects.begin();

		for (; it2 != g->objects.end(); ++it2)
		{
			it2->second->sceneNode->setVisible(false);
		}
		return true;
	}
	return false;

}

bool ARGroupManager::ShowGroup(uint id)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
	{
		ARSceneGroup* g = it->second;;
		std::map<uint, ARSceneObject*>::iterator it2 = g->objects.begin();

		for (; it2 != g->objects.end(); ++it2)
		{
			it2->second->sceneNode->setVisible(true);
		}
		return true;
	}
	return false;


}
void ARGroupManager::ClearGroups()
{
	GroupMap::iterator it = m_groups.begin();
	for (; it != m_groups.end();++it)
	{
		delete it->second;
	}
	m_groups.clear();

}

bool ARGroupManager::SetAlpha(float alpha)
{
	GroupMap::iterator it = m_groups.begin();
	for (; it != m_groups.end(); ++it)
	{
		SetAlpha(it->first,alpha);
	}
	return true;
}
bool ARGroupManager::SetAlpha(uint id, float alpha)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
	{
		ARSceneGroup* g = it->second;;
		std::map<uint, ARSceneObject*>::iterator it2 = g->objects.begin();

		for (; it2 != g->objects.end(); ++it2)
		{
			it2->second->alpha = alpha;
			const scene::AttachNodesList& lst = it2->second->sceneNode->GetAttachedNodes();
			scene::AttachNodesList::const_iterator it2 = lst.begin();
			for (; it2 != lst.end();++it2)
			{
				for (int j = 0; j < (*it2)->getMaterialCount(); ++j)
				{
					(*it2)->getMaterial(j)->GetActiveTechnique()->GetPassAt(0)->SetAlpha(alpha);
					if (alpha<1)
						(*it2)->getMaterial(j)->GetActiveTechnique()->GetPassAt(0)->setMaterialRenderer(video::MRT_TRANSPARENT);
					else 
						(*it2)->getMaterial(j)->GetActiveTechnique()->GetPassAt(0)->setMaterialRenderer(video::MRT_SOLID);
				}
			}

		}
		return true;
	}
	return false;

}

}
}