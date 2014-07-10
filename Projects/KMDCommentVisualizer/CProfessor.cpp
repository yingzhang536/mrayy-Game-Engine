
#include "stdafx.h"
#include "CProfessor.h"

namespace mray
{
namespace kmd
{

CProfessor::ProfMap CProfessor::s_Professors;

CProfessor* CProfessor::GetProfessor(const core::string& name)
{
	core::string s = core::StringUtil::ToLower(name);
	ProfMap::iterator it = s_Professors.find(s);
	if (it == s_Professors.end())
		return 0;
	return it->second;

}
void CProfessor::AddProfessor(CProfessor* p)
{
	core::string s = core::StringUtil::ToLower(p->GetName());
	ProfMap::iterator it = s_Professors.find(s);
	if (it != s_Professors.end())
		return;
	s_Professors[s] = p;
}

CProfessor* CProfessor::LoadProfessor(xml::XMLElement* e)
{

	CProfessor* p;
	p = GetProfessor(e->getValueString("Name"));
	if (p)
	{
	}
	else
	{
		p = new CProfessor();
		p->LoadFromXML(e);
		AddProfessor(p);
	}
	return p;
}

}
}
