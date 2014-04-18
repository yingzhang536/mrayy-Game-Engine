


/********************************************************************
	created:	2010/11/04
	created:	4:11:2010   15:29
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\ImageSetResourceManager.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	ImageSetResourceManager
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ImageSetResourceManager_h__
#define ImageSetResourceManager_h__

#include "IResourceManager.h"
#include "ISingleton.h"
#include "ImageSet.h"

namespace mray
{

class MRAY_DLL ImageSetResourceManager:public IResourceManager,public ISingleton<ImageSetResourceManager>
{
private:
protected:

	typedef std::map<core::string,ResourceHandle> ImageSetMap;
	ImageSetMap m_imageSets;

	virtual IResourcePtr createInternal(const core::string& name);
	virtual void removeInternal(const IResourcePtr& resource);
	virtual bool addInternal(const IResourcePtr& resource,const core::string& name);

	virtual IResourcePtr loadResourceFromFile(OS::IStream* file);

	virtual core::string getDefaultGroup();
public:
	ImageSetResourceManager();
	virtual ~ImageSetResourceManager();

	virtual IResourcePtr getOrCreate(const core::string& name);

	video::ImageSetPtr  getImageSet(const core::string& name);
	video::ImageSetPtr  loadImageSet(const core::string& path);

	virtual void writeResourceToDist(const core::string&resName,const core::string&fileName);
	virtual void writeResourceToDist(const IResourcePtr& resource,const core::string&fileName);
	virtual void writeResourceToDist(ResourceHandle resource,const core::string&fileName);

};

}
#define gImageSetResourceManager mray::ImageSetResourceManager::getInstance()
#endif // ImageSetResourceManager_h__
