

/********************************************************************
	created:	2014/01/19
	created:	19:1:2014   2:20
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\TBeeCommon.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	TBeeCommon
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __TBeeCommon__
#define __TBeeCommon__




namespace mray
{
	namespace TBee
	{

		enum class EHeadControllerType
		{
			None,
			Keyboard,
			Oculus,
			OptiTrack
		};

		enum class ERobotControllerType
		{
			None,
			Keyboard,
			Joystick,
			Wiiboard
		};
		enum class ERenderStereoMode
		{
			None,
			Oculus,
			SideBySide,
			TopDown,
			NVidia,
			StereoTV
		};
		enum ETargetEye
		{
			Eye_Left,
			Eye_Right
		};

	}
}


#endif
