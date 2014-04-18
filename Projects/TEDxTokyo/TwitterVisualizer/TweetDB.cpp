
#include "stdafx.h"
#include "TweetDB.h"

#include "DBHandler.h"
#include "SQLAPI.h"
#include "UserDB.h"
#include "StringUtil.h"

namespace mray
{
namespace ted
{
class UserD;


std::map<ulong, uint> TweetDB::TweetIDMap;
std::vector<TweetDB*> TweetDB::TweetDBList;

void TweetDB::AddTweetDB(TweetDB* t)
{
	TweetDBList.push_back(t);
	TweetIDMap[t->ID] = TweetDBList.size() - 1;
}
TweetDB* TweetDB::GetTweetByID(ulong id)
{
	std::map<ulong, uint>::iterator it = TweetIDMap.find(id);
	if (it == TweetIDMap.end())
	{
		TweetDB* u = DBHandler::Instance()->RequestTweet(id);
		if (u)
			AddTweetDB(u);
		return u;
	}
	return TweetDBList[it->second];

}

TweetDB* TweetDB::LoadTweet(SACommand& cmd)
{
	/*
	SAString t;
	t = cmd.Field("Text").asString();
	core::stringw text = core::ConvertToStringW(t.GetMultiByteChars(), t.GetMultiByteCharsLength());

	text=core::StringUtilW::ToLower(text);
	if (text.find(L"tedxtokyo")==-1)
	{
		return 0;
	}*/

	TweetDB* ret = new TweetDB();
	ret->_innerLoad(cmd);
	return ret;
}
void TweetDB::_innerLoad(SACommand& cmd)
{
	SAString t;
	t = cmd.Field("Text").asString();
	SADateTime d;
	ID = cmd.Field("ID").asULong();
	ulong userid = cmd.Field("UserID").asULong();
	d = cmd.Field("Date");
	text = core::ConvertToStringW(t.GetMultiByteChars(), t.GetMultiByteCharsLength());
	date.year=d.GetYear();
	date.month = d.GetMonth();
	date.day = d.GetDay();
	retweets = cmd.Field("Retweets").asLong();
	user = UserDB::GetUserByID(userid);
	if (user)
		user->tweets.push_back(this);
}


}
}

