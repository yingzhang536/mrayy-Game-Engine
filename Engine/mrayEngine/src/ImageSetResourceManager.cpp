#include "stdafx.h"

#include "ImageSetResourceManager.h"

#include "XMLTree.h"
#include "IFileSystem.h"

#include "ILogManager.h"

namespace mray
{


ImageSetResourceManager::ImageSetResourceManager()
	:IResourceManager(mT("ImageSet Resource Manager"))
{
}
ImageSetResourceManager::~ImageSetResourceManager()
{
}

IResourcePtr ImageSetResourceManager::createInternal(const core::string& name)
{
	OS::IStreamPtr stream=gFileSystem.openFile(name,OS::BIN_READ);
	if(stream.pointer()==0)
		return IResourcePtr::Null;
	IResourcePtr res= loadResourceFromFile(stream);
	stream->close();
	return res;
}
void ImageSetResourceManager::removeInternal(const IResourcePtr& resource)
{
	video::ImageSet*set=(video::ImageSet*)resource.pointer();
	m_imageSets.erase(set->GetName());
	IResourceManager::removeInternal(resource);
}
bool ImageSetResourceManager::addInternal(const IResourcePtr& resource,const core::string& name)
{
	bool res=IResourceManager::addInternal(resource,name);
	if(res)
	{
		video::ImageSet*set=(video::ImageSet*)resource.pointer();
		set->unload();
		set->load(false);
		m_imageSets[set->GetName()]=resource->getResourceHandle().ID();
	}
	return res;
}
IResourcePtr ImageSetResourceManager::loadResourceFromFile(OS::IStream* file)
{
	if(!file)
		return IResourcePtr::Null;
	xml::XMLTree xmlTree;
	if(!xmlTree.load(file)){
		gLogManager.log(mT("Couldn't parse XML ImageSet File :\"")+core::string(file->getStreamName())+mT("\""),ELL_WARNING);
		return IResourcePtr::Null;
	}

	xml::XMLElement*elem= xmlTree.getSubElement(mT("ImageSet"));
	if(!elem)
		return IResourcePtr::Null;
	video::ImageSetPtr iset=new video::ImageSet();
	iset->Deserialize(elem);
	//addResource(iset,iset->GetName());
	return iset;
}

core::string ImageSetResourceManager::getDefaultGroup()
{
	return mT("ImageSets");
}

IResourcePtr ImageSetResourceManager::getOrCreate(const core::string& name)
{
	IResourcePtr resource=getResource(name);
	if(!resource){
		return create(name);
	}else
		return resource;
}

video::ImageSetPtr  ImageSetResourceManager::getImageSet(const core::string& name)
{
	ImageSetMap::iterator it= m_imageSets.find(name);
	if(it==m_imageSets.end())return 0;
	return getResourceByHandle(it->second);
}
video::ImageSetPtr  ImageSetResourceManager::loadImageSet(const core::string& path)
{
	return create(path);
}

void ImageSetResourceManager::writeResourceToDist(const core::string&resName,const core::string&fileName)
{
}
void ImageSetResourceManager::writeResourceToDist(const IResourcePtr& resource,const core::string&fileName)
{
}
void ImageSetResourceManager::writeResourceToDist(ResourceHandle resource,const core::string&fileName)
{
}

}

