

#include "stdafx.h"
#include "CSubProject.h"



namespace mray
{
namespace kmd
{

std::map<core::string, CSubProject*> CSubProject::SubProjectMap;
void CSubProject::AddSubProject(CSubProject* t)
{
	SubProjectMap[core::StringUtil::ToLower(t->GetName())] = t;
}
CSubProject* CSubProject::GetSubProject(const core::string& name)
{
	std::map<core::string, CSubProject*>::iterator it = SubProjectMap.find(core::StringUtil::ToLower(name));
	if (it == SubProjectMap.end())
		return 0;
	return it->second;
}

}
}