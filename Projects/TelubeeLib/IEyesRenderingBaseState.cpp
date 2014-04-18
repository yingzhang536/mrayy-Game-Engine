

#include "stdafx.h"
#include "IEyesRenderingBaseState.h"

#include "RemoteRobotCommunicator.h"
#include "AppData.h"
#include "OculusDevice.h"
#include "InputManager.h"
#include "FontResourceManager.h"
#include "JoystickDefinitions.h"
#include "CRobotConnector.h"

#include "GUIBatchRenderer.h"
#include "SMeshBuffer.h"
#include "RenderPass.h"

#include "OculusHeadController.h"
#include "OptiTrackHeadController.h"
#include "ICameraVideoSource.h"

#include "WiiboardInputController.h"
#include "JoystickInputController.h"
#include "KeyboardHeadController.h"

namespace mray
{
namespace TBee
{

IEyesRenderingBaseState::IEyesRenderingBaseState()
{
	m_exitCode = 0;

	m_cameraFov = 25;
	m_videoSource = 0;

	m_hmdDistance = 0;

	m_robotConnector = new CRobotConnector();

	m_lensCorrectionPP = new video::ParsedShaderPP(Engine::getInstance().getDevice());
	m_lensCorrectionPP->LoadXML(gFileSystem.openFile("LensCorrection.peff"));

	m_correctionValue[0] = m_lensCorrectionPP->GetValue("final.HMDWrapParams1");
	m_correctionValue[1] = m_lensCorrectionPP->GetValue("final.HMDWrapParams2");

	switch (AppData::Instance()->headController)
	{
	case EHeadControllerType::Keyboard:
		m_robotConnector->SetHeadController(new KeyboardHeadController);
		break;;
	case EHeadControllerType::Oculus:
		m_robotConnector->SetHeadController(new OculusHeadController);
		break;;
	case EHeadControllerType::OptiTrack:
		m_robotConnector->SetHeadController(new OptiTrackHeadController(1));
		break;;
	default:
		break;
	}

	switch (AppData::Instance()->robotController)
	{
	case ERobotControllerType::Keyboard:
		break;;
	case ERobotControllerType::Joystick:
		m_robotConnector->SetRobotController(new JoystickInputController);
		break;;
	case ERobotControllerType::Wiiboard:
		m_robotConnector->SetRobotController(new WiiboardInputController);
		break;;
	default:
		break;
	}

	GUI::GUIBatchRenderer*r = new GUI::GUIBatchRenderer();
	r->SetDevice(Engine::getInstance().getDevice());
	m_guiRenderer = r;

	m_panningScale = 1;
	m_headPan = 0;
	m_headTilt = 0;

	m_useLensCorrection = false;
	m_correctionParamsU.set(1, 0, 0, 0);
	m_correctionParamsV.set(1, 0, 0, 0);
	m_enablePanning = false;
}


IEyesRenderingBaseState::~IEyesRenderingBaseState()
{
	delete m_robotConnector;
}

void IEyesRenderingBaseState::_UpdateCameraParams()
{
	if (!m_videoSource)
		return;
	m_targetAspectRatio = m_hmdSize.x / m_hmdSize.y;
	for (int i = 0; i < 2; ++i)
	{
		math::vector2d framesize = m_videoSource->GetEyeResolution(i)*m_videoSource->GetEyeScalingFactor(i);
		if (m_eyes[i].flip90)
			math::Swap(framesize.x, framesize.y);
		if (framesize.y == 0)framesize.y = 1;
		float camRatio = framesize.x / framesize.y;
		m_eyes[i].cropping.set(framesize.x, framesize.x / m_targetAspectRatio);
		m_eyes[i].ratio = camRatio;

		float focal = 1;//in meter
		float w1 = 2 * focal*tan(math::toRad(m_hmdFov*0.5f));
		float w2 = 2 * (focal - m_hmdDistance)*tan(math::toRad(m_cameraFov*0.5f));

		float ratio = w2 / w1;
		m_eyes[i].scale = m_hmdSize*ratio;
	}
}

void IEyesRenderingBaseState::SetParameters(float targetAspectRatio, float hmdDistance, float cameraFov, float hmdFov)
{
	m_targetAspectRatio = targetAspectRatio;
	m_hmdDistance = hmdDistance;
	m_cameraFov = cameraFov;
	m_hmdFov = hmdFov;
	_UpdateCameraParams();
}

void IEyesRenderingBaseState::InitState()
{
	IRenderingState::InitState();

}



bool IEyesRenderingBaseState::OnEvent(Event* e, const math::rectf& rc)
{
	bool ok = false;
	if (e->getType() == ET_Keyboard)
	{
		KeyboardEvent* evt = (KeyboardEvent*)e;
		if (evt->press )
		{
			if (evt->key == KEY_Y)
			{
				m_correctionValue[0]->floatParam[0] += 0.01*(evt->shift ? -1 : 1);
			}
			else if (evt->key == KEY_U)
			{
				m_correctionValue[0]->floatParam[1] += 0.01*(evt->shift ? -1 : 1);
			}
			else if (evt->key == KEY_I)
			{
				m_correctionValue[0]->floatParam[2] += 0.01*(evt->shift ? -1 : 1);
			}
			else if (evt->key == KEY_H)
			{
				m_correctionValue[1]->floatParam[0] += 0.01*(evt->shift ? -1 : 1);
			}
			else if (evt->key == KEY_J)
			{
				m_correctionValue[1]->floatParam[1] += 0.01*(evt->shift ? -1 : 1);
			}
			else if (evt->key == KEY_K)
			{
				m_correctionValue[1]->floatParam[2] += 0.01*(evt->shift ? -1 : 1);
			}
		}
	}
	
	if (e->getType() == ET_Joystick)
	{
		JoystickEvent* evt = (JoystickEvent*)e;
		if (evt->event == JET_BUTTON_PRESSED)
		{
			if (evt->button == JOYSTICK_SelectButton)
			{
				m_exitCode = STATE_EXIT_CODE;
				m_robotConnector->EndUpdate();
				ok = true;
			}
			else if (evt->button == JOYSTICK_StartButton)
			{
				if (m_robotConnector->IsRobotConnected())
					m_robotConnector->EndUpdate();
				else
					m_robotConnector->StartUpdate();
				ok = true;
			}
		}
	}
	return ok;
}


void IEyesRenderingBaseState::OnEnter(IRenderingState*prev)
{
	//VCameraType* cam=(VCameraType*)m_camera;//m_video->GetGrabber().pointer();

	_UpdateCameraParams();

	m_robotConnector->ConnectRobot();
}


void IEyesRenderingBaseState::OnExit()
{
	IRenderingState::OnExit();
	//VCameraType* cam=(VCameraType*)m_video->GetGrabber().pointer();
	m_robotConnector->DisconnectRobot();

}

class TextureRenderTarget :public video::IRenderTarget
{
protected:
	video::ITexturePtr m_tex;
public:
	TextureRenderTarget(video::ITexturePtr tex){ m_tex = tex; }
	virtual~TextureRenderTarget()
	{
	}

