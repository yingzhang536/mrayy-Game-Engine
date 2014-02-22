

/********************************************************************
	created:	2014/02/16
	created:	16:2:2014   12:42
	filename: 	C:\Development\mrayEngine\Projects\AugTelWhiteBoard\LineDrawer.h
	file path:	C:\Development\mrayEngine\Projects\AugTelWhiteBoard
	file base:	LineDrawer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __LineDrawer__
#define __LineDrawer__

#include "StreamReader.h"
#include "IUDPClient.h"
#include "SurfaceWrapper.h"


namespace mray
{
	class UDPLineReceivingThread;
	class MovingAverage;

	enum class EDrawMessages
	{
		AddLine =1,
		RemoveLastLine=2,
		CurrentLine = 3,
		CurrentPos = 4
	};

class LineDrawer
{
protected:
	struct LineInfo
	{
		LineInfo()
		{
			clr = video::DefaultColors::Black;
			width = 2;
		}
		std::vector<math::vector2d> points;
		video::SColor clr;
		float width;
	};

	std::list<LineInfo> m_localLines;
	std::list<LineInfo> m_remoteLines;

	math::vector2di m_currentPos;
	math::vector2di m_lastPos;
	LineInfo m_currentLine;
	LineInfo m_currentRemoteLine;
	math::vector2di m_currentRemotePos;

	OS::IMutex* m_mutex;
	network::IUDPClient* m_rcvClient;
	UDPLineReceivingThread* m_rcvThreadFunc;
	OS::IThread* m_rcvThread;

	AugTel::SurfaceWrapper m_wrapper;
	MovingAverage* m_movingAverage;

	bool m_drawing;

	int m_calibPoint;
	math::vector2d m_calibPoints[4];

	friend class UDPLineReceivingThread;

	void _readLine(OS::StreamReader& stream, LineInfo& ret);
	void _ReceiveData(OS::IStream* stream);

	void _DrawLine(LineInfo* line);

	bool _getProjPos(math::vector2d& pos);

public:
	LineDrawer();
	virtual~LineDrawer();


	bool OnEvent(Event* e);

	void StartReciver(int port);

	void Draw(const math::rectf& vp);

	void Update(float dt);
};

}


#endif
