#include "stdafx.h"

#include "TextureResourceManager.h"
#include "IVideoDevice.h"
#include "ILogManager.h"

#include "TexLoader.h"
#include "TexWriter.h"

// #include "CTargaLoader.h"
// #include "CBmpLoader.h"
// #include "CRawLoader.h"
// #include "CJPGLoader.h"
// #include "CPNGLoader.h"
// #include "CPCXLoader.h"
#include "CDDSLoader.h"
#include "FreeImageLoader.h"
#include "CustomTexLoader.h"

// #include "CBmpWriter.h"
// #include "CJPGWriter.h"
// #include "CTargaWriter.h"
#include "CustomTexWriter.h"
#include "FreeImageWriter.h"


#include "IFileSystem.h"
#include "FileResourceManager.h"


namespace mray{

core::string TextureResourceManager::getDefaultGroup(){
	return mT("texture");
}

TextureResourceManager::TextureResourceManager(video::IVideoDevice*device)
:IResourceManager(mT("Texture Resource Manager")),m_device(device),m_defaultMipmapsCount(1024)
{

	m_def1D=0;
	m_def2D=0;
	m_def3D=0;
	m_defCube=0;

	m_texLoader=new loaders::TexLoader();
	m_texWriter=new loaders::TexWriter();
	/*

	addTextureLoader(new loaders::CPNGLoader);
	addTextureLoader(new loaders::CBmpLoader);
	addTextureLoader(new loaders::CTargaLoader);
	addTextureLoader(new loaders::CJPGLoader);
	addTextureLoader(new loaders::CRawLoader);
	addTextureLoader(new loaders::CPCXLoader);*/
	addTextureLoader(new loaders::CustomTexLoader);

//	addTextureLoader(new loaders::CDDSLoader);
	addTextureLoader(new loaders::FreeImageLoader);

	/*
	addTextureWriter(new loaders::CBmpWriter);
	addTextureWriter(new loaders::CTargaWriter);
	addTextureWriter(new loaders::CJPGWriter);
	*/
	addTextureWriter(new loaders::FreeImageWriter);
	addTextureWriter(new loaders::CustomTexWriter);

}
TextureResourceManager::~TextureResourceManager()
{
	delete m_texLoader;
	delete m_texWriter;
	m_def1D=0;
	m_def2D=0;
	m_def3D=0;
	m_defCube=0;
}

void TextureResourceManager::setDefaultTexture1D(video::ImageInfoCRef tex){
	m_def1D=tex;
}
void TextureResourceManager::setDefaultTexture2D(video::ImageInfoCRef tex){
	m_def2D=tex;
}
void TextureResourceManager::setDefaultTexture3D(video::ImageInfoCRef tex){
	m_def3D=tex;
}
void TextureResourceManager::setDefaultTextureCube(video::ImageInfoCRef tex){
	m_defCube=tex;
}

video::ImageInfoCRef TextureResourceManager::getDefaultTexture1D(){
	return m_def1D;
}
video::ImageInfoCRef  TextureResourceManager::getDefaultTexture2D(){
	return m_def2D;
}
video::ImageInfoCRef  TextureResourceManager::getDefaultTexture3D(){
	return m_def3D;
}
video::ImageInfoCRef  TextureResourceManager::getDefaultTextureCube(){
	return m_defCube;
}

void TextureResourceManager::setDevice(video::IVideoDevice* dev){
	m_device=dev;
}

void TextureResourceManager::addTextureLoader(loaders::ITexLoader*loader){
	m_texLoader->addLoader(loader);
}
void TextureResourceManager::addTextureWriter(loaders::ITexWriter*writer){
	m_texWriter->addWriter(writer);
}
void TextureResourceManager::removeTextureLoader(const core::string& ext){
	m_texLoader->removeLoader(ext);
}
void TextureResourceManager::removeTextureWriter(const core::string& ext){
	m_texWriter->removeWriter(ext);
}


video::ImageInfoPtr TextureResourceManager::loadImageInfo(const core::string&name,video::ETextureType target,bool useDefault)
{
	ImageInfoMap::iterator it=m_imageInfoList.find(name);
	if(it!=m_imageInfoList.end()){
		return it->second;
	}
	video::ImageInfoPtr info=new video::ImageInfo();

	if(!m_texLoader->load(name.c_str(),info,target)){
		gLogManager.log(mT("TextureResourceManager::loadImageInfo(): Image couldn't be loaded :\"")+name+mT("\""),ELL_WARNING);
		if(!useDefault)
			return video::ImageInfoPtr::Null;
		else
		{
			switch (target)
			{
			case video::ETT_1DTex:return m_def1D;
			case video::ETT_2DTex:return m_def2D;
			case video::ETT_3DTex:return m_def3D;
			case video::ETT_CubeTex:return m_defCube;
			default:
				return video::ImageInfoPtr::Null;
			}
		}
	}else
	{
		m_imageInfoList[name]=info;
	}
	return info;
}
video::ImageInfoPtr TextureResourceManager::loadImageInfo(OS::IStream*stream,video::ETextureType target,bool useDefault)
{

	ImageInfoMap::iterator it=m_imageInfoList.find(stream->getStreamName());
	if(it!=m_imageInfoList.end()){
		return it->second;
	}
	video::ImageInfoPtr info=new video::ImageInfo();

	if(!m_texLoader->load(stream,info,target)){
		gLogManager.log(mT("TextureResourceManager::loadImageInfo(): Image stream couldn't be loaded :\"")+stream->getStreamName()+mT("\""),ELL_WARNING);
		if(!useDefault)
			return video::ImageInfoPtr::Null;
		else
		{
			switch (target)
			{
			case video::ETT_1DTex:return m_def1D;
			case video::ETT_2DTex:return m_def2D;
			case video::ETT_3DTex:return m_def3D;
			case video::ETT_CubeTex:return m_defCube;
			default:
				return video::ImageInfoPtr::Null;
			}
		}
	}else
	{
		m_imageInfoList[stream->getStreamName()]=info;
	}
	return info;
}
//loaders

video::ITexturePtr  TextureResourceManager::loadTexture1D(const core::string&name,bool useDefault){
	IResourcePtr res=getResource(name);
	if(res)
		return res;
	/*
	GCPtr<video::ImageInfo> info=loadImageInfo(name,video::ETT_1DTex);
	if(!info){
		if(useDefault)
		{
			info=m_def1D;
		}
		else return 0;
	}

	video::ImageInfo* lst[1];
	lst[0]=info.pointer();
*/
	video::ITexturePtr  tex=createEmptyTexture1D(name,false);
	if(!tex)
		return 0;
	tex->AddLoadStream(gFileResourceManager.getFile(name));
//	tex->loadSurfaces(lst,1);
//	tex->load();

	/*
	int i=name.findlast('\\');
		core::string shortName;
		if(i!=-1)
		{
			shortName=name.substr(i,name.length()-i);
		}else
			shortName=name;*/
	
	//addResource(tex,name);
	return tex;
}
video::ITexturePtr  TextureResourceManager::loadTexture2D(const core::string&name,bool useDefault){
	IResourcePtr res=getResource(name);
	if(res)
		return res;
	/*GCPtr<video::ImageInfo> info=loadImageInfo(name,video::ETT_2DTex);
	if(!info){
		if(useDefault)
		{
			info=m_def2D;
		}
		else return 0;
	}*/
	video::ITexturePtr  tex=createEmptyTexture2D(name,false);
	if(!tex)
		return 0;
	tex->AddLoadStream(gFileResourceManager.getFile(name));
	/*video::ImageInfo* lst[1];
	lst[0]=info.pointer();
	tex->loadSurfaces(lst,1);*/
//	tex->load();

	/*
		int i=name.findlast('\\');
			core::string shortName;
			if(i!=-1)
			{
				shortName=name.substr(i,name.length()-i);
			}else
				shortName=name;*/
		
	//addResource(tex,name);
	return tex;
}
video::ITexturePtr  TextureResourceManager::loadTexture3D(const core::string&name,bool useDefault){
	IResourcePtr res=getResource(name);
	if(res)
		return res;
	/*
	GCPtr<video::ImageInfo> info=loadImageInfo(name,video::ETT_3DTex);
		if(!info){
			if(useDefault)
				return m_def3D;
			else return 0;
		}*/
	
	video::ITexturePtr  tex=createEmptyTexture3D(name,false);
	if(!tex)
		return 0;
	tex->AddLoadStream(gFileResourceManager.getFile(name));
	/*video::ImageInfo* lst[1];
	lst[0]=info.pointer();
	tex->loadSurfaces(lst,1);*/
//	tex->load();

	/*
	int i=name.findlast('\\');
		core::string shortName;
		if(i!=-1)
		{
			shortName=name.substr(i,name.length()-i);
		}else
			shortName=name;*/
	
	//addResource(tex,name);
	return tex;
}

video::ITexturePtr  TextureResourceManager::loadTextureCube(const core::string&name,const core::string* files,bool useDefault){
	IResourcePtr res=getResource(name);
	if(res)
		return res;
	/*
	video::ImageInfoPtr info=loadImageInfo(name,video::ETT_CubeTex);
		if(!info){
			if(useDefault)
			{
				info=m_defCube;
			}
			else return 0;
		}*/
	
	video::ITexturePtr  tex=createEmptyTextureCube(name,false);
	if(!tex)
		return 0;
	for(int i=0;i<6;++i)
		tex->AddLoadStream(gFileResourceManager.getFile(files[i]));
	/*video::ImageInfo* lst[1];
	lst[0]=info.pointer();
	tex->loadSurfaces(lst,1); //take to consideration faces of the cube (should be 6 surfaces)
	*/
//	tex->load();

	/*
	int i=name.findlast('\\');
	core::string shortName;
	if(i!=-1)
	{
		shortName=name.substr(i,name.length()-i);
	}else
		shortName=name;*/
	//addResource(tex,name);
	return tex;
}

video::ITexturePtr  TextureResourceManager::loadTexture1D(const core::string&name,OS::IStream*stream,bool useDefault)
{
	video::ITexturePtr  tex=createEmptyTexture1D(name,false);
	if(!tex)
		return 0;
	tex->AddLoadStream(stream);
	return tex;
}
video::ITexturePtr  TextureResourceManager::loadTexture2D(const core::string&name,OS::IStream*stream,bool useDefault)
{
	video::ITexturePtr  tex=createEmptyTexture2D(name,false);
	if(!tex)
		return 0;
	tex->AddLoadStream(stream);
	return tex;
}
video::ITexturePtr  TextureResourceManager::loadTexture3D(const core::string&name,OS::IStream*stream,bool useDefault)
{
	video::ITexturePtr  tex=createEmptyTexture3D(name,false);
	if(!tex)
		return 0;
	tex->AddLoadStream(stream);
	return tex;
}
video::ITexturePtr  TextureResourceManager::loadTextureCube(const core::string&name,OS::IStream**streams,bool useDefault)
{
	video::ITexturePtr  tex=createEmptyTextureCube(name,false);
	if(!tex)
		return 0;
	for(int i=0;i<6;++i)
	{
		tex->AddLoadStream(streams[i]);
	}
	return tex;
}


IResourcePtr TextureResourceManager::createInternal(const core::string& name){
// 	video::ITexturePtr  tex=m_texLoader->load(name.c_str());
// 	return tex;

	return 0;
}
IResourcePtr TextureResourceManager::loadResourceFromFile(OS::IStream* file){
	//	return m_texLoader->load(file);
	return 0;
}


/// create Textures

video::ITexturePtr  TextureResourceManager::createTexture1D(const core::string&name,int size,video::EPixelFormat format){
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createTexture1D(size,format,true);	
	addResource(tex,name);
	return tex;
}


video::ITexturePtr  TextureResourceManager::createTexture2D(const core::string&name,math::vector2di size,video::EPixelFormat format){
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createTexture2D(size,format,true);	
	addResource(tex,name);
	return tex;
}
video::ITexturePtr  TextureResourceManager::createTexture3D(const core::string&name,math::vector3di size,video::EPixelFormat format){
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createTexture3D(size,format,true);	
	addResource(tex,name);
	return tex;
}

video::ITexturePtr  TextureResourceManager::createTextureCube(const core::string&name,math::vector2di size,video::EPixelFormat format){
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createTextureCube(size,format,true);	
	addResource(tex,name);
	return tex;
}

video::ITexturePtr  TextureResourceManager::createEmptyTexture1D(const core::string&name,bool internal)
{
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createEmptyTexture1D(internal);	
	addResource(tex,name);
	return tex;
}
video::ITexturePtr  TextureResourceManager::createEmptyTexture2D(const core::string&name,bool internal)
{
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createEmptyTexture2D(internal);	
	addResource(tex,name);
	return tex;
}
video::ITexturePtr  TextureResourceManager::createEmptyTexture3D(const core::string&name,bool internal)
{
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createEmptyTexture3D(internal);	
	addResource(tex,name);
	return tex;
}
video::ITexturePtr  TextureResourceManager::createEmptyTextureCube(const core::string&name,bool internal)
{
	if(!m_device)return 0;
	video::ITexturePtr  tex=m_device->createEmptyTextureCube(internal);	
	addResource(tex,name);
	return tex;
}


void TextureResourceManager::writeResourceToDist(const core::string&resName,const core::string&fileName){
	IResourcePtr res=getResource(resName);
	if(!res)return;
	writeResourceToDist(res,fileName);
}
void TextureResourceManager::writeResourceToDist(const IResourcePtr& resource,const core::string&fileName){

	video::ITexturePtr tex=resource;
	if(!tex){
		gLogManager.log(mT("TextureResourceManager-invalied resource found! - resource name :\"")+
			resource->getResourceName()+mT("\""),ELL_ERROR);
		return;
	}
	OS::IStreamPtr file=gFileSystem.openFile(fileName,OS::BIN_WRITE);
	if(!file){
		gLogManager.log(mT("Couldn't open file for writing:\"")+
			fileName+mT("\""),ELL_ERROR);
		return;
	}
	int i=fileName.findlast('.');
	if(i==-1)
		return;
	core::string ext=fileName.substr(i+1,fileName.length()-i-1);
	m_texWriter->write(tex,file,ext);
	file->close();
}
void TextureResourceManager::writeResourceToDist(ResourceHandle resource,const core::string&fileName){
	IResourcePtr res=getResourceByHandle(resource);
	if(!res)return;
	writeResourceToDist(res,fileName);
}

}


