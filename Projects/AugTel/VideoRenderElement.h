

#ifndef VideoRenderElement_h__
#define VideoRenderElement_h__


#include "VideoGrabberTexture.h"


namespace mray
{
namespace AugTel
{
	
class VideoRenderElement
{
protected:
	core::string m_fileName;
	video::VideoGrabberTexture* m_video;
	video::ITexturePtr m_texture;
	bool m_visible;
public:
	VideoRenderElement();
	virtual ~VideoRenderElement();

	void LoadVideo(const core::string&path);

	virtual void Start();
	void Stop();
	virtual bool IsDone();
	virtual void Render(const math::rectf& rc,const video::SColor& clr);
	virtual void Update(float dt);
	float GetLength();
	float GetTime();

	void SetVisible(bool v)
	{
		m_visible = v;
	}

	bool IsVisible()
	{
		return m_visible;
	}
};

}
}

#endif // VideoRenderElement_h__
