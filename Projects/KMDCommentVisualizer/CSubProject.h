

#ifndef CSubProject_h__
#define CSubProject_h__


#include "TextureResourceManager.h"
#include "XMLTextNode.h"


namespace mray
{
namespace kmd
{

	class KMDUser;
	class SessionDetails;
	class KMDComment;
class CSubProject
{
public:
	static std::map<core::string, CSubProject*> SubProjectMap;
	static void AddSubProject(CSubProject* t);
	static CSubProject* GetSubProject(const core::string& name);

protected:
	SessionDetails* m_session;
	int m_order;
	core::string m_name;
	core::string m_title;

	int m_length;


public:
	CSubProject(SessionDetails*s) : m_session(s), m_order(0), m_length(5)
	{}
	virtual~CSubProject(){}

	void SetOrder(int o){ m_order = o; }
	int GetOrder(){ return m_order; }
	SessionDetails* GetSession(){ return m_session; }
	const core::string& GetName(){ return m_name; }
	const core::string& GetTitle(){ return m_title; }

	int GetLength(){ return m_length; }

	void LoadFromXML(xml::XMLElement* e)
	{
		m_name = e->getValueString("Name");
		m_title = e->getValueString("Title");
		m_length = e->getValueInt("Length");
	}
};

}
}

#endif // CSubProject_h__
