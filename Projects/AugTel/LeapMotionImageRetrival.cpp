
#include "stdafx.h"
#include "LeapMotionImageRetrival.h"
#include "TextureRTWrap.h"


namespace mray
{

LeapMotionImageRetrival::LeapMotionImageRetrival(int index) :m_index(index), undistortShader(0)
{

	distortionXTex = gEngine.getDevice()->createEmptyTexture2D(false);
	distortionXTex->setMipmapsFilter(false);
	distortionYTex = gEngine.getDevice()->createEmptyTexture2D(false);
	distortionYTex->setMipmapsFilter(false);
	mainTexture = gEngine.getDevice()->createEmptyTexture2D(false);
	mainTexture->setMipmapsFilter(false);
	mainTexture->setAnisotropicFilter(true);
	mainTexture->setBilinearFilter(true);
	//_SetMainTex(DEFAULT_TEXTURE_WIDTH, DEFAULT_TEXTURE_HEIGHT);
	//_SetDistortionTex(DEFAULT_DISTORTION_WIDTH, DEFAULT_DISTORTION_HEIGHT);

	mainTU.SetTexture(mainTexture);
	disXTU.SetTexture(distortionXTex);
	disYTU.SetTexture(distortionYTex);

	mainTU.setTextureClamp(video::ETW_WrapS, video::ETC_CLAMP);
	mainTU.setTextureClamp(video::ETW_WrapT, video::ETC_CLAMP);

	disXTU.setTextureClamp(video::ETW_WrapS, video::ETC_CLAMP);
	disXTU.setTextureClamp(video::ETW_WrapT, video::ETC_CLAMP);

	disYTU.setTextureClamp(video::ETW_WrapS, video::ETC_CLAMP);
	disYTU.setTextureClamp(video::ETW_WrapT, video::ETC_CLAMP);

	video::ParsedShaderPP* pp = new video::ParsedShaderPP(gEngine.getDevice());
	pp->LoadXML(gFileSystem.openFile("leapUndistort.peff"));
	undistortShader = pp;
}
LeapMotionImageRetrival::~LeapMotionImageRetrival()
{
	delete undistortShader;
}



void LeapMotionImageRetrival::_SetMainTex(int width, int height)
{
	mainTexture->createTexture(math::vector3di(width, height, 1), video::EPixel_Alpha8);

}

void LeapMotionImageRetrival::_SetDistortionTex(int width, int height)
{
	distortionXTex->createTexture(math::vector3di(width, height, 1), video::EPixel_R8G8B8A8);

	distortionYTex->createTexture(math::vector3di(width, height, 1), video::EPixel_R8G8B8A8);
}

void  LeapMotionImageRetrival::_LoadMainTexture(const uchar* data)
{
	video::IHardwarePixelBuffer* buff = mainTexture->getSurface(0);

	video::LockedPixelBox bb(buff->getWidth(), buff->getHeight(), 1, video::EPixel_Alpha8, (uchar*)data);
	buff->blitFromMemory(bb);

}

void LeapMotionImageRetrival::_EncodeDisortion(Leap::Image &image)
{
	int numVals = image.distortionWidth()*image.distortionHeight();
	const float* dist = image.distortion();

	video::IHardwarePixelBuffer* dx = distortionXTex->getSurface(0);
	video::IHardwarePixelBuffer* dy = distortionYTex->getSurface(0);

	video::LockedPixelBox dxBB = dx->lock(math::box3d(0, 0, 0, dx->getWidth(), dx->getHeight(), 1), video::IHardwarePixelBuffer::ELO_Discard);
	video::LockedPixelBox dyBB = dy->lock(math::box3d(0, 0, 0, dy->getWidth(), dy->getHeight(), 1), video::IHardwarePixelBuffer::ELO_Discard);
	struct rgba
	{
		uchar v[4];
	};

	rgba* ptrX = (rgba*)dxBB.data;
	rgba* ptrY = (rgba*)dyBB.data;


	for (int i = 0; i < numVals; ++i)
	{
		float v = dist[i];

		// The distortion range is -0.6 to +1.7. Normalize to range [0..1).
		v = (v + 0.6f) / 2.3f;

		float encX = v;
		float encY = v * 255.0f;
		float encZ = v * 65025.0f;
		float encW = v * 160581375.0f;

		encX = encX - floor(encX);
		encY = encY - floor(encY);
		encZ = encZ - floor(encZ);
		encW = encW - floor(encW);

		encX -= math::i255*encY;
		encY -= math::i255*encZ;
		encZ -= math::i255*encW;

		int index = i >> 1;
		rgba* p = 0;
		if (i % 2 == 0)
			p = &ptrX[index];
		else
			p = &ptrY[index];

		p->v[0] = (uchar)(encX * 256);
		p->v[1] = (uchar)(encY * 256);
		p->v[2] = (uchar)(encZ * 256);
		p->v[3] = (uchar)(encW * 256);
	}

	dx->unlock();
	dy->unlock();

}
bool LeapMotionImageRetrival::Capture(Leap::Frame &frame)
{

	if (frame.images().isEmpty())
		return false;

	Leap::Image image = frame.images()[m_index];
	int w = image.width();
	int h = image.height();
	if (w == 0 || h == 0)
		return false;

	_SetMainTex(w, h);

	int dw = image.distortionWidth() / 2;
	int dh = image.distortionHeight();

	if (dw == 0 || dh == 0)
		return false;

	_SetDistortionTex(dw, dh);

	_LoadMainTexture(image.data());
	_EncodeDisortion(image);

	math::vector4d ray(image.rayOffsetX(), image.rayOffsetY(), image.rayScaleX(), image.rayScaleY());
	undistortShader->GetParam("Ray")->SetValue(ray);

	gEngine.getDevice()->useTexture(0, &mainTU);
	gEngine.getDevice()->useTexture(1, &disXTU);
	gEngine.getDevice()->useTexture(2, &disYTU);
	undistortShader->Setup(math::rectf(0, 0, mainTexture->getSize().x, mainTexture->getSize().y));
	undistortShader->render(&video::TextureRTWrap(mainTexture));
	finalTexture = undistortShader->getOutput()->GetColorTexture();
	//finalTexture = mainTexture;

	gEngine.getDevice()->useTexture(0, 0);
	gEngine.getDevice()->useTexture(1, 0);
	gEngine.getDevice()->useTexture(2, 0);

	return true;
}


}



