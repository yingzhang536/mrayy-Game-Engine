#include "stdafx.h"

#include "ShaderResourceManager.h"
#include "GPUShader.h"
#include "GPUNullShaderProgram.h"
#include "IVideoDevice.h"
#include "IFileSystem.h"
#include "ILogManager.h"
#include "XMLWriter.h"
#include "XMLTree.h"

#include "DeviceCapabilites.h"
#include "EnumManager.h"
#include "CShaderFactory.h"
#include "StreamWriter.h"
#include "XMLTextNode.h"

namespace mray
{


ShaderResourceManager::ShaderResourceManager(video::IVideoDevice*device):IResourceManager(mT("Shader Resource Manager"))
{
	m_gpuFactory=new video::CShaderFactory();
	m_supportShaders=false;

	setDevice(device);
	
}
ShaderResourceManager::~ShaderResourceManager(){
	m_device=0;
	m_gpuFactory->clear();
	delete m_gpuFactory;
	m_nullShader=0;
}



void ShaderResourceManager::parseShaderXML(OS::IStream* file)
{
	if(!file)
		return;
	xml::XMLTree t;
	if(!t.load(file))
	{
		gLogManager.log(mT("Failed to load Shader xml file: ")+core::string(file->getStreamName()),ELL_WARNING);
		return;
	}
	xml::XMLElement*e=t.getSubElement(mT("Shaders"));
	if(!e)
		return;
	parseShaderXML(e);
}
void ShaderResourceManager::exportShaderXML(const OS::IStreamPtr& file,bool onlyUsed)
{
	if(file.isNull())
		return;
	ResourceMap::iterator it=m_resourceMap.begin();

	xml::XMLWriter writer;
	xml::XMLElement e(mT("Shaders"));
	exportShaderXML(&e);
	writer.addElement(&e);

	OS::StreamWriter w(file);

	w.writeString(writer.flush());
}
video::IGPUShaderProgramPtr ShaderResourceManager::parseShaderXMLElement(xml::XMLElement*elem,bool isAddResource)
{

	xml::XMLAttribute*name=elem->getAttribute(mT("Name"));
	if(isAddResource && getResource(name->value))
		return video::IGPUShaderProgramPtr::Null;
	xml::XMLAttribute*type=elem->getAttribute(mT("Type"));
	xml::XMLAttribute*path=elem->getAttribute(mT("Path"));
	xml::XMLAttribute*entryPoint=elem->getAttribute(mT("EntryPoint"));
	xml::XMLAttribute*programType=elem->getAttribute(mT("ProgramType"));
	if((!name && isAddResource) || !type || !entryPoint || !programType)
		return video::IGPUShaderProgramPtr::Null;
	video::EShaderProgramType t=(video::EShaderProgramType)EnumManager::getInstance().getValue(mT("ShaderProgramType"),programType->value);
	core::stringc epStr;
	core::string_to_char(entryPoint->value.c_str(),epStr);

	core::string program;
	if(path)
		program=path->value;
	else
	{
	 	xml::XMLTextNode* tnode=elem->GetTextNode(); // try to load from path attribute
		if(tnode)
		{
			program=tnode->GetValue(); // try to load from text node
		}else
			return video::IGPUShaderProgramPtr::Null; // failed to find a valid program to load
	}
	video::IGPUShaderProgramPtr m=loadShader(program,t,epStr.c_str(),type->value,path!=0);
	if(isAddResource)
		addResource(m,name->value);
	return m;
}

void ShaderResourceManager::parseShaderXML(xml::XMLElement*e)
{
	xml::xmlSubElementsMapIT it= e->getElementsBegin();
	xml::xmlSubElementsMapIT end= e->getElementsEnd();
	for(;it!=end;++it)
	{
		if((*it)->GetType()!=xml::ENT_Element)continue;
		xml::XMLElement*elem=dynamic_cast<xml::XMLElement*>(*it);
		if(elem->getName().equals_ignore_case(mT("Shader")))
		{
			parseShaderXMLElement(elem,true);
		}
	}
}
xml::XMLElement* ShaderResourceManager::exportShaderXML(xml::XMLElement*e,bool onlyUsed)
{
	//xml::XMLElement *e=new xml::XMLElement(mT("Shaders"));
	ResourceMap::iterator it=m_resourceMap.begin();
	for(;it!=m_resourceMap.end();++it)
	{
		video::IGPUShaderProgramPtr s=it->second;
		if(onlyUsed && s.getRefCount()==1)
			continue;
		xml::XMLElement *elem=new xml::XMLElement(mT("Shader"));
		e->addSubElement(elem);
		elem->addAttribute(mT("Name"),s->getName());
		elem->addAttribute(mT("Type"),s->getShaderType());
		elem->addAttribute(mT("EntryPoint"),s->GetEntryPoint());
		elem->addAttribute(mT("ProgramType"),EnumManager::getInstance().getName(mT("ShaderProgramType"),s->GetProgramType()));

	}
	//elem->addSubElement(e);
	return e;
}

IResourcePtr ShaderResourceManager::createInternal(const core::string& name){
	//return with default parameters
	//core::stringc str;
	//core::string_to_char(name,str);
	//return loadShader(str.c_str(),str.c_str());
	return IResourcePtr::Null;
}

IResourcePtr  ShaderResourceManager::loadResourceFromFile(OS::IStream* file)
{
	return IResourcePtr::Null;
/*

	if(!m_supportShaders)
		return m_nullShader;
	return m_gpuFactory->createShader(m_defaultShaderType,m_device,file);*/

}


core::string ShaderResourceManager::getDefaultGroup(){
	return mT("Shaders");
}

void ShaderResourceManager::setDevice(video::IVideoDevice* dev){
	m_device=dev;
	m_nullShader=new video::GPUNullShaderProgram(video::EShader_VertexProgram);
	m_nullShader->setResourceName(mT("NullShader"));
	if(!m_device)return;
// 	m_supportShaders=m_device->getCapabilities()->isFeatureSupported(video::EDF_VertexProgram) && 
// 		m_device->getCapabilities()->isFeatureSupported(video::EDF_FragmentProgram);

}
void ShaderResourceManager::addShaderLoader(video::IGPUShaderFactory* loader){
	m_gpuFactory->addFactory(loader);
}

video::IGPUShaderProgramPtr ShaderResourceManager::loadShader(const core::string&program,video::EShaderProgramType programType,
															  const char*entryPoint,const core::string&type,bool fromFile)
{
	if(!m_device)
		return video::IGPUShaderProgramPtr::Null;
	if(programType==video::EShader_VertexProgram && !m_device->getCapabilities()->isFeatureSupported(video::EDF_VertexProgram))
		return m_nullShader;
	if(programType==video::EShader_FragmentProgram && !m_device->getCapabilities()->isFeatureSupported(video::EDF_FragmentProgram))
		return m_nullShader;
	if(programType==video::EShader_GeometryProgram && !m_device->getCapabilities()->isFeatureSupported(video::EDF_GeometryProgram))
		return m_nullShader;

	video::IGPUShaderProgramPtr s;

	s=getResource(program);

	if(s)
		return s;

	core::string t;
	if(type==mT(""))
		t=m_defaultShaderType;
	else t=type;

	s=m_gpuFactory->createShader(type,m_device,programType,fromFile,program,entryPoint);

	//addResource(s,name);
	return s;
}


void ShaderResourceManager::setDefaultShaderType(const core::string&type){
	m_defaultShaderType=type;
}
const core::string& ShaderResourceManager::getDefaultShaderType(){
	return m_defaultShaderType;
}



const video::IGPUShaderProgramPtr& ShaderResourceManager::getDefaultShader(){
	return m_defaultShader;
}
void ShaderResourceManager::setDefaultShader(const video::IGPUShaderProgramPtr& shader){
	m_defaultShader=shader;
}


void ShaderResourceManager::writeResourceToDist(const core::string&resName,const core::string&fileName){

}
void ShaderResourceManager::writeResourceToDist(const IResourcePtr& resource,const core::string&fileName){

}
void ShaderResourceManager::writeResourceToDist(ResourceHandle resource,const core::string&fileName){

}


}



