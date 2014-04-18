#include "stdafx.h"


#include "MeshLoader.h"
#include "IFileSystem.h"
#include "ILogManager.h"
#include "IMeshLoader.h"


namespace mray{
namespace loaders{

MeshLoader::~MeshLoader()
{
	std::list<IMeshLoader*>::iterator it=Loaders.begin();
	for(;it!=Loaders.end();++it)
		delete *it;
	Loaders.clear();
}

void MeshLoader::addLoader(IMeshLoader* loader)
{
	Loaders.push_back(loader);
}

scene::SMesh* MeshLoader::Load(const  core::string&name)
{/*
	core::string fileName;
	OS::FileSystem::getCorrectFilePath(name,fileName);*/
	scene::SMesh* mesh=0;
	std::list<IMeshLoader*>::iterator it=Loaders.begin();
	for(;it!=Loaders.end();++it)
		if((*it)->canLoad(name))
		{
			mesh=(*it)->load(name);
			break;
		}
	if(!mesh)
		gLogManager.log(core::string(mT("Mesh File Not Found: "))+name,ELL_WARNING);
	return mesh;
}
scene::SMesh* MeshLoader::Load(OS::IStream* file){
	scene::SMesh* mesh=0;
	std::list<IMeshLoader*>::iterator it=Loaders.begin();
	for(;it!=Loaders.end();++it)
		if((*it)->canLoad(file->getStreamName().c_str()))
		{
			mesh=(*it)->load(file->getStreamName().c_str());
			break;
		}
	if(!mesh)
		gLogManager.log(core::string(mT("Mesh File Not Found: "))+file->getStreamName(),ELL_WARNING);
	return mesh;
}













}
}
