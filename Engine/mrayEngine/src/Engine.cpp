#include "stdafx.h"


#include "Engine.h"

#include "AsyncLoadManager.h"

#include "XMLExpatParser.h"
#include "common.h"
#include "CSettingsFile.h"
#include "SkeletonResourceManager.h"
#include "ShaderSemanticTable.h"
//#include "StreamLogger.h"
#include "MaterialResourceManager.h"
#include "FileResourceManager.h"
#include "ScriptResourceManager.h"
#include "MeshAnimatorManager.h"


#include "MeshFileCreatorManager.h"

#include "LogManager.h"
#include "GUIOverlayManager.h"
#include "EnumManager.h"
#include "DefaultEnum.h"
#include "SceneHelper.h"
#include "CommandManager.h"
#include "DefaultObjects.h"

#include "SkyBoxManager.h"

#include "TextDecorateNodeFactory.h"
#include "DeviceCapabilites.h"

#include "LogSubSystemManager.h"
#include "FunctionProfileManager.h"

#include "BenchmarkItem.h"


#include "VideoResourceManager.h"
#include "TextureResourceManager.h"
#include "SoundResourceManager.h"
#include "MeshResourceManager.h"
#include "FontResourceManager.h"
#include "ResourceGroup.h"
#include "ShaderResourceManager.h"
#include "AnimationResourceManager.h"
#include "MaterialResourceManager.h"
#include "ImageSetResourceManager.h"
#include "ScheduleManager.h"
#include "MeshGenerator.h"
#include "TraceManager.h"
#include "ArchiveManager.h"
#include "IOSRegistry.h"
#include "ShaderSemanticTable.h"
#include "GUIThemeManager.h"
#include "IDllManager.h"
#include "ILogManager.h"
#include "PluginManager.h"
#include "SoundManagerFactory.h"
#include "IErrorDialog.h"
#include "ITimer.h"
#include "CFPS.h"
#include "SParticleSystemFactory.h"
#include "SMeshManipulator.h"
#include "EventQueue.h"
#include "CPUInfo.h"
#include "JobPool.h"
#include "DelegateManager.h"
#include "IDeviceFactory.h"
#include "IFileSystem.h"
#include "GUIElementFactory.h"
#include "TextureSourceFactory.h"


