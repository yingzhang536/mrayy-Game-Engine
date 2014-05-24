
#include "stdafx.h"
#include "TwitterUserProfile.h"

#include "SQLAPI.h"
#include "DataTypes.h"
#include "IDBHandler.h"

namespace mray
{
namespace ted
{
class UserD;


std::map<IDType, uint> TwitterUserProfile::UserIDMap;
std::vector<TwitterUserProfile*> TwitterUserProfile::TwitterUserProfileList;

void TwitterUserProfile::AddTwitterUserProfile(TwitterUserProfile* t)
{
	TwitterUserProfileList.push_back(t);
	UserIDMap[t->ID] = TwitterUserProfileList.size() - 1;
}
TwitterUserProfile* TwitterUserProfile::GetUserByID(IDType id, bool localOnly)
{
	std::map<IDType, uint>::iterator it = UserIDMap.find(id);
	if (it == UserIDMap.end())
	{
		if (localOnly)
			return 0;
		TwitterUserProfile* u = IDBHandler::getInstance().RequestUser(id);
		if (u)
			AddTwitterUserProfile(u);
		return u;
	}
	return TwitterUserProfileList[it->second];
}

void TwitterUserProfile::LoadXML(xml::XMLElement* e)
{
	ID = core::StringConverter::toUInt(e->getValueString("ID"));
	followersCount = e->getValueInt("followersCount");
	imageUrl = e->getValueString("imageUrl");
	imageUrlHi = e->getValueString("imageUrlHi");
	name = ConvertToUtf16(e->getValueString("name"));
	displayName = ConvertToUtf16(e->getValueString("displayName"));
	place = ConvertToUtf16(e->getValueString("place"));
	URL = ConvertToUtf16(e->getValueString("URL"));
}
void TwitterUserProfile::SaveXML(xml::XMLElement* e)
{
	xml::XMLElement* elem = new xml::XMLElement("User");
	e->addSubElement(elem);

	elem->addAttribute("ID", core::StringConverter::toString(ID));
	elem->addAttribute("followersCount", core::StringConverter::toString(followersCount));
	elem->addAttribute("imageUrl", "imageUrl");
	elem->addAttribute("imageUrlHi", "imageUrlHi");
	elem->addAttribute("name", ConvertToUtf8(name));
	elem->addAttribute("displayName", ConvertToUtf8(displayName));
	elem->addAttribute("place", ConvertToUtf8(place));
	elem->addAttribute("URL", ConvertToUtf8(URL));
}


}
}

