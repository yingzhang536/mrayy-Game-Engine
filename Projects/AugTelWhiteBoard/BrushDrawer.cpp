

#include "stdafx.h"
#include "BrushDrawer.h"
#include "Application.h"
#include "InputManager.h"
#include "IUDPClient.h"
#include "OptiTrackDataSource.h"
#include "TextureResourceManager.h"

namespace mray
{

#define LOCAL_GROUP_ID 0
#define REMOTE_GROUP_ID 1

#define BRUSH_SIZE 20

	BrushDrawer::BrushDrawer()
	{
		m_drawing = false;

		m_activeGroup = 0;

		AddGroup(LOCAL_GROUP_ID);
		//AddGroup(REMOTE_GROUP_ID);

	//	m_groups[REMOTE_GROUP_ID].clr = video::DefaultColors::Blue;

		m_brushTex = gTextureResourceManager.loadTexture2D("BrushTexture.png");
	}
	BrushDrawer::~BrushDrawer()
	{
	}
	void BrushDrawer::_ReceiveData(OS::IStream* stream)
	{

	}

	bool BrushDrawer::OnEvent(Event* e)
	{

		if (ILineDrawer::OnEvent(e))
			return true;
		bool ok = false;
		if (e->getType() == ET_Keyboard)
		{
			KeyboardEvent* evt = dynamic_cast<KeyboardEvent*>(e);
			if (evt->press)
			{
				if (evt->key == KEY_ESCAPE)
				{
					m_activeGroup = 1 - m_activeGroup;
				}
			}

		}
		else if (e->getType() == ET_Mouse)
		{
			MouseEvent* evt = dynamic_cast<MouseEvent*>(e);
			if (evt->event == MET_LEFTDOWN)
			{
			}
		}
		return ok;
	}

	void BrushDrawer::AddGroup(int group)
	{
		GroupInfo g;
		g.id = group;
		video::IVideoDevice* device = Engine::getInstance().getDevice();

		video::ITexturePtr t = device->createEmptyTexture2D(false);
		math::vector2d sz= Application::getInstance().GetRenderWindow(0)->GetSize(); 
		t->setMipmapsFilter(false);
		t->createTexture(math::vector3d(sz.x, sz.y, 1), video::EPixel_R8G8B8);

		g.texture = device->createRenderTarget("", t, 0, 0, 0);
		g.clr = video::DefaultColors::Red;
		m_groups[group] = g;
		device->setRenderTarget(m_groups[group].texture, true, true, video::SColor(1, 1, 1, 0));

		device->draw2DRectangle(math::rectf(0, 0, sz.x, sz.y), video::SColor(1,1,1,0));
		device->setRenderTarget(0, false, false);
	}

	void BrushDrawer::RemoveAt(const math::vector2d& pos)
	{

	}

	bool BrushDrawer::RemoveLast(int group)
	{
		return true;
	}

	void BrushDrawer::DrawAt(const math::vector2d& pos, int group, const video::SColor& clr)
	{
		video::IVideoDevice* device = Engine::getInstance().getDevice();

		video::TextureUnit tu;
		tu.SetTexture(m_brushTex);
		device->set2DMode();
		device->useTexture(0, &tu);
		device->setRenderTarget(m_groups[group].texture,false,false);

		device->draw2DImage(math::rectf(pos - BRUSH_SIZE, pos + BRUSH_SIZE), clr);
		device->setRenderTarget(0, false, false);

	}
	void BrushDrawer::DrawLine(const math::vector2d& a, const math::vector2d& b, int group, const video::SColor& clr)
	{
		float len = (b - a).Length();

		int steps = len / 2;
		if (steps < 1)steps = 1;

		for (int i = 0; i < steps; ++i)
		{
			math::vector2d p = math::lerp(a, b, (float)i / (float)steps);
			DrawAt(p, group, clr);
		}
	}



	void BrushDrawer::Draw(const math::rectf& vp)
	{
		std::map<int, GroupInfo>::iterator git;

		video::IVideoDevice* device = Engine::getInstance().getDevice();
		git = m_groups.begin();
		m_mutex->lock();
		device->set2DMode();
		device->draw2DRectangle(vp, 1);
	//	device->SetSceneBlending(video::EBF_SRC_ALPHA, video::EBF_ONE);
		for (; git != m_groups.end(); ++git)
		{
			video::TextureUnit tu;
			tu.SetTexture(git->second.texture->GetColorTexture());
			device->useTexture(0,&tu);
			device->draw2DImage(vp, 1);

			tu.SetTexture(m_brushTex);
			device->useTexture(0, &tu);
			device->draw2DImage(math::rectf(git->second.currentPos - BRUSH_SIZE, git->second.currentPos + BRUSH_SIZE), git->second.clr);
		}
		m_mutex->unlock();

	}


	void BrushDrawer::Update(float dt)
	{
		controllers::IMouseController* mouse = Application::getInstance().GetInputManager()->getMouse();
		controllers::IKeyboardController* kb = Application::getInstance().GetInputManager()->getKeyboard();

		bool erase = false;
		{
			if (kb->getKeyState(KEY_E))
				erase = true;
			if (kb->getKeyState(KEY_RETURN) || mouse->isPressed(controllers::EMB_Left))
			{
				if (!m_drawing)
				{
					m_drawing = true;
				}
			}
			else
			{
				if (m_drawing)
				{
					m_drawing = false;
				}
			}

		}

		float threshold = 1;

		math::vector2d pos;
		if (_getProjPos(pos))
		{
			/*
			math::vector2d size = m_calibPoints[1] - m_calibPoints[0];
			size.x = fabs(size.x);
			size.y = fabs(size.y);
			math::vector2d pos(pt.x - m_calibPoints[0].x, pt.y - m_calibPoints[0].y);

			pos /= size;
			if (pos.x < 0 || pos.y<0 || pos.x>1 || pos.y>1)
			{
			return;
			}*/

			m_groups[LOCAL_GROUP_ID].currentPos = pos;

			if (!m_drawing)
				return;
			if (pos.getDist(m_groups[LOCAL_GROUP_ID].lastPos) > threshold)
			{
				video::SColor clr = m_groups[LOCAL_GROUP_ID].clr;
				if (erase)
					clr = 1;
				DrawLine(m_groups[LOCAL_GROUP_ID].lastPos,pos, LOCAL_GROUP_ID, clr);
				//m_groups[LOCAL_GROUP_ID].currentLine.points.push_back(pos);
				m_groups[LOCAL_GROUP_ID].lastPos = m_groups[LOCAL_GROUP_ID].currentPos;
				m_groups[LOCAL_GROUP_ID].currentPos = pos;
			}
		}

	}



}


