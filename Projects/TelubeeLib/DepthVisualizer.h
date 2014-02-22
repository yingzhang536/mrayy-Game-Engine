

/********************************************************************
	created:	2014/02/17
	created:	17:2:2014   2:59
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\DepthVisualizer.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	DepthVisualizer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __DepthVisualizer__
#define __DepthVisualizer__




namespace mray
{
namespace TBee
{
	class DepthFrame;
class DepthVisualizer
{
protected:
	DepthFrame* m_frame;

	video::ITexturePtr m_tex;
	video::ImageInfo m_depthData;

	bool m_viewDepth ;
public:
	DepthVisualizer();
	virtual~DepthVisualizer();

	void Init();

	void SetDepthFrame(DepthFrame* f){ m_frame = f; }
	DepthFrame* GetDepthFrame(){ return m_frame; }

	void SetViewDepth(bool v){ m_viewDepth = v; }
	bool IsViewDepth(){ return m_viewDepth; }

	void Update();

	video::ITexturePtr GetTexture()
	{
		return m_tex;
	}
};

}
}


#endif
