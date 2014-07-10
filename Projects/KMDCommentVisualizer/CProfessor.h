

#ifndef CProfessor_h__
#define CProfessor_h__


#include "TextureResourceManager.h"
#include "StringUtil.h"

namespace mray
{
namespace kmd
{

	class SessionDetails;
class CProfessor
{
protected:

	typedef std::map<core::string, CProfessor*> ProfMap;
	static ProfMap s_Professors;
public:

	static CProfessor* GetProfessor(const core::string& name);
	static void AddProfessor(CProfessor* p);

	static CProfessor* LoadProfessor(xml::XMLElement* e);

protected:

	core::string m_name;
	core::string m_picture;
	core::string m_title;
	video::ITexturePtr m_imgTexture;
public:
	CProfessor() {}
	virtual ~CProfessor(){}

	const core::string& GetName(){ return m_name; }
	const core::string& GetTitle(){ return m_title; }
	const core::string& GetPicture(){ return m_picture; }
	video::ITexturePtr GetTexture(){ return m_imgTexture; }

	void LoadFromXML(xml::XMLElement* e)
	{
		m_name = e->getValueString("Name");
		m_title = e->getValueString("Title");
		m_picture = e->getValueString("Picture");

		m_imgTexture = 0;
		if (m_picture != "")
			m_imgTexture = gTextureResourceManager.loadTexture2D(m_picture);
		
		if (m_imgTexture.isNull())
			m_imgTexture = gTextureResourceManager.loadTexture2D("DefaultPicture.png");
		if (m_imgTexture)
			m_imgTexture->setMipmapsFilter(false);
	}
	
};

}
}

#endif // CProfessor_h__
