

#include "stdafx.h"
#include "LineDrawer.h"
#include "Application.h"
#include "InputManager.h"
#include "IUDPClient.h"
#include "OptiTrackDataSource.h"

namespace mray
{

#define LOCAL_GROUP_ID 0
#define REMOTE_GROUP_ID 1

LineDrawer::LineDrawer()
{
	m_drawing = false;

	AddGroup(LOCAL_GROUP_ID);
	AddGroup(REMOTE_GROUP_ID);
}
LineDrawer::~LineDrawer()
{
}

void LineDrawer::_readLine(OS::StreamReader& rdr, LineDrawer::LineInfo& ret)
{
	ret.width = rdr.binReadFloat();
	ret.clr.R = rdr.binReadFloat();
	ret.clr.G = rdr.binReadFloat();
	ret.clr.B = rdr.binReadFloat();
	ret.clr.A = rdr.binReadFloat();
	int cnt = rdr.binReadInt();
	math::vector2d wsz = Application::getInstance().GetRenderWindow()->GetSize();
	for (int i = 0; i < cnt; ++i)
	{
		math::vector2d pt;
		pt.x = rdr.binReadFloat();
		pt.y = rdr.binReadFloat();

		ret.points.push_back(pt*wsz);
	}
}

void LineDrawer::_ReceiveData(OS::IStream* stream)
{
	OS::StreamReader rdr(stream);
	int messageType = rdr.binReadInt();
	if (messageType == (int)EDrawMessages::AddLine)
	{
		LineInfo line;
		_readLine(rdr, line);
		m_mutex->lock();
		m_groups[REMOTE_GROUP_ID].lines.push_back(line);
		m_groups[REMOTE_GROUP_ID].currentLine.points.clear();
		m_mutex->unlock();
	}
	else
	if (messageType == (int)EDrawMessages::RemoveLastLine)
	{
		m_mutex->lock();
		if (!m_groups[REMOTE_GROUP_ID].lines.empty())
			m_groups[REMOTE_GROUP_ID].lines.pop_back();
		m_mutex->unlock();
	}
	else
	if (messageType == (int)EDrawMessages::CurrentPos)
	{
		m_mutex->lock();

		stream->read(&m_groups[REMOTE_GROUP_ID].currentPos, sizeof(m_groups[REMOTE_GROUP_ID].currentPos));
		m_mutex->unlock();
	}
	else
	if (messageType == (int)EDrawMessages::CurrentLine)
	{
		m_mutex->lock();
		m_groups[REMOTE_GROUP_ID].currentLine.points.clear();
		_readLine(rdr, m_groups[REMOTE_GROUP_ID].currentLine);
		m_mutex->unlock();
	}

}

bool LineDrawer::OnEvent(Event* e)
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
				if (!m_groups[LOCAL_GROUP_ID].lines.empty())
					m_groups[LOCAL_GROUP_ID].lines.pop_back();
				
				ok = true;
			}
			if (evt->key == KEY_1)
				m_groups[LOCAL_GROUP_ID].currentLine.clr = video::DefaultColors::Red;
			else if (evt->key == KEY_2)
				m_groups[LOCAL_GROUP_ID].currentLine.clr = video::DefaultColors::Blue;
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

void LineDrawer::AddGroup(int group)
{
	GroupInfo g;
	g.id = group;

	g.currentLine.clr = video::DefaultColors::Red;
	g.currentLine.width = 10;
	m_groups[group] = g;
}

void LineDrawer::RemoveAt(const math::vector2d& pos)
{

}

bool LineDrawer::RemoveLast(int group)
{
	return true;
}

void LineDrawer::DrawAt(const math::vector2d& pos, int group, const video::SColor& clr)
{

}

void LineDrawer::_DrawLine(LineInfo* line)
{
	if (line->points.size() <= 1)
		return;
		
	video::IVideoDevice* device= Engine::getInstance().getDevice();

	device->setLineWidth(line->width);

	device->draw2DLine(&line->points[0], line->points.size(), line->clr);
}

void LineDrawer::Draw(const math::rectf& vp)
{
	std::map<int, GroupInfo>::iterator git;
	std::list<LineInfo>::iterator it;

	video::IVideoDevice* device = Engine::getInstance().getDevice();
	git = m_groups.begin();
	m_mutex->lock();
	for (; git != m_groups.end();++git)
	{
		it = git->second.lines.begin();
		for (; it != git->second.lines.end(); ++it)
		{
			_DrawLine(&*it);
		}
		_DrawLine(&git->second.currentLine);

		device->draw2DRectangle(math::rectf(git->second.currentPos - 2, git->second.currentPos + 2), git->second.currentLine.clr);
	}
	m_mutex->unlock();

}


void LineDrawer::Update(float dt)
{
	controllers::IMouseController* mouse = Application::getInstance().GetInputManager()->getMouse();
	controllers::IKeyboardController* kb = Application::getInstance().GetInputManager()->getKeyboard();
	
	{
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
				m_groups[LOCAL_GROUP_ID].lines.push_back(m_groups[LOCAL_GROUP_ID].currentLine);
				m_groups[LOCAL_GROUP_ID].currentLine.points.clear();
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
			m_groups[LOCAL_GROUP_ID].currentLine.points.push_back(pos);
			m_groups[LOCAL_GROUP_ID].lastPos = pos;
		}
	}
	
}



}


