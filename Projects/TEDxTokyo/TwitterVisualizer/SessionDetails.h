



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

namespace mray
{
namespace ted
{

class SessionDetails
{
protected:

	core::string m_sessionName;
	core::string m_speakerName;
	core::string m_description;

	core::CTime m_sessionTime;
public:
	SessionDetails(){}
	virtual ~SessionDetails(){}

	const core::string& GetSessionName()const{ return m_sessionName; }
	const core::string& GetSpeakerName()const{ return m_speakerName; }
	const core::string& GetDescription()const{ return m_description; }
	const core::CTime& GetSessionTime()const{ return m_sessionTime; }


	void LoadFromXML(xml::XMLElement* e)
	{
		m_sessionName = e->getValueString("SessionName");
		m_speakerName = e->getValueString("SpeakerName");
		m_sessionTime = core::CTime::Parse(e->getValueString("SessionTime"));

		xml::XMLTextNode* tn= e->GetTextNode();
		if (tn)
			m_description = tn->GetValue();
	}
};

}
}

#endif
