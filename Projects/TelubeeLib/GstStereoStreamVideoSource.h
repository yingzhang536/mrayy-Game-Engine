

/********************************************************************
	created:	2014/02/28
	created:	28:2:2014   3:02
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\GstStereoStreamVideoSource.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	GstStereoStreamVideoSource
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GstStereoStreamVideoSource__
#define __GstStereoStreamVideoSource__


#include "ICameraVideoSource.h"


namespace mray
{
namespace TBee
{
	class GstSingleNetVideoSource;

class GstStereoStreamVideoSource :public ICameraVideoSource
{
protected:
	GstSingleNetVideoSource* m_sources[2];
public:
	GstStereoStreamVideoSource(const core::string& ip1 = "127.0.0.1", int port1 = 5000, const core::string& ip2 = "127.0.0.1", int port2 = 5001);
	virtual~GstStereoStreamVideoSource();

	void SetIPs(const core::string& ip1, const core::string& ip2);
	void SetPorts(int port1, int port2);

	void Init();
	void Open();
	void Close();
	bool Blit();

	virtual math::vector2d GetEyeScalingFactor(int i){ return math::vector2d(1, 1); }
	virtual math::vector2d GetEyeResolution(int i);
	virtual video::ITexturePtr GetEyeTexture(int i);
	virtual math::rectf GetEyeTexCoords(int i) { return math::rectf(0, 0, 1, 1); }
};

}
}


#endif
