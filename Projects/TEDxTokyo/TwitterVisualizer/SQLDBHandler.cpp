

#include "stdafx.h"
#include "SQLDBHandler.h"
#include "TwitterTweet.h"
#include "TwitterUserProfile.h"

#include "DataTypes.h"


namespace mray
{
namespace ted
{



void SQLDBHandler::_LoadUsersDB()
{

	try
	{
		SACommand cmd;
		cmd.setConnection(&m_connection);
		cmd.setCommandText("select * from dbo.UsersTB");
		cmd.Execute();


		while (cmd.FetchNext())
		{
			ted::TwitterUserProfile *t = new ted::TwitterUserProfile();;
			LoadUser(cmd,*t);
			TwitterUserProfile::AddTwitterUserProfile(t);
		}
	}
	catch (SAException& e)
	{
		printf("%s\n", e.ErrText().GetMultiByteChars());
	}
}
void SQLDBHandler::_LoadTwitterTweet()
{

	try
	{
		SACommand cmd;
		cmd.setConnection(&m_connection);
		cmd.setCommandText("select * from dbo.TweetsDB");
		cmd.Execute();
		
		int rows = 0;
		while (cmd.FetchNext())
		{
			++rows;
			ted::TwitterTweet *t = new ted::TwitterTweet();
			LoadTweet(cmd,*t);;
			if (t)
				TwitterTweet::AddTwitterTweet(t);
		}
		printf("Tweet Rows:%d/%d\n", TwitterTweet::TwitterTweetList.size(), rows);
	}
	catch (SAException& e)
	{
		printf("%s\n", e.ErrText().GetMultiByteChars());

	}
}

SQLDBHandler::SQLDBHandler()
{
}
bool SQLDBHandler::_connect()
{
	if (IsConnected())
		return true;;
	try
	{
		m_connection.setClient(SA_SQLServer_Client);
		m_connection.Connect(".\\SQLEXPRESS@TwitterDB", "TweetU2", "TweetP", SA_SQLServer_Client);
		return m_connection.isConnected();
	}
	catch (SAException& e)
	{
		printf("Failed to connect to DB:%s\n", e.ErrText().GetMultiByteChars());
	}
	return false;
}
SQLDBHandler::~SQLDBHandler()
{
}

void SQLDBHandler::LoadDB()
{
	if (!_connect())
	{
		return;
	}
	_LoadUsersDB();
	_LoadTwitterTweet();
}
bool SQLDBHandler::IsConnected()
{
	return m_connection.isConnected();
}



TwitterTweet* SQLDBHandler::RequestTweet(IDType id)
{

	try
	{
		core::string cstr = "select * from TweetsDB where id=" + core::StringConverter::toString(id);

		SACommand cmd;
		cmd.setConnection(&m_connection);
		cmd.setCommandText(cstr.c_str());
		cmd.Execute();


		while (cmd.FetchNext())
		{
			ted::TwitterTweet *t = new ted::TwitterTweet();;
			LoadTweet(cmd,*t);
			TwitterTweet::AddTwitterTweet(t);
			return t;
		}
	}
	catch (SAException& e)
	{

	}
	return 0;
}
TwitterUserProfile* SQLDBHandler::RequestUser(IDType id)
{
	try
	{
		core::string cstr = "select * from UsersDB where id=" + core::StringConverter::toString(id);

		SACommand cmd;
		cmd.setConnection(&m_connection);
		cmd.setCommandText(cstr.c_str());
		cmd.Execute();


		while (cmd.FetchNext())
		{
			ted::TwitterUserProfile *t = new ted::TwitterUserProfile();;
			LoadUser(cmd,*t);
			TwitterUserProfile::AddTwitterUserProfile(t);
			return t;
		}
	}
	catch (SAException& e)
	{

	}
	return 0;
}

void SQLDBHandler::LoadUser(SACommand& cmd, TwitterUserProfile& user)
{
	SAString n;
	SAString p;
	SAString u;
	SAString dn;
	SAString img;
	n = cmd.Field("Name").asString();
	dn = cmd.Field("DisplayName").asString();
	u = cmd.Field("URL").asString();
	p = cmd.Field("Place").asString();
	img = cmd.Field("ImageURL").asString();

	user.ID = cmd.Field("ID").asULong();
	user.name = ted::ConvertToStringW(n.GetMultiByteChars(), n.GetMultiByteCharsLength());
	user.displayName = ted::ConvertToStringW(dn.GetMultiByteChars(), dn.GetMultiByteCharsLength());
	user.URL = ted::ConvertToStringW(u.GetMultiByteChars(), u.GetMultiByteCharsLength());
	user.imageUrl = img.GetMultiByteChars();
	user.place = ted::ConvertToStringW(p.GetMultiByteChars(), p.GetMultiByteCharsLength());
	user.followersCount = cmd.Field("FollowersCount").asLong();

	int i = user.imageUrl.find("_normal");
	if (i != -1)
	{
		user.imageUrlHi = user.imageUrl.substr(0, i) + user.imageUrl.substr(i + 7, user.imageUrl.size() - (i + 7));
	}
}

void SQLDBHandler::LoadTweet(SACommand& cmd, TwitterTweet& tw)
{
	SAString t;
	t = cmd.Field("Text").asString();
	SADateTime d;
	tw.ID = cmd.Field("ID").asULong();
	ulong userid = cmd.Field("UserID").asULong();
	d = cmd.Field("Date");
	tw.text = ted::ConvertToStringW(t.GetMultiByteChars(), t.GetMultiByteCharsLength());
	tw.date.SetYear(d.GetYear());
	tw.date.SetMonth( d.GetMonth());
	tw.date.SetDay(d.GetDay());
	tw.retweets = cmd.Field("Retweets").asLong();
	tw.user = TwitterUserProfile::GetUserByID(userid,true);
	if (tw.user)
		tw.user->tweets.push_back(&tw);
}

}
}
