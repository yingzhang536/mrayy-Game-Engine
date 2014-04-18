

#include "stdafx.h"
#include "LineDrawer.h"
#include "Application.h"
#include "InputManager.h"
#include "IThreadManager.h"
#include "IThreadFunction.h"
#include "IUDPClient.h"
#include "INetwork.h"
#include "OptiTrackDataSource.h"

namespace mray
{

	class UDPLineReceivingThread :public OS::IThreadFunction
	{
	protected:
		network::IUDPClient* m_client;
		LineDrawer* m_owner;
	public:

		UDPLineReceivingThread(network::IUDPClient*c, LineDrawer*o)
		{
			m_client = c;
			m_owner = o;
		}
		virtual void execute(OS::IThread*caller, void*arg)
		{
			const uint Size = 6000;
			byte buffer[Size];
			while (caller->isActive())
			{
				uint len = Size;
				network::NetAddress addr;
				network::UDPClientError err = m_client->RecvFrom((char*)buffer, &len, &addr);
				if (err == network::UDP_SOCKET_ERROR_NONE)
				{
					OS::CMemoryStream stream("", buffer, len, false);
					m_owner->_ReceiveData(&stream);
				}
				else
				{
					printf("%s\n", network::IUDPClient::TranslateErrorMessage(err).c_str());
				}
			}
		}
	};

	class MovingAverage
	{
		std::list<math::vector3d> m_points;
		math::vector3d m_average;

		std::vector<float> m_kernel;
		int m_cnt;
	public:
		MovingAverage()
		{
			m_cnt = 20;
			float s = 0;
			float w = 0;
			for (int i = 0; i < m_cnt; ++i)
			{
				w = powf(i + 1,2.0f);
				m_kernel.push_back(w);
				s += w;
			}
			for (int i = 0; i<m_kernel.size(); ++i)
				m_kernel[i] /= s;
		}
		math::vector3d AddPoint(math::vector3d& pt)
		{
			m_points.push_back(pt);
			if (m_points.size() > m_cnt)
			{
				m_points.pop_front();
			}
			std::list<math::vector3d>::iterator it= m_points.begin();
			m_average = 0;
			for (int i = 0; it != m_points.end(); ++it,++i)
			{
				m_average += *it*m_kernel[i];
			}
			//m_average /= m_points.size();
			return m_average;

		}
	};

LineDrawer::LineDrawer()
{
	m_drawing = false;

	m_mutex = 0;
	m_rcvThreadFunc = 0;
	m_rcvThread = 0;
	m_rcvClient = 0;
	m_currentLine.clr = video::DefaultColors::Red;
	m_currentLine.width = 10;
	m_calibPoint = 0;
	m_movingAverage = new MovingAverage;
}
LineDrawer::~LineDrawer()
{
	if (m_rcvClient)
	{
		m_rcvClient->Close();
	}
	OS::IThreadManager::getInstance().killThread(m_rcvThread);
	delete m_mutex;
	delete m_rcvClient;
	delete m_movingAverage;
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
		m_remoteLines.push_back(line);
		m_currentRemoteLine.points.clear();
		m_mutex->unlock();
	}
	else
	if (messageType == (int)EDrawMessages::RemoveLastLine)
	{
		m_mutex->lock();
		if (!m_remoteLines.empty())
			m_remoteLines.pop_back();
		m_mutex->unlock();
	}
	else
	if (messageType == (int)EDrawMessages::CurrentPos)
	{
		m_mutex->lock();

		stream->read(&m_currentRemotePos, sizeof(m_currentRemotePos));
		m_mutex->unlock();
	}
	else
	if (messageType == (int)EDrawMessages::CurrentLine)
	{
		m_mutex->lock();
		m_currentRemoteLine.points.clear();
		_readLine(rdr, m_currentRemoteLine);
		m_mutex->unlock();
	}

}

