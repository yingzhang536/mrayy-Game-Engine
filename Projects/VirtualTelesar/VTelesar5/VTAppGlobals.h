


/********************************************************************
	created:	2012/10/29
	created:	29:10:2012   18:37
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\VTAppGlobals.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	VTAppGlobals
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___VTAppGlobals___
#define ___VTAppGlobals___



namespace mray
{
	namespace game
	{
		class OculusCameraComponent;
	}
	namespace video
	{
		class OculusDevice;
	}
namespace VT
{
	class Application;

	enum ECameraSourceType
	{
		EWebCamera,
		EFlyCapture
	};
class VTAppGlobals
{
protected:

	static script::CSettingsFile s_values;
public:

	static Application* App;
	static bool IsDebugging;
	static bool Physics;
	static core::string GhostServerIP;
	static int Camera0;
	static int Camera1;

	static video::OculusDevice* oculusDevice;
	static game::OculusCameraComponent* oculusComponents[2];
	
	static bool FlipCameras;
	static ECameraSourceType CameraType;
	
	static void SetValue(const core::string&catagory,const core::string&name,const core::string& v);
	static core::string GetValue(const core::string&catagory,const core::string&name);


	static void Load(const core::string& path);
	static void Save(const core::string& path);
};

}
}

#endif