namespace mray
{


Engine::Engine(GCPtr<OS::IOSystem> system)
{


	m_rootBenchmark=new BenchmarkItem(mT("Engine"));

	new FunctionProfileManager();
	new DefaultObjects();

	DefaultObjects::registerDefaultObjects();

	new EnumManager();
	DefaultEnum::registerDefaultEnum();

	new CommandManager();

	m_ioSystem=system;

	m_traceManager=new TraceManager();

	traceFunction(eEngine);


	m_delegateManager=new DelegateManager();

	m_scheduleManager=new ScheduleManager();

	m_mathTrigTable=new math::MTrigTable();


	m_loggerManager=new LogManager();

	new LogSubSystemManager();

	m_cpuInfo=new CPUInfo();

	m_eventQueue=new EventQueue();

	m_jobPool=new JobPool();

	m_particleSystemFactory=new scene::SParticleSystemFactory();

	m_meshManipulator=new scene::SMeshManipulator();


// 	GCPtr<StreamLogger> logger= new StreamLogger(true);
// 	m_loggerManager->addLogDevice(logger);
// 	logger->setStream(gFileSystem.createTextFileWriter(mT("log.txt")));

	m_resourceGroup=new ResourceGroup();

	m_shaderSemTable=new video::ShaderSemanticTable();

	new scene::MeshFileCreatorManager();


	new ScriptResourceManager();

	new scene::SceneHelper();

	new scene::SkyBoxManager();

	new AsyncLoadManager();

	m_pluginManager=new PluginManager();

	new FileResourceManager();
	new AnimationResourceManager();
	new SkeletonResourceManager();
	new MaterialResourceManager();
	new SoundResourceManager();
	new MeshResourceManager();
	new TextureResourceManager(0);
	new FontResourceManager(0);
	new ShaderResourceManager(0);
	new ImageSetResourceManager();
	new VideoResourceManager();

	new video::TextureSourceFactory();

	m_soundManagerFactory=new sound::SoundManagerFactory();

	m_meshGenerator=new scene::MeshGenerator();
	

	m_xmlParser=new xml::XMLExpatParser();


	m_archiveManager=new OS::ArchiveManager();

	m_fpsCounter = new core::CFPS();


//	m_skinManager=new GUI::GUIThemeManager();
	m_themeManager=new GUI::GUIThemeManager();


	new GUI::GUIOverlayManager();
	new GUI::GUIElementFactory();

	gTimer.initTimer();

	
	m_deviceFactory=new video::DeviceFactory();

	new scene::MeshAnimatorManager();

	new GUI::TextDecorateNodeFactory();


}
Engine::~Engine(){
	traceFunction(Engine);


	delete m_pluginManager;

	delete m_mathTrigTable;

	delete m_deviceFactory;

	delete m_themeManager;

	delete GUI::TextDecorateNodeFactory::getInstancePtr();
	delete scene::MeshAnimatorManager::getInstancePtr();
	delete GUI::GUIOverlayManager::getInstancePtr();
	delete GUI::GUIElementFactory::getInstancePtr();

	delete AsyncLoadManager::getInstancePtr();

	delete scene::SkyBoxManager::getInstancePtr();


	delete m_archiveManager;
	SoundResourceManager::getInstance().removeAll();
	ScriptResourceManager::getInstance().removeAll();
	FileResourceManager::getInstance().removeAll();
	FontResourceManager::getInstance().removeAll();
	ShaderResourceManager::getInstance().removeAll();
	MaterialResourceManager::getInstance().removeAll();
	MeshResourceManager::getInstance().removeAll();
	AnimationResourceManager::getInstance().removeAll();
	ImageSetResourceManager::getInstance().removeAll();
	TextureResourceManager::getInstance().removeAll();
	VideoResourceManager::getInstance().removeAll();

	delete video::TextureSourceFactory::getInstancePtr();

	delete m_resourceGroup;
	delete m_soundManagerFactory;

	delete m_xmlParser;

	delete m_particleSystemFactory;
	delete m_meshManipulator;

	delete m_shaderSemTable;

	delete m_scheduleManager;

	delete m_meshGenerator;

	m_device=0;
	OS::IDllManager::getInstance().ClearLibraries();
	delete m_jobPool;
	
	m_loggerManager->log(mT("Engine shutdown"),ELL_SUCCESS);
	m_loggerManager->close();

	delete scene::SceneHelper::getInstancePtr();
	delete scene::MeshFileCreatorManager::getInstancePtr();

	delete LogSubSystemManager::getInstancePtr();

	delete m_loggerManager;


	delete CommandManager::getInstancePtr();

	delete m_delegateManager;

	delete m_cpuInfo;
	delete m_eventQueue;
	delete m_fpsCounter;


	forceExitFunction();
	delete m_traceManager;

	DefaultEnum::removeDefaultEnum();
	delete EnumManager::getInstancePtr();
	delete DefaultObjects::getInstancePtr();

	delete FunctionProfileManager::getInstancePtr();

	delete m_rootBenchmark;
	m_ioSystem=0;
}

void Engine::updateBenchMark(){
	traceFunction(Engine);
	static ulong lastTime=0;
	ulong t=gTimer.getActualTime();
	lastTime=t;
}

void Engine::updateEngine(){
	traceFunction(Engine);
	float currtime=gTimer.getActualTime();
	core::CFPS::getInstance().regFrame(currtime);
	updateBenchMark();
	video::ShaderSemanticTable::getInstance().setTime(gTimer.getActualTime()*0.001);
	video::ShaderSemanticTable::getInstance().setDTime(gFPS.dt());
	m_scheduleManager->update(gFPS.dt());
}

/*
GCPtr<video::IVideoDevice> Engine::createDevice(video::EDeviceType deviceType,uint window,math::Point2di size,
										   int bits,bool vsync,int multiSample,bool stencilBuffer)
{

	traceFunction(Engine);
	m_device=m_deviceFactory->createDevice(deviceType,window,size,bits,vsync,multiSample,stencilBuffer);
	if(!m_device){
		gLogManager.log(mT("Device Type Unkown!"),ELL_WARNING);
		return 0;
	}

	m_textureResManager->setDevice(m_device);
	m_fontResManager->setDevice(m_device);
	m_shaderResManager->setDevice(m_device);

	if(m_device->getCapabilities()->isFeatureSupported(video::EDF_VertexProgram))
	{
		scene::MeshAnimatorManager::getInstance().SetGPUBasedAnimators(true);
	}else
		scene::MeshAnimatorManager::getInstance().SetGPUBasedAnimators(false);
	return m_device;

}*/
GCPtr<video::IVideoDevice> Engine::createDevice(const core::string& type)
{
	traceFunction(Engine);
	if(m_device)
		gLogManager.log(mT("Device already created"),ELL_ERROR);

	m_device=m_deviceFactory->createDevice(type);
	if(!m_device){
		gLogManager.log(mT("Device Type Unkown!"),ELL_WARNING);
		return 0;
	}

	TextureResourceManager::getInstance().setDevice(m_device);
	FontResourceManager::getInstance().setDevice(m_device);
	ShaderResourceManager::getInstance().setDevice(m_device);

	
	if(m_device->getCapabilities()->isFeatureSupported(video::EDF_VertexProgram))
	{
		scene::MeshAnimatorManager::getInstance().SetGPUBasedAnimators(true);
	}else
		scene::MeshAnimatorManager::getInstance().SetGPUBasedAnimators(false);

	gLogManager.log(mT("Device Created: ")+ type,ELL_SUCCESS);
	return m_device;

}

void Engine::loadPlugins(const core::string &pluginsConfigFile){
	traceFunction(Engine);
	gLogManager.startSection(mT("Loading Plugins"));
	OS::IStreamPtr stream=gFileSystem.createTextFileReader(pluginsConfigFile.c_str());
	GCPtr<script::CSettingsFile> resources=new script::CSettingsFile(stream);
	script::SConfigTag*tag= resources->getTag(mT("Plugins"));
	if(tag){
		script::SConfigTag::AttributesIT it= tag->m_attrSet.begin();
		for(;it!=tag->m_attrSet.end();++it){
			PluginManager::getInstance().addPlugin(it->second);
		}
	}
	gLogManager.endSection(1);
}

void Engine::loadResourceFile(const core::string &resFile){

	traceFunction(Application);
	OS::IStreamPtr stream=gFileSystem.createTextFileReader(resFile);
	GCPtr<script::CSettingsFile> resources=new script::CSettingsFile(stream);
	script::SConfigTag*tag= resources->getTag(mT("SearchPath"));
	if(tag){
		script::SConfigTag::AttributesIT it= tag->m_attrSet.begin();
		for(;it!=tag->m_attrSet.end();++it){
			gFileSystem.addPath(it->second);
		}
	}
	tag= resources->getTag(mT("Archive"));
	if(tag){
		script::SConfigTag::AttributesIT it= tag->m_attrSet.begin();
		for(;it!=tag->m_attrSet.end();++it){
			OS::ArchiveManager::getInstance().addArchive(it->second);
		}
	}
	tag= resources->getTag(mT("Materials"));
	if(tag){
		script::SConfigTag::AttributesIT it= tag->m_attrSet.begin();
		for(;it!=tag->m_attrSet.end();++it){
			gMaterialResourceManager.parseMaterialScript(gFileSystem.createBinaryFileReader(it->second));
		}
	}
}


/*
void Engine::debugBreak(){
#ifdef _DEBUG
	_asm{
		int 3
	}
#endif
}*/

}