bool LineDrawer::OnEvent(Event* e)
{
	if (e->getType() == ET_Keyboard)
	{
		KeyboardEvent* evt = dynamic_cast<KeyboardEvent*>(e);
		if (evt->press)
		{
			if (evt->key == KEY_ESCAPE)
			{
				if (!m_localLines.empty())
					m_localLines.pop_back();
				return true;
			}
			if (evt->key == KEY_C)
			{
				m_calibPoint = 0;
			}
			if (evt->key == KEY_1)
				m_currentLine.clr = video::DefaultColors::Red;
			else if (evt->key == KEY_2)
				m_currentLine.clr = video::DefaultColors::Blue;
		}

	}
	else if (e->getType() == ET_Mouse)
	{
		MouseEvent* evt = dynamic_cast<MouseEvent*>(e);
		if (evt->event == MET_LEFTDOWN)
		{
			if (m_calibPoint <4)
			{
				math::vector3d* p = Application::getInstance().GetOptiProvider()->GetPositionByID(1);
					if (p)
				{
					m_calibPoints[m_calibPoint].x = p->x;
					m_calibPoints[m_calibPoint].y = p->y;
					m_calibPoint++;
					printf("Point calib[%d]: %f,%f\n", m_calibPoint, p->x, p->y);
					if (m_calibPoint == 4)
					{
						/*
						if (m_calibPoints[1].x < m_calibPoints[0].x)
							math::Swap(m_calibPoints[1].x , m_calibPoints[0].x);
						if (m_calibPoints[1].y < m_calibPoints[0].y)
							math::Swap(m_calibPoints[1].y, m_calibPoints[0].y);
						math::vector2d size = m_calibPoints[1] - m_calibPoints[0];
						size.x = fabs(size.x);
						size.y = fabs(size.y);
						if (size.x == 0 || size.y == 0)
						{
							m_calibPoint = 0;
							printf("failed to calibrate!\n");
						}else
							printf("Screen Calibrated\n", m_calibPoint, p->x, p->y);
							*/

						m_wrapper.setSource(m_calibPoints[0], m_calibPoints[1], m_calibPoints[2], m_calibPoints[3]);


						printf("Screen Calibrated\n");
					}
				}
			}
		}
	}
	return false;
}

void LineDrawer::StartReciver(int port)
{
	m_rcvClient = network::INetwork::getInstance().createUDPClient();
	m_rcvThreadFunc = new UDPLineReceivingThread(m_rcvClient,this);
	m_rcvThread = OS::IThreadManager::getInstance().createThread(m_rcvThreadFunc);
	m_mutex = OS::IThreadManager::getInstance().createMutex();
	m_rcvClient->Open(port);

	m_rcvThread->start(0);
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
	std::list<LineInfo>::iterator it;
	it = m_localLines.begin();
	for (; it != m_localLines.end();++it)
	{
		_DrawLine(&*it);
	}

	if (m_mutex)
	{
		m_mutex->lock();
		it = m_remoteLines.begin();
		for (; it != m_remoteLines.end(); ++it)
		{
			_DrawLine(&*it);
		}
		m_mutex->unlock();
		_DrawLine(&m_currentRemoteLine);
	}
	_DrawLine(&m_currentLine);
	video::IVideoDevice* device = Engine::getInstance().getDevice();

	device->draw2DRectangle(math::rectf(m_currentPos - 2, m_currentPos + 2), m_currentLine.clr);
	device->draw2DRectangle(math::rectf(m_currentRemotePos - 2, m_currentRemotePos + 2), m_currentRemoteLine.clr);
}

bool LineDrawer::_getProjPos(math::vector2d& pos)
{

	math::vector3d* p = Application::getInstance().GetOptiProvider()->GetPositionByID(1);
	if (p)
	{
		if (m_calibPoint != 4)
			return false;
		math::vector3d pt = m_movingAverage->AddPoint(*p);
		pos = m_wrapper.Wrap(math::vector2d(pt.x, pt.y));
		pos.y = 1 - pos.y;
		pos *= Application::getInstance().GetRenderWindow()->GetSize();
	}
	else
	{
		controllers::IMouseController* mouse = Application::getInstance().GetInputManager()->getMouse();	
		pos.set(mouse->getX(), mouse->getY());
		
	}
	return true;

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
				m_localLines.push_back(m_currentLine);
				m_currentLine.points.clear();
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
		
		m_currentPos = pos;

		if (!m_drawing)
			return;
		if (pos.getDist(m_lastPos) > threshold)
		{
			m_currentLine.points.push_back(pos);
			m_lastPos = pos;
		}
	}
	
}



}


