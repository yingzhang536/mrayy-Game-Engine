

/********************************************************************
	created:	2014/02/22
	created:	22:2:2014   14:02
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\GstFileVideoSource.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	GstFileVideoSource
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GstFileVideoSource__
#define __GstFileVideoSource__

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

class GstFileVideoSource :public ICameraVideoSource
{
protected:
	GStreamVideoProvider* m_providers;
	video::ITexturePtr m_videoTex;
	core::string m_left,m_right;
public:
	GstFileVideoSource(const core::string&left,const core::string&right);
	virtual~GstFileVideoSource();

	void SetVideo(const core::string& left, const core::string& right){ m_left = left; m_right = right; }

	void Init();
	void Open();
	void Close();
	bool Blit();

	virtual math::vector2d GetEyeScalingFactor(int i){ return math::vector2d(1, 1); }
	virtual math::vector2d GetEyeResolution(int i);
	virtual video::ITexturePtr GetEyeTexture(int i);
	virtual math::rectf GetEyeTexCoords(int i) { return math::rectf(i*0.5, 0, 0.5 + i*0.5, 1); }

	virtual void LoadFromXML(xml::XMLElement* e);

};

}
}


#endif
