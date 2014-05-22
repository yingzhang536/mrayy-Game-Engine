



/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   17:40
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\SessionDetails.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	SessionDetails
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __SessionDetails__
#define __SessionDetails__


#include "DateTime.h"
#include "XMLElement.h"
#include "XMLTextNode.h"
#include "CSpeaker.h"

namespace mray
{
namespace ted
{
class SessionDetails
{
protected:

	core::string m_sessionName;
	core::string m_theme;
	video::SColor m_color;

	std::vector<CSpeaker*> m_speakers;

	core::CTime m_sessionStartTime;
	core::CTime m_sessionEndTime;
public:
	SessionDetails(){}
	virtual ~SessionDetails()
	{
		for (int i = 0; i < m_speakers.size();++i)
		{
			delete m_speakers[i];
		}
		m_speakers.clear();
	}

	const video::SColor& GetColor()const{ return m_color; }
	const core::string& GetSessionName()const{ return m_sessionName; }
	const core::string& GetTheme()const{ return m_theme; }
	const core::CTime& GetSessionStartTime()const{ return m_sessionStartTime; }
	const core::CTime& GetSessionEndTime()const{ return m_sessionEndTime; }
	const std::vector<CSpeaker*>& GetSpeakers()const{ return m_speakers; }


	void LoadFromXML(xml::XMLElement* e)
	{
		m_sessionName = e->getValueString("SessionName");
		m_theme = e->getValueString("Theme");
		m_sessionStartTime = core::CTime::Parse(e->getValueString("SessionStartTime"));
		m_sessionEndTime = core::CTime::Parse(e->getValueString("SessionEndTime"));

		math::vector3d clr = core::StringConverter::toVector3d(e->getValueString("Color"))/255.0f;
		m_color.Set(clr.x, clr.y, clr.z, 1);
		xml::XMLElement* elem = e->getSubElement("Speaker");
		int o = 0;
		while (elem)
		{
			CSpeaker* s = new CSpeaker(this);
			s->LoadFromXML(elem);
			s->SetOrder(o);
			++o;
			m_speakers.push_back(s);
			elem = elem->nextSiblingElement("Speaker");
		}
	}
};

}
}

#endif
