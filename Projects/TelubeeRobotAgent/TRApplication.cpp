

#include "stdafx.h"
#include "TRApplication.h"

#include "FontResourceManager.h"
#include "ImageSetResourceManager.h"
#include "GUIThemeManager.h"
#include "StringUtil.h"

#include "win32NetInterface.h"

#include <ViewPort.h>

#include "DirectShowVideoGrabber.h"

#include <windows.h>

namespace mray
{


	class CombineVideoGrabber :public video::IVideoGrabber
	{
	protected:
		video::IVideoGrabber* m_g1;
		video::IVideoGrabber* m_g2;

		math::vector2di m_targetSize;
		bool m_newFrame;
		video::ImageInfo m_lastImage;
		video::ImageInfo m_frame1;
		video::ImageInfo m_frame2;

	public:

		CombineVideoGrabber()
		{
			m_g1 = 0;
			m_g2 = 0;
		}

		void SetGrabbers(video::IVideoGrabber* g1, video::IVideoGrabber* g2)
		{
			m_g1 = g1;
			m_g2 = g2;
		}

		virtual void SetFrameSize(int w, int h)
		{
			m_targetSize.set(w, h);
			m_lastImage.createData(math::vector3di(w, h, 1), GetImageFormat());

		}
		virtual const math::vector2di& GetFrameSize()
		{
			return m_targetSize;
		}

		virtual void SetImageFormat(video::EPixelFormat fmt)
		{
		}
		virtual video::EPixelFormat GetImageFormat()
		{
			return video::EPixel_R8G8B8;
		}

		virtual bool GrabFrame()
		{
			m_newFrame = false;
			bool ret = false;
			bool a = false, b = false;

			if (m_g1 && m_g1->GrabFrame())
				a = true;
			if (m_g2 && m_g2->GrabFrame())
				b = true;
			ret = a | b;
			if (!ret)
				return false;
			m_newFrame = true;
			if (a)
				m_frame1.setData(m_g1->GetLastFrame()->imageData, m_g1->GetLastFrame()->Size, m_g1->GetLastFrame()->format);
			if (b)
				m_frame2.setData(m_g2->GetLastFrame()->imageData, m_g2->GetLastFrame()->Size, m_g2->GetLastFrame()->format);

			if (a && b)
			{
				video::ColorConverter::resizeImage(&m_frame1, math::Point2di(m_targetSize.x / 2, m_targetSize.y));
				video::ColorConverter::resizeImage(&m_frame2, math::Point2di(m_targetSize.x / 2, m_targetSize.y));
				for (int i = 0; i < m_targetSize.x / 2; ++i)
				{
					for (int j = 0; j < m_targetSize.y; ++j)
					{
						int index1 = (j*m_targetSize.x + i) * 3;//for frame1
						int index2 = (j*m_targetSize.x + i*2) * 3;//for frame1
						int index  = (j*m_targetSize.x / 2 + i) * 3;//for frame
						m_lastImage.imageData[index1 + 0] = m_frame1.imageData[index + 0];
						m_lastImage.imageData[index1 + 1] = m_frame1.imageData[index + 1];
						m_lastImage.imageData[index1 + 2] = m_frame1.imageData[index + 2];

						m_lastImage.imageData[index2 + 0] = m_frame2.imageData[index + 0];
						m_lastImage.imageData[index2 + 1] = m_frame2.imageData[index + 1];
						m_lastImage.imageData[index2 + 2] = m_frame2.imageData[index + 2];
					}
				}
			}else if (a)
			{
				video::ColorConverter::resizeImage(&m_frame1, math::Point2di(m_targetSize.x, m_targetSize.y));
				m_lastImage.setData(m_frame1.imageData, m_frame1.Size, m_frame1.format);
			}else if ( b)
			{
				video::ColorConverter::resizeImage(&m_frame2, math::Point2di(m_targetSize.x, m_targetSize.y));
				m_lastImage.setData(m_frame2.imageData, m_frame2.Size, m_frame2.format);
			}
			return true;
		}
		virtual bool HasNewFrame()
		{
			return m_newFrame;
		}


