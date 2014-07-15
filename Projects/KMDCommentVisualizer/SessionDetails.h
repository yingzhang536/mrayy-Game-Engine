



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
#include "CSubProject.h"
#include "KMDUser.h"

namespace mray
{
namespace kmd
{
class SessionDetails
{
public:
	static std::map<core::string, SessionDetails*> ProjectMap;
	static void AddSession(SessionDetails* t);
	static SessionDetails* GetSession(const core::string& name);

protected:

	core::string m_sessionName;
	core::string m_theme;
	video::SColor m_color;
	core::stringw m_description;
	core::string m_picture;
	video::ITexturePtr m_imgTexture;

	std::vector<CSubProject*> m_projects;

	core::CTime m_sessionStartTime;
	core::CTime m_sessionEndTime;

	std::vector<KMDUser*> m_projectProf;
	std::vector<KMDUser*> m_commiteeProf;

	std::vector<KMDComment*> m_comments;
public:
	SessionDetails(){}
	virtual ~SessionDetails()
	{
		for (int i = 0; i < m_projects.size(); ++i)
		{
			delete m_projects[i];
		}
		m_projects.clear();
	}

	const video::SColor& GetColor()const{ return m_color; }
	const core::string& GetSessionName()const{ return m_sessionName; }
	const core::string& GetTheme()const{ return m_theme; }
	const core::CTime& GetSessionStartTime()const{ return m_sessionStartTime; }
	const core::CTime& GetSessionEndTime()const{ return m_sessionEndTime; }
	const std::vector<CSubProject*>& GetProjects()const{ return m_projects; }
	const core::stringw& GetDescription(){ return m_description; }

	const core::string& GetPicture(){ return m_picture; }
	video::ITexturePtr GetTexture(){ return m_imgTexture; }

	const std::vector<KMDUser*>& GetProjectProfessors(){ return m_projectProf; }
	const std::vector<KMDUser*>& GetComitteeProfessors(){ return m_commiteeProf; }

	void AddComment(KMDComment* c){ m_comments.push_back(c); }
	const std::vector<KMDComment*>& GetComments()const{ return m_comments; }

	void LoadFromXML(xml::XMLElement* e)
	{
		m_sessionName = e->getValueString("SessionName");
		m_theme = e->getValueString("Theme");
		m_sessionStartTime = core::CTime::Parse(e->getValueString("SessionStartTime"));
		m_sessionEndTime = core::CTime::Parse(e->getValueString("SessionEndTime"));

		math::vector3d clr = core::StringConverter::toVector3d(e->getValueString("Color"))/255.0f;
		m_color.Set(clr.x, clr.y, clr.z, 1);
		xml::XMLElement* elem = e->getSubElement("SubProject");
		int o = 0;
		while (elem)
		{
			CSubProject* s = new CSubProject(this);
			s->LoadFromXML(elem);
			s->SetOrder(o);
			++o;
			m_projects.push_back(s);
			CSubProject::AddSubProject(s);
			elem = elem->nextSiblingElement("SubProject");
		}



		elem = e->getSubElement("Adviser");
		while (elem)
		{
			KMDUser* p = KMDUser::GetUserByName(elem->getValueString("Name"));
			if (p)
				m_projectProf.push_back(p);
			elem = elem->nextSiblingElement("Adviser");
		}

		elem = e->getSubElement("Committee");
		while (elem)
		{

			KMDUser* p = KMDUser::GetUserByName(elem->getValueString("Name"));
			if (p)
				m_commiteeProf.push_back(p);
			elem = elem->nextSiblingElement("Committee");
		}


		m_picture = e->getValueString("Picture");
		
		elem = e->getSubElement("Description");
		if (elem)
		{
			xml::XMLTextNode* tn = elem->GetTextNode();
			if (tn)
				m_description = core::string_to_wchar(tn->GetValue());
		}

		m_imgTexture = gTextureResourceManager.loadTexture2D(m_picture);
		m_imgTexture->setMipmapsFilter(false);

	}
};

}
}

#endif
