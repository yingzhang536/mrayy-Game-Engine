#include "stdafx.h"


#include "MeshResourceManager.h"
#include "IVideoDevice.h"
#include "ILogManager.h"

#include "3dsLoader.h"
#include "mdlLoaderV0.h"
#include "MeshV2Loader.h"
#include "AImpSceneLoader.h"




namespace mray{


core::string MeshResourceManager::getDefaultGroup(){
	return mT("mesh");
}

MeshResourceManager::MeshResourceManager()
:IResourceManager(mT("Mesh Resource Manager"))
{
	m_meshLoader=new loaders::MeshLoader();
	//addMeshLoader(new loaders::_3dsLoader());
	addMeshLoader(new loaders::mdlLoaderV0());
	addMeshLoader(new loaders::MeshV2Loader());
	loaders::AImpSceneLoader::InitImporters(m_meshLoader);

}
MeshResourceManager::~MeshResourceManager(){
	m_meshLoader=0;
	loaders::AImpSceneLoader::DestroyImporter();
}

void MeshResourceManager::addMeshLoader(loaders::IMeshLoader*loader){
	m_meshLoader->addLoader(loader);
}

GCPtr<scene::SMesh> MeshResourceManager::loadMesh(const core::string&name,bool instance){
	GCPtr<scene::SMesh> m=getOrCreate(name);
	if(!m)
		return m;
	m->calcCurrBoundingBox();
	if(instance)
	{
		return m->duplicateContextOnly();
	}
	return m->getDuplicate();
}
IResourcePtr MeshResourceManager::createInternal(const core::string& name){
	GCPtr<scene::SMesh> model=m_meshLoader->Load(name);
	return model;
}
IResourcePtr MeshResourceManager::loadResourceFromFile(OS::IStream* file){
	IResourcePtr res= m_meshLoader->Load(file);
	addResource(res,file->getStreamName());
	return res;
}


void MeshResourceManager::writeResourceToDist(const core::string&resName,const core::string&fileName){
}
void MeshResourceManager::writeResourceToDist(const IResourcePtr& resource,const core::string&fileName){

}
void MeshResourceManager::writeResourceToDist(ResourceHandle resource,const core::string&fileName){
	
}

}




