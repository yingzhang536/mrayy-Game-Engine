
#include "stdafx.h"
#include "UserDB.h"

#include "SQLAPI.h"
#include "DataTypes.h"
#include "DBHandler.h"

namespace mray
{
namespace ted
{
class UserD;


std::map<ulong, uint> UserDB::UserIDMap;
std::vector<UserDB*> UserDB::UserDBList;

void UserDB::AddUserDB(UserDB* t)
{
	UserDBList.push_back(t);
	UserIDMap[t->ID] = UserDBList.size() - 1;
}
UserDB* UserDB::GetUserByID(ulong id)
{
	std::map<ulong, uint>::iterator it=  UserIDMap.find(id);
	if (it == UserIDMap.end())
	{
		UserDB* u= DBHandler::Instance()->RequestUser(id);
		if (u)
			AddUserDB(u);
		return u;
	}
	return UserDBList[it->second];
}

void UserDB::LoadUser(SACommand& cmd)
{
	SAString n;
	SAString p;
	SAString u;
	SAString dn;
	SAString img;
	ID = cmd.Field("ID").asULong();
	n = cmd.Field("Name").asString();
	dn = cmd.Field("DisplayName").asString();
	u = cmd.Field("URL").asString();
	p = cmd.Field("Place").asString();
	img = cmd.Field("ImageURL").asString();

	ID = cmd.Field("ID").asULong();
	name = core::ConvertToStringW(n.GetMultiByteChars(), n.GetMultiByteCharsLength());
	displayName = core::ConvertToStringW(dn.GetMultiByteChars(), dn.GetMultiByteCharsLength());
	URL = core::ConvertToStringW(u.GetMultiByteChars(), u.GetMultiByteCharsLength());
	imageUrl = img.GetMultiByteChars();
	place = core::ConvertToStringW(p.GetMultiByteChars(), p.GetMultiByteCharsLength());
	followersCount = cmd.Field("FollowersCount").asLong();

	int i= imageUrl.find("_normal");
	if (i != -1)
	{
		imageUrlHi = imageUrl.substr(0, i) + imageUrl.substr(i + 7, imageUrl.size() - (i + 7));
	}
}


}
}