	virtual void clear(const video::SColor&c, bool clearBackbuffer, bool clearDepthBuffer)
	{
	}

	virtual void bind() {}
	virtual void unbind() {}

	virtual void attachRenderTarget(const video::ITexturePtr& tex, uint index = 0) {}
	virtual void deattachRenderTarget(const video::ITexturePtr& tex, uint index = 0) {}

	virtual const video::ITexturePtr& getColorTexture(int i = 0) { return m_tex; }
	virtual const video::IHardwarePixelBufferPtr& getDepthBuffer() { return video::IHardwarePixelBufferPtr::Null; }
	virtual const video::IHardwarePixelBufferPtr& getStencilBuffer() { return video::IHardwarePixelBufferPtr::Null; }

	virtual int GetColorTextureCount() { return 1; }
	virtual void Resize(int x, int y) {}
	virtual math::vector2di getSize()
	{
		return math::vector2di(m_tex->getSize().x, m_tex->getSize().y);
	}

};


void IEyesRenderingBaseState::_RenderUI(const math::rectf& rc)
{
	if (!AppData::Instance()->IsDebugging)
		return;
	GUI::IFont* font = gFontResourceManager.getDefaultFont();
	GUI::FontAttributes attr;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	AppData* app = AppData::Instance();
	if (font)
	{
		attr.fontColor.Set(1, 1, 1, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 18;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;

		math::rectf r = rc;

		if (m_robotConnector->GetHeadController())
		{
			math::vector3d head;
			head=m_robotConnector->GetHeadRotation();
			core::string msg = mT("Head Rotation: ")+core::StringConverter::toString(head);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;

			head = m_robotConnector->GetHeadPosition();
			msg = mT("Head Position: ") + core::StringConverter::toString(head);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
		}
		else
		{
			core::string msg = mT("Head: Non") ;
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
		}
		if (m_robotConnector->GetRobotController())
		{

			math::vector2d speed;
			float rot;
			speed = m_robotConnector->GetSpeed();
			rot = m_robotConnector->GetRotation();
			core::string msg = mT("Robot Speed: ") + core::StringConverter::toString(speed);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
			msg = mT("Robot Rotation: ") + core::StringConverter::toString(rot);
			font->print(r, &attr, 0, msg, m_guiRenderer);
			r.ULPoint.y += attr.fontSize + 5;
		}
		/*
		math::vector3d correctionX(m_correctionValue[0]->floatParam[0], m_correctionValue[0]->floatParam[1], m_correctionValue[0]->floatParam[2]);
		math::vector3d correctionY(m_correctionValue[1]->floatParam[0], m_correctionValue[1]->floatParam[1], m_correctionValue[1]->floatParam[2]);
		core::string msg = mT("Correction X:") + core::StringConverter::toString(correctionX);
		font->print(r, &attr, 0, msg, m_guiRenderer);
		r.ULPoint.y += attr.fontSize + 5;
		msg = mT("Correction Y:") + core::StringConverter::toString(correctionY);
		font->print(r, &attr, 0, msg, m_guiRenderer);
		r.ULPoint.y += attr.fontSize + 5;*/
		m_guiRenderer->Flush();
	}

}
video::IRenderTarget* IEyesRenderingBaseState::Render(const math::rectf& rc, ETargetEye eye)
{
	IRenderingState::Render(rc, eye);
	int index = GetEyeIndex(eye);
	if (!m_videoSource)
		return m_renderTarget[index].pointer();


	video::IVideoDevice* dev = Engine::getInstance().getDevice();

	video::TextureUnit tex;
	tex.SetEdgeColor(video::DefaultColors::Black);
	tex.setTextureClamp(video::ETW_WrapS, video::ETC_CLAMP_TO_BORDER);
	tex.setTextureClamp(video::ETW_WrapT, video::ETC_CLAMP_TO_BORDER);

	video::ITexturePtr cameraTex = m_videoSource->GetEyeTexture(index);

	if (m_useLensCorrection)
	{
		math::vector2d size(cameraTex->getSize().x, cameraTex->getSize().y);
		video::ParsedShaderPP::MappedParams* UParams = m_lensCorrectionPP->GetParam("WrapParamsU");
		video::ParsedShaderPP::MappedParams* VParams = m_lensCorrectionPP->GetParam("WrapParamsV");

		if (UParams)
			UParams->SetValue(m_correctionParamsU);
		if (VParams)
			VParams->SetValue(m_correctionParamsV);

		m_lensCorrectionPP->Setup(math::rectf(0, size));
		m_lensCorrectionPP->render(&TextureRenderTarget(cameraTex));
		cameraTex = m_lensCorrectionPP->getOutput()->getColorTexture();
	}
	dev->setRenderTarget(m_renderTarget[index],1,1,video::DefaultColors::Black);
	dev->set2DMode();

	//	gTextureResourceManager.writeResourceToDist(m_video->GetTexture(),"screens\\image#"+core::StringConverter::toString(s_id++)+".jp2");
	if (rc.getSize() != m_hmdSize)
	{
		m_hmdSize = rc.getSize();
		_UpdateCameraParams();
	}
	tex.SetTexture(cameraTex);
	//float croppingHeight=1-m_targetAspectRatio/m_cameraSource[index].ratio;
	math::vector2d frameRes = m_videoSource->GetEyeResolution(index)*m_videoSource->GetEyeScalingFactor(index);
	if (m_eyes[index].flip90)
		math::Swap(frameRes.x, frameRes.y);
	if (frameRes.y == 0)
		frameRes.y = 1;
	float targetHeight = frameRes.x / m_targetAspectRatio;
	float heightDiff = (targetHeight - frameRes.y);
	//float croppingHeight = 1-1.0f/(targetHeight / rc.getHeight());
	float croppingHeight = (heightDiff / frameRes.y);
	croppingHeight *= 0.5f;
	math::rectf tc;
	tc=math::rectf(0, -croppingHeight, 1, 1 + croppingHeight);
	math::rectf eyeTc = m_videoSource->GetEyeTexCoords(index);
	//now calculate the actual rendering rectangle
	math::rectf targetRect;
	math::vector2d margin = (rc.getSize() - m_eyes[index].scale) / 2;
	targetRect.ULPoint = rc.ULPoint + margin;
	targetRect.BRPoint = rc.BRPoint - margin;
	math::vector2d tcSz = tc.getSize();
	tc.ULPoint += eyeTc.ULPoint;
	tc.BRPoint = tc.ULPoint + tcSz*eyeTc.getSize();


	//tc.ULPoint=targetRect.ULPoint/rc.getSize();
	//tc.BRPoint=targetRect.BRPoint/rc.getSize();
	
	tc.ULPoint.y = 1 - tc.ULPoint.y;
	tc.BRPoint.y = 1 - tc.BRPoint.y;
	//tc = math::rectf(0, 0, 1, 1);
	if (m_enablePanning)
	{
		math::vector2d center = tc.getCenter();
		math::vector2d halfSz = tc.getSize()*0.5f;
		if (m_panningScale > 1)
		{
			halfSz = halfSz / m_panningScale;
			tc.ULPoint = center - halfSz;
			tc.BRPoint = center + halfSz;
		}
		halfSz.x = fabs(halfSz.x);
		halfSz.y = fabs(halfSz.y);
		math::vector2d panMargin = (eyeTc.getSize()*0.5 - halfSz );
		panMargin.x = math::Max<float>(panMargin.x, 0);
		panMargin.y = math::Max<float>(panMargin.y, 0);
		math::vector2d panning(panMargin.x*m_headPan * 2, panMargin.y*m_headTilt * 2);
		tc.ULPoint += panning;
		tc.BRPoint += panning;
	}

	dev->useTexture(0, &tex);

	//dev->draw2DImage(math::rectf(targetRect.ULPoint, targetRect.BRPoint), 1, 0, &tc);

	math::vector2d points[4]=
	{
		math::vector2d(targetRect.ULPoint.x, targetRect.ULPoint.y),
		math::vector2d(targetRect.BRPoint.x, targetRect.ULPoint.y),
		math::vector2d(targetRect.BRPoint.x, targetRect.BRPoint.y),
		math::vector2d(targetRect.ULPoint.x, targetRect.BRPoint.y)
	};

	math::vector2d coords[4] =
	{
		math::vector2d(tc.ULPoint.x, tc.ULPoint.y),
		math::vector2d(tc.BRPoint.x, tc.ULPoint.y),
		math::vector2d(tc.BRPoint.x, tc.BRPoint.y),
		math::vector2d(tc.ULPoint.x, tc.BRPoint.y)
	};
	if (m_eyes[index].flip90)
	{
    		math::Swap(tc.ULPoint.x, tc.ULPoint.y);
    		math::Swap(tc.BRPoint.x, tc.BRPoint.y);
		if (!m_eyes[index].cw)
		{
			coords[0].set(tc.ULPoint.x, tc.BRPoint.y);
			coords[1].set(tc.ULPoint.x, tc.ULPoint.y);
			coords[2].set(tc.BRPoint.x, tc.ULPoint.y);
			coords[3].set(tc.BRPoint.x, tc.BRPoint.y);
		}
		else
		{
			coords[0].set(tc.BRPoint.x, tc.ULPoint.y);
			coords[1].set(tc.BRPoint.x, tc.BRPoint.y);
			coords[2].set(tc.ULPoint.x, tc.BRPoint.y);
			coords[3].set(tc.ULPoint.x, tc.ULPoint.y);
		}
		
	}
	dev->draw2DShapeTextured(points, coords, 4, 1, 1);
	//draw UI


	_RenderUI(rc);

	return m_renderTarget[index].pointer();
}

void IEyesRenderingBaseState::Update(float dt)
{
	m_robotConnector->UpdateStatus();

	if (m_enablePanning)
	{
		if (AppData::Instance()->oculusDevice)
		{
			math::quaternion ori = AppData::Instance()->oculusDevice->GetOrientation();
			math::vector3d angles;
			ori.toEulerAngles(angles);

			float maxAngle = 20 * m_panningScale;
			angles.x = math::clamp(angles.x, -maxAngle, maxAngle);
			angles.y = math::clamp(angles.y, -maxAngle, maxAngle);
			m_headPan = angles.y / maxAngle;
			m_headTilt = -angles.x / maxAngle;

			// 		m_headPan = m_headPan*0.5 + 0.5;
			// 		m_headTilt = m_headTilt*0.5 + 0.5;
		}
	}
}



void IEyesRenderingBaseState::LoadFromXML(xml::XMLElement* e)
{
	IRenderingState::LoadFromXML(e);
	xml::XMLAttribute* attr;

	attr = e->getAttribute("CameraDistance");
	if (attr)
	{
		m_hmdDistance = core::StringConverter::toFloat(attr->value);
	}
	attr = e->getAttribute("CameraFov");
	if (attr)
	{
		m_cameraFov = core::StringConverter::toFloat(attr->value);
	}
	attr = e->getAttribute("HMDFov");
	if (attr)
	{
		m_hmdFov = core::StringConverter::toFloat(attr->value);
	}
	attr = e->getAttribute("HMDSize");
	if (attr)
	{
		m_hmdSize = core::StringConverter::toVector2d(attr->value);
	}
	attr = e->getAttribute("Cam90");
	if (attr)
	{
		m_eyes[0].flip90 = m_eyes[1].flip90 = core::StringConverter::toBool(attr->value);
	}

	attr = e->getAttribute("EnablePanning");
	if (attr)
	{
		m_enablePanning = core::StringConverter::toBool(attr->value);
	}
	attr = e->getAttribute("PanningScale");
	if (attr)
	{
		m_panningScale = core::StringConverter::toFloat(attr->value);
	}

	m_useLensCorrection = e->getValueBool("UseLensCorrection");

	attr = e->getAttribute("CorrectionParamsU");
	if (attr)
	{
		m_correctionParamsU = core::StringConverter::toVector4d(attr->value);
	}
	attr = e->getAttribute("CorrectionParamsV");
	if (attr)
	{
		m_correctionParamsV = core::StringConverter::toVector4d(attr->value);
	}
	//m_robotConnector->LoadXML(e);
}


}
}


