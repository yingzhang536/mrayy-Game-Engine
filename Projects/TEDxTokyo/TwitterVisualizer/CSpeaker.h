

#ifndef CSpeaker_h__
#define CSpeaker_h__


#include "TextureResourceManager.h"
#include "XMLTextNode.h"


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
	int m_order;
	core::string m_name;
	core::string m_picture;
	core::string m_title;
	core::string m_twitterID;
	core::stringw m_description;
	video::ITexturePtr m_imgTexture;
public:
	CSpeaker(SessionDetails*s) :m_user(0), m_session(s), m_order(0)
	{}
	virtual~CSpeaker(){}

	void SetOrder(int o){ m_order = o; }
	int GetOrder(){ return m_order; }
	SessionDetails* GetSession(){ return m_session; }
	TwitterUserProfile* GetUser(){ return m_user; }
	const core::string& GetName(){ return m_name; }
	const core::string& GetTitle(){ return m_title; }
	const core::string& GetPicture(){ return m_picture; }
	const core::stringw& GetDescription(){ return m_description; }
	const core::string& GetTwitterID(){ return m_twitterID; }
	video::ITexturePtr GetTexture(){ return m_imgTexture; }

	void LoadFromXML(xml::XMLElement* e)
	{
		m_name = e->getValueString("Name");
		m_title = e->getValueString("Title");
		m_picture = e->getValueString("Picture");
		m_twitterID = e->getValueString("TwitterID");
		xml::XMLTextNode* tn= e->GetTextNode();
		if (tn)
			m_description = core::string_to_wchar(tn->GetValue());


		m_imgTexture = gTextureResourceManager.loadTexture2D(m_picture);
		m_imgTexture->setMipmapsFilter(false);
	}
};

}
}

#endif // CSpeaker_h__
