
#include "stdafx.h"
#include "KMDComment.h"

#include "IDBHandler.h"
#include "SQLAPI.h"
#include "KMDUser.h"
#include "StringUtil.h"

namespace mray
{
namespace kmd
{
class UserD;


std::map<IDType, uint> KMDComment::CommentIDMap;
std::vector<KMDComment*> KMDComment::KMDCommentList;

void KMDComment::AddKMDComment(KMDComment* t)
{
	KMDCommentList.push_back(t);
	CommentIDMap[t->ID] = KMDCommentList.size() - 1;
}
KMDComment* KMDComment::GetCommentByID(IDType id, bool localOnly)
{
	std::map<IDType, uint>::iterator it = CommentIDMap.find(id);
	if (it == CommentIDMap.end())
	{
		if (!localOnly)
		{
			KMDComment* u = IDBHandler::getInstance().RequestComment(id);
			if (u)
				AddKMDComment(u);
			return u;
		}else
			return 0;
	}
	return KMDCommentList[it->second];

}


void KMDComment::LoadXML(xml::XMLElement* e)
{
	ID = core::StringConverter::toUInt(e->getValueString("ID"));
	text = ConvertToUtf16(e->getValueString("text"));
	core::DateTime::Parse(e->getValueString("date"), date);
	

}
void KMDComment::SaveXML(xml::XMLElement* e)
{
	xml::XMLElement* elem = new xml::XMLElement("Comment");
	e->addSubElement(elem);

	elem->addAttribute("ID", core::StringConverter::toString(ID));
	elem->addAttribute("user", core::StringConverter::toString(user->name));
	elem->addAttribute("text", ConvertToUtf8(text));
	elem->addAttribute("date", core::DateTime::ToString(date));
}


}
}

