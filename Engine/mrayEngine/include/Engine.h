
#ifndef ___Engine___
#define ___Engine___



#include "GCPtr.h"
#include "IOSystem.h"
#include "IVideoDevice.h"


namespace mray
{
	namespace core
	{
		class CFPS;
	}
	namespace video
	{
		class DeviceFactory;
		class ShaderSemanticTable;
	}
	namespace scene
	{
		class SParticleSystemFactory;
		class MeshGenerator;
		class SMeshManipulator;
	}
	namespace GUI
	{
		class GUIThemeManager;
	}
	namespace xml
	{
		class IXMLParser;
	}
	namespace sound
	{
		class SoundManagerFactory;
	}
	namespace OS
	{
		class ArchiveManager;
	}

	class BenchmarkItem;
/*
	class TextureResourceManager;
	class SoundResourceManager;
	class MeshResourceManager;
	class FontResourceManager;
	class ShaderResourceManager;
	class AnimationResourceManager;
	class MaterialResourceManager;
	class ImageSetResourceManager;*/

	class ResourceGroup;
	class ScheduleManager;
	class TraceManager;
	class IOSRegistry;
	class ShaderSemanticTable;
	class IDllManager;
	class ILogManager;
	class PluginManager;
	class IErrorDialog;
	class ITimer;
	class EventQueue;
	class CPUInfo;
	class JobPool;
	class DelegateManager;


class MRAY_DLL Engine:public ISingleton<Engine>
{
protected:
	JobPool* m_jobPool;

	GCPtr<OS::IOSystem> m_ioSystem;
	GCPtr<video::IVideoDevice> m_device;
	OS::ArchiveManager* m_archiveManager;

	scene::MeshGenerator* m_meshGenerator;
//	GCPtr<GUI::GUIThemeManager> m_skinManager;
	GUI::GUIThemeManager* m_themeManager;

	ILogManager* m_loggerManager;
	PluginManager* m_pluginManager;

	sound::SoundManagerFactory* m_soundManagerFactory;
	/*
	TextureResourceManager* m_textureResManager;
	SoundResourceManager* m_soundResManager;
	MaterialResourceManager* m_mtrlResManager;
	MeshResourceManager* m_meshResManager;
	FontResourceManager* m_fontResManager;
	ImageSetResourceManager* m_imagesetResManager;
	ShaderResourceManager* m_shaderResManager;
	AnimationResourceManager* m_animResManager;*/
	ResourceGroup* m_resourceGroup;

	DelegateManager* m_delegateManager;

	scene::SMeshManipulator* m_meshManipulator;
	scene::SParticleSystemFactory* m_particleSystemFactory;

	xml::IXMLParser* m_xmlParser;

	EventQueue* m_eventQueue;

	TraceManager* m_traceManager;
	
	math::MTrigTable* m_mathTrigTable;

	core::CFPS* m_fpsCounter;

	video::DeviceFactory* m_deviceFactory;

	video::ShaderSemanticTable* m_shaderSemTable;

	CPUInfo* m_cpuInfo;

	ScheduleManager* m_scheduleManager;

	BenchmarkItem *m_rootBenchmark;

	void updateBenchMark();
public:

	Engine(GCPtr<OS::IOSystem> system);
	virtual~Engine();



	void updateEngine();


	//GCPtr<video::IVideoDevice> createDevice(video::EDeviceType deviceType,uint window,math::Point2di size,int bits,
	//	bool vsync=false,int multiSample=0,bool stencilBuffer=false);

	GCPtr<video::IVideoDevice> createDevice(const core::string& type);

	GCPtr<video::IVideoDevice> getDevice(){return m_device;}



	void loadPlugins(const core::string &pluginsConfigFile);
	void loadResourceFile(const core::string &resFile);

	BenchmarkItem* getRootBenchmarking(){return m_rootBenchmark;}

};

}







#endif


