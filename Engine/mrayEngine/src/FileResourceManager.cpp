#include "stdafx.h"

#include "FileResourceManager.h"
#include "IFileSystem.h"


namespace mray{

FileResourceManager::FileResourceManager()
	:IResourceManager(mT("File Resource Manager"))
{
}
FileResourceManager::~FileResourceManager(){
}

IResourcePtr FileResourceManager::createInternal(const core::string& name){
	OS::IStreamPtr f=gFileSystem.createBinaryFileReader(name.c_str());
	if(f)
		f->setResourceName(name);
	return f;
}

IResourcePtr FileResourceManager::loadResourceFromFile(OS::IStream* file){
	return 0;
}

core::string FileResourceManager::getDefaultGroup(){
	return mT("Files");
}

OS::IStreamPtr FileResourceManager::getFile(const core::string&name){
	return getOrCreate(name);
}

void FileResourceManager::writeResourceToDist(const core::string&resName,const core::string&fileName){
}
void FileResourceManager::writeResourceToDist(const IResourcePtr& resource,const core::string&fileName){
}
void FileResourceManager::writeResourceToDist(ResourceHandle resource,const core::string&fileName){
}


}