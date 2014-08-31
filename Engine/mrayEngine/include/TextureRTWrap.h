

#ifndef TextureRTWrap_h__
#define TextureRTWrap_h__

#include "IRenderArea.h"

namespace mray
{
namespace video
{

class TextureRTWrap :public video::IRenderArea
{
protected:
	video::ITexturePtr m_Tex;
public:


	TextureRTWrap(video::ITexture* t)
	{
		m_Tex = t;
	}

	void SetTexture(video::ITexturePtr tex){ m_Tex=tex; }
	video::ITexturePtr GetTexture(){ return m_Tex; }


	virtual math::vector2di GetSize()
	{
		math::vector3di v = m_Tex->getSize();
		return math::vector2d(v.x, v.y);
	}

	virtual const video::ITexturePtr& GetColorTexture(int i = 0)
	{
		return m_Tex;
	}

	virtual void GetParameter(const core::string& name, void* param){}

	virtual int GetColorTextureCount() {
		return 1;
	}

	virtual void Resize(int x, int y) {}
};

}
}

#endif // TextureRTWrap_h__
