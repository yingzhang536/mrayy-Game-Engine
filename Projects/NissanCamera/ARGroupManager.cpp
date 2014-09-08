
#include "stdafx.h"
#include "ARGroupManager.h"
#include "MeshBufferData.h"
#include "MeshResourceManager.h"




namespace mray
{
namespace NCam
{

ARGroupManager::ARGroupManager()
{
	m_sceneManager = 0;
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
	video::IHardwareStreamBuffer* normalStream = buffer->createStream(0, video::EMST_Normal, video::ESDT_Point3f, vertCount, video::IHardwareBuffer::EUT_Dynamic, false, true);

	//math::vector3d* pos = (math::vector3d*)posStream->lock(0, vertCount, video::IHardwareBuffer::ELO_Discard);
	//math::vector3d* normal = (math::vector3d*)normalStream->lock(0, vertCount, video::IHardwareBuffer::ELO_Discard);
	
	posStream->writeData(0, sizeof(math::vector3d)*vertCount, &obj->verticies[0]);
	normalStream->writeData(0, sizeof(math::vector3d)*vertCount, &obj->normals[0]);


	//posStream->unlock();
	//normalStream->unlock();

	if (obj->colorType == EARColorType::EVertex)
	{
		video::IHardwareStreamBuffer* colorStream = buffer->createStream(0, video::EMST_Color, video::ESDT_Point4f, vertCount, video::IHardwareBuffer::EUT_Dynamic, false, true);
		posStream->writeData(0, sizeof(video::SColor)*vertCount, &obj->colors[0]);
		
		/*video::SColor* color = (video::SColor*)colorStream->lock(0, vertCount, video::IHardwareBuffer::ELO_Discard);

		for (int i = 0; i < vertCount; ++i)
		{
			
		}
		colorStream->unlock();*/
	}
	gLogManager.log("AR custom mesh object was loaded: vertices["+core::StringConverter::toString(vertCount)+"]", ELL_SUCCESS);

	return node;
}

scene::IRenderable* ARGroupManager::LoadMeshObject(ARPredef* obj)
{
	scene::SMeshPtr mesh = gMeshResourceManager.loadMesh("ARObjects\\"+obj->name,true);
	if (!mesh)
	{

		gLogManager.log("Failed to load AR Predefined mesh object: " + obj->name, ELL_WARNING);
		return 0;

	}
	scene::MeshRenderableNode* node = new scene::MeshRenderableNode(mesh);
	gLogManager.log("AR Predefined mesh object was loaded: " + obj->name, ELL_SUCCESS);

	return node;
}
ARSceneGroup* ARGroupManager::AddGroup(ARGroup* group)
{
	ARSceneGroup* ret = GetGroupListByID(group->groupID);
	if (ret)
		return  ret;

	ret = new ARSceneGroup();
	ret->group = group;
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
		default:
			break;
		}


		if (!r)
			continue;
		scene::ISceneNode* sceneNode = m_sceneManager->createSceneNode("ARObject@" + core::StringConverter::toString(group->groupID) + "#" + core::StringConverter::toString(i));

		sceneNode->AttachNode(r);
		sceneNode->setPosition(group->objects[i]->pos);
		sceneNode->setOrintation(group->objects[i]->dir);

		ARSceneObject* obj = new ARSceneObject;
		obj->obj = group->objects[i];
		obj->group = ret;
		obj->sceneNode = sceneNode;
		ret->objects.push_back(obj);
	}
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

void ARGroupManager::RemoveGroup(uint id)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
	{
		ARSceneGroup* g = it->second;;
		for (int i = 0; i < g->objects.size(); ++i)
		{
			m_sceneManager->removeSceneNode(g->objects[i]->sceneNode);
		}
		delete g;
		m_groups.erase(it);
	}

}

void ARGroupManager::HideGroup(uint id)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
	{
		ARSceneGroup* g = it->second;;
		for (int i = 0; i < g->objects.size(); ++i)
		{
			g->objects[i]->sceneNode->setVisible(false);
		}
	}

}

void ARGroupManager::ShowGroup(uint id)
{
	GroupMap::iterator it = m_groups.find(id);
	if (it != m_groups.end())
	{
		ARSceneGroup* g = it->second;;
		for (int i = 0; i < g->objects.size(); ++i)
		{
			g->objects[i]->sceneNode->setVisible(true);
		}
	}


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

}
}