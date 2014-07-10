



/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   18:59
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\SessionContainer.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	SessionContainer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __SessionContainer__
#define __SessionContainer__


namespace mray
{
namespace kmd
{

class SessionDetails;

class SessionContainer
{
protected:
	std::vector<SessionDetails*> m_sessions;
	int m_currentSession;

public:
	typedef DelegateEvent2<SessionContainer*, const SessionDetails*> SessionChanged;
	SessionChanged OnSessionChanged;
public:

	SessionContainer();
	virtual ~SessionContainer();

	int GetCurrentSessionID(){ return m_currentSession; }
	SessionDetails* GetCurrentSession();

	void SetCurrentSession(int s);
	const std::vector<SessionDetails*>& GetSessions(){ return m_sessions; }

	void Clear();

	void LoadFromXML(const core::string& path);

};

}
}

#endif
