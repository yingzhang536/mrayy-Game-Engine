
#include "stdafx.h"
#include "SessionDetails.h"
#include "StringUtil.h"



namespace mray
{
namespace kmd
{

	std::map<core::string, SessionDetails*> SessionDetails::ProjectMap;
void SessionDetails::AddSession(SessionDetails* t)
{
	ProjectMap[core::StringUtil::ToLower(t->GetSessionName())] = t;
}
SessionDetails* SessionDetails::GetSession(const core::string& name)
{
	std::map<core::string, SessionDetails*>::iterator it = ProjectMap.find(core::StringUtil::ToLower(name));
	if (it == ProjectMap.end())
		return 0;
	return it->second;
}

}
}


