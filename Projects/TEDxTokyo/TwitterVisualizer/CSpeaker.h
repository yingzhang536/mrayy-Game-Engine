

#ifndef CSpeaker_h__
#define CSpeaker_h__

namespace mray
{
namespace ted
{

	class TwitterUserProfile;
	class SessionDetails;
class CSpeaker
{
protected:
	SessionDetails* m_session;
	TwitterUserProfile* m_user;
	core::string m_name;
	core::string m_picture;
	core::string m_twitterID;
public:
	CSpeaker(SessionDetails*s) :m_user(0), m_session(s)
	{}
	virtual~CSpeaker(){}

	SessionDetails* GetSession(){ return m_session; }
	TwitterUserProfile* GetUser(){ return m_user; }
	const core::string& GetPicture(){ return m_picture; }
	const core::string& GetTwitterID(){ return m_twitterID; }

	void LoadFromXML(xml::XMLElement* e)
	{
		m_name = e->getValueString("Name");
		m_picture = e->getValueString("Picture");
		m_twitterID = e->getValueString("TwitterID");
	}
};

}
}

#endif // CSpeaker_h__
