

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

#include "ILineDrawer.h"


namespace mray
{
	class MovingAverage;

	enum class EDrawMessages
	{
		AddLine =1,
		RemoveLastLine=2,
		CurrentLine = 3,
		CurrentPos = 4
	};

class LineDrawer:public ILineDrawer
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
		float width;
		video::SColor clr;
	};

	struct GroupInfo
	{
		int id;
		std::list<LineInfo> lines;
		LineInfo currentLine;
		math::vector2di currentPos;
		math::vector2di lastPos;
	};

	std::map<int, GroupInfo> m_groups;


	void _readLine(OS::StreamReader& stream, LineInfo& ret);
	virtual void _ReceiveData(OS::IStream* stream) ;
	void _DrawLine(LineInfo* line);
public:
	LineDrawer();
	virtual~LineDrawer();

	void AddGroup(int group) ;

	void RemoveAt(const math::vector2d& pos) ;

	bool RemoveLast(int group) ;

	void DrawAt(const math::vector2d& pos, int group, const video::SColor& clr);

	bool OnEvent(Event* e);

	void Draw(const math::rectf& vp);

	void Update(float dt);
};

}


#endif
