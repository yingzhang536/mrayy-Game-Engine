


/********************************************************************
	created:	2012/10/29
	created:	29:10:2012   18:37
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\TBAppGlobals.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	TBAppGlobals
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___TBAppGlobals___
#define ___TBAppGlobals___


#include "StereoRenderer.h"

namespace mray
{
	class InputManager;
	namespace db
	{
		class ISQLManager;
	}
	namespace video
	{
		class OculusDevice;
	}
namespace TBee
{

	class Application;
	class MapObject;
	class RobotInfoManager;
	class TBRobotInfo;
	class SeeThroughWindow;
	class TBeeOptiTrackObject;


	enum class EController
	{
		XBox,
		Logicool
	};
class TBAppGlobals
{
protected:

	static script::CSettingsFile s_values;
public:

	static SeeThroughWindow* seeTrough;
	static Application* App;
	static bool IsDebugging;
	static core::string ServerIP;
	static TBeeOptiTrackObject* optiObj;

	static EController m_controller;

	static int MajorVer;
	static int MinorVer;

	static core::string GetVersion();
	static core::string GetBuild();

	static scene::EStereoMode StereoMode;

	static math::vector2d GetStereoScaleRatio();

	static core::string DVIPort;

	static MapObject* mapObj;

	static TBRobotInfo* selectedRobot;

	static RobotInfoManager* robotInfoManager;

	static core::string userName;
	static core::string userPassword;

	static bool usingOculus;

	static db::ISQLManager* sqlManager;

	static video::OculusDevice* oculusDevice;

	static InputManager* inputMngr;
	
	static void SetValue(const core::string&catagory,const core::string&name,const core::string& v);
	static core::string GetValue(const core::string&catagory,const core::string&name);

	static void ConnectOpti();
	static void DisconnectOpti();

	static void Load(const core::string& path);
	static void Save(const core::string& path);
};

}
}

#endif
