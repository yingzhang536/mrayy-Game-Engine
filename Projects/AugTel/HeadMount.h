

/********************************************************************
	created:	2014/01/16
	created:	16:1:2014   22:53
	filename: 	C:\Development\mrayEngine\Projects\AugTel\HeadMount.h
	file path:	C:\Development\mrayEngine\Projects\AugTel
	file base:	HeadMount
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __HeadMount__
#define __HeadMount__

#include "ISceneNode.h"
#include "OculusDevice.h"
#include "InputManager.h"
#include "Application.h"
#include "ATAppGlobal.h"


namespace mray
{
namespace AugTel
{


class HeadMount :public scene::ISceneNode
{
protected:

	float m_speed;
	bool m_disable;
	video::OculusDevice* m_oculus;
	math::vector3d m_angles;
	scene::IMovable* m_parent;
	math::vector3d m_parentOffset;

	scene::CameraNode* m_camera[2];
public:
	//TrackIRCamera* m_trackCam;
	void SetDisabled(bool d)
	{
		if (!m_disable)
		{
			m_parent = getParent();
			if (m_parent)
			{
				m_parentOffset = getPosition();
				m_parent->removeChild(this,true);
			}
		}
		else
		{
			if (m_parent)
			{
				m_parent->addChild(this, true);
				setPosition(m_parentOffset);
				setOrintation(math::quaternion::Identity);
			}
		}
		m_disable = d;
	}
	bool IsDisabled()
	{
		return m_disable;
	}

	void SetCamera(scene::CameraNode* left, scene::CameraNode* right)
	{
		m_camera[0] = left;
		m_camera[1] = right;
	}

	scene::CameraNode* GetLeftCamera(){
		return m_camera[0];
	}
	scene::CameraNode* GetRightCamera(){
		return m_camera[1];
	}

	void SetOculus(video::OculusDevice* d)
	{
		m_oculus = d;
	}
	HeadMount(scene::ISceneManager*smngr, float speed) :scene::ISceneNode(mT("HeadMount"), 0, smngr)
	{
		m_parent = 0;
		m_oculus = 0;
		m_speed = speed;
		m_disable = false;
	}
	virtual void update(float dt){
		ISceneNode::update(dt);
		if (!this->isVisible() || !m_disable)
			return;
		math::quaternion resultQ;

		InputManager* m_input = ATAppGlobal::Instance()->App->GetInputManager();
		{

			// 			dx*=(1+2*gKeyboardController.isLShiftPress());
			// 			dz*=(1+2*gKeyboardController.isLShiftPress());

			if (m_input->getMouse()->isPressed(controllers::EMB_Right)){
				m_angles.y += -80 * m_input->getMouse()->getDX()*dt;
				m_angles.x += 80 * m_input->getMouse()->getDY()*dt;
			}

		}
		if (m_oculus)
		{
			math::quaternion q = m_oculus->GetOrientation();
			// 				q.x= q.x;
			// 				q.y= q.y;
			// 				q.z= q.z;
			//this->setOrintation(q);
			math::vector3d angles;
			q.toEulerAngles(angles);
			resultQ.fromEulerAngles(angles.x, -angles.y, -angles.z);
		}
		resultQ *= math::quaternion(m_angles.x, m_angles.y, m_angles.z);
		this->setOrintation(resultQ);
		float dx = m_speed*(m_input->getKeyboard()->getKeyState(KEY_A) -
			m_input->getKeyboard()->getKeyState(KEY_D))*dt;
		float dz = m_speed*(m_input->getKeyboard()->getKeyState(KEY_W) -
			m_input->getKeyboard()->getKeyState(KEY_S))*dt;
		this->translate(math::vector3d(dx, 0, dz), scene::TS_Local);
		/*
		math::vector3d pos,rot;
		if(m_trackCam->ReceiveData(pos,rot))
		{
		rot*=90.0f/5000.0f;
		this->setOrintation(rot);
		}
		*/

	}

};

}
}


#endif
