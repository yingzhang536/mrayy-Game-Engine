

/********************************************************************
	created:	2014/02/09
	created:	9:2:2014   19:28
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\GstNetVideoSource.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	GstNetVideoSource
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GstNetVideoSource__
#define __GstNetVideoSource__

#include "ICameraVideoSource.h"

namespace mray
{
	namespace video
	{
		class ICameraVideoGrabber;
	}
namespace TBee
{
	class GStreamVideoProvider;

class GstNetVideoSource:public ICameraVideoSource
{
protected:
	GStreamVideoProvider* m_providers;
	video::ITexturePtr m_remoteTex;
	core::string m_ip;
public:
	GstNetVideoSource(const core::string& ip = "127.0.0.1");
	virtual~GstNetVideoSource();

	void SetIP(const core::string& ip){ m_ip = ip; }

	void Init();
	void Open();
	void Close();
	bool Blit();

	virtual math::vector2d GetEyeScalingFactor(int i){ return math::vector2d(0.5, 1); }
	virtual math::vector2d GetEyeResolution(int i);
	virtual video::ITexturePtr GetEyeTexture(int i);
	virtual math::rectf GetEyeTexCoords(int i) { return math::rectf(i*0.5, 0, 0.5 + i*0.5, 1); }

};

}
}


#endif
