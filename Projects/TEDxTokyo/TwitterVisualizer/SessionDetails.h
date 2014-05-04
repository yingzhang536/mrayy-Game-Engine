



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
	core::string m_description;

	std::vector<CSpeaker*> m_speakers;

	core::CTime m_sessionTime;
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

	const core::string& GetSessionName()const{ return m_sessionName; }
	const core::string& GetDescription()const{ return m_description; }
	const core::CTime& GetSessionTime()const{ return m_sessionTime; }
	const std::vector<CSpeaker*>& GetSpeakers()const{ return m_speakers; }


	void LoadFromXML(xml::XMLElement* e)
	{
		m_sessionName = e->getValueString("SessionName");
		m_sessionTime = core::CTime::Parse(e->getValueString("SessionTime"));

		xml::XMLElement* elem = e->getSubElement("Speaker");
		while (elem)
		{
			CSpeaker* s = new CSpeaker(this);
			s->LoadFromXML(elem);
			m_speakers.push_back(s);
			elem = elem->nextSiblingElement("Speaker");
		}
		elem = e->getSubElement("Description");
		xml::XMLTextNode* tn = elem->GetTextNode();
		if (tn)
			m_description = tn->GetValue();
	}
};

}
}

#endif