		virtual const video::ImageInfo* GetLastFrame()
		{
			return &m_lastImage;
		}
	};

TRApplication::TRApplication()
{
}

TRApplication::~TRApplication()
{
}



void TRApplication::_InitResources()
{
	CMRayApplication::loadResourceFile(mT("Resources.stg"));


	gImageSetResourceManager.loadImageSet(mT("VistaCG_Dark.imageset"));
	GCPtr<OS::IStream> themeStream = gFileSystem.createBinaryFileReader(mT("VistaCG_Dark.xml"));
	GUI::GUIThemeManager::getInstance().loadTheme(themeStream);
	GUI::GUIThemeManager::getInstance().setActiveTheme(mT("VistaCG_Dark"));

	//load font
	GCPtr<GUI::IFont>font = gFontResourceManager.loadFont(mT("Calibrib_font.fnt"));
	gFontResourceManager.loadFont(mT("OCRAStd.fnt"));
	gFontResourceManager.setDefaultFont(font);

	gLogManager.log("Resources Loaded", ELL_SUCCESS);
}


void TRApplication::onEvent(Event* event)
{
	CMRayApplication::onEvent(event);
}


void TRApplication::init(const OptionContainer &extraOptions)
{
	CMRayApplication::init(extraOptions);

	_InitResources();

	m_limitFps = true;
	network::createWin32Network();

	m_guiRender = new GUI::GUIBatchRenderer();
	m_guiRender->SetDevice(getDevice());


	m_viewPort = GetRenderWindow()->CreateViewport("MainVP", 0, 0, math::rectf(0, 0, 1, 1), 0);


	int cameraIndex[2] = { 0, 1 };

	//create cameras
	for (int i = 0; i < 2; ++i)
	{
		m_cameras[i] = new video::DirectShowVideoGrabber();
		m_cameras[i]->SetDevice(cameraIndex[i]);
		m_cameras[i]->SetFrameSize(1280, 800);
		m_cameras[i]->Start();

		m_cameraTextures[i].Set(m_cameras[i], getDevice()->createEmptyTexture2D(true));

	}

	m_combinedCameras = new CombineVideoGrabber();
	m_combinedCameras->SetFrameSize(1280, 800);
	((CombineVideoGrabber*)m_combinedCameras.pointer())->SetGrabbers(m_cameras[0], m_cameras[1]);
	m_cameraTextures[2].Set(m_combinedCameras, getDevice()->createEmptyTexture2D(true));

	m_videoProvider = new GstVideoProvider();
	m_videoProvider->SetDataSource(m_combinedCameras);
	m_videoProvider->StreamDataTo(network::NetAddress("127.0.0.1", 5000));
	m_videoProvider->Start();
}


void TRApplication::draw(scene::ViewPort* vp)
{
	CMRayApplication::draw(vp);



	video::TextureUnit tex;
	tex.SetTexture(m_cameraTextures[2].GetTexture());
	getDevice()->useTexture(0, &tex);
	getDevice()->draw2DImage(vp->getAbsViewPort(), 1);

	GCPtr<GUI::IFont> font = gFontResourceManager.getDefaultFont();
	if (font){
		m_guiRender->Prepare();

		float yoffset = 50;


		GUI::FontAttributes attr;
		attr.fontColor.Set(0.05, 1, 0.5, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 24;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;
		core::string msg = mT("FPS= ");
		msg += core::StringConverter::toString((int)core::CFPS::getInstance().getFPS());
		font->print(math::rectf(vp->getAbsRenderingViewPort().getWidth() - 250, vp->getAbsRenderingViewPort().getHeight() - 50, 10, 10), &attr, 0, msg, m_guiRender);
		yoffset += attr.fontSize;

	}

	m_guiRender->Flush();
	getDevice()->useShader(0);
}

void TRApplication::WindowPostRender(video::RenderWindow* wnd)
{
}

void TRApplication::update(float dt)
{
	CMRayApplication::update(dt);
	//m_cameraTextures[2].Blit();
	Sleep(30);
}

void TRApplication::onDone()
{
	CMRayApplication::onDone();
}


void TRApplication::onRenderDone(scene::ViewPort*vp)
{
}


}