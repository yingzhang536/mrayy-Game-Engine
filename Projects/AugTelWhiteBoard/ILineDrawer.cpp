
#include "stdafx.h"
#include "ILineDrawer.h"


#include "IThreadManager.h"
#include "IThreadFunction.h"
#include "INetwork.h"
#include "Application.h"
#include "OptiTrackDataSource.h"

namespace mray
{

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
				w = powf(i + 1, 2.0f);
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
			std::list<math::vector3d>::iterator it = m_points.begin();
			m_average = 0;
			for (int i = 0; it != m_points.end(); ++it, ++i)
			{
				m_average += *it*m_kernel[i];
			}
			//m_average /= m_points.size();
			return m_average;

		}
	};

	class UDPLineReceivingThread :public OS::IThreadFunction
	{
	protected:
		network::IUDPClient* m_client;
		ILineDrawer* m_owner;
	public:

		UDPLineReceivingThread(network::IUDPClient*c, ILineDrawer*o)
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
				network::UDPClientError err = m_client->RecvFrom((char*)buffer, &len, &addr, 0);
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

	ILineDrawer::ILineDrawer()
	{

		m_mutex = 0;
		m_rcvThreadFunc = 0;
		m_rcvThread = 0;
		m_rcvClient = 0;
		m_calibPoint = 0;
		m_movingAverage = new MovingAverage;
	}
	ILineDrawer::~ILineDrawer()
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


	bool ILineDrawer::OnEvent(Event* e)
	{
		bool ok = false;
		if (e->getType() == ET_Keyboard)
		{
			KeyboardEvent* evt = dynamic_cast<KeyboardEvent*>(e);
			if (evt->press)
			{
				if (evt->key == KEY_C)
				{
					m_calibPoint = 0;
					ok = true;
				}
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
						ok = true;
					}
				}
			}
		}
		return ok;
	}
	void ILineDrawer::StartReciver(int port)
	{
		m_rcvClient = network::INetwork::getInstance().createUDPClient();
		m_rcvThreadFunc = new UDPLineReceivingThread(m_rcvClient, this);
		m_rcvThread = OS::IThreadManager::getInstance().createThread(m_rcvThreadFunc);
		m_mutex = OS::IThreadManager::getInstance().createMutex();
		m_rcvClient->Open(port);

		m_rcvThread->start(0);
	}
	bool ILineDrawer::_getProjPos(math::vector2d& pos)
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






}


