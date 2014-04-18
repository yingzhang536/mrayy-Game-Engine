

#include "stdafx.h"
#include "DBHandler.h"
#include "TweetDB.h"
#include "UserDB.h"



namespace mray
{
namespace ted
{

DBHandler* DBHandler::s_instance=0;


void DBHandler::_LoadUsersDB()
{

	try
	{
		SACommand cmd;
		cmd.setConnection(&m_connection);
		cmd.setCommandText("select * from dbo.UsersTB");
		cmd.Execute();


		while (cmd.FetchNext())
		{
			ted::UserDB *t = new ted::UserDB();;
			t->LoadUser(cmd);
			UserDB::AddUserDB(t);
		}
	}
	catch (SAException& e)
	{
		printf("%s\n", e.ErrText().GetMultiByteChars());
	}
}
void DBHandler::_LoadTweetDB()
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
			ted::TweetDB *t = ted::TweetDB::LoadTweet(cmd);;
			if (t)
				TweetDB::AddTweetDB(t);
		}
		printf("Tweet Rows:%d/%d\n", TweetDB::TweetDBList.size(), rows);
	}
	catch (SAException& e)
	{
		printf("%s\n", e.ErrText().GetMultiByteChars());

	}
}

DBHandler::DBHandler()
{
}
bool DBHandler::_connect()
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
DBHandler::~DBHandler()
{
}

void DBHandler::LoadDB()
{
	if (!_connect())
	{
		return;
	}
	_LoadUsersDB();
	_LoadTweetDB();
}
bool DBHandler::IsConnected()
{
	return m_connection.isConnected();
}



TweetDB* DBHandler::RequestTweet(ulong id)
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
			ted::TweetDB *t = new ted::TweetDB();;
			t->LoadTweet(cmd);
			TweetDB::AddTweetDB(t);
			return t;
		}
	}
	catch (SAException& e)
	{

	}
	return 0;
}
UserDB* DBHandler::RequestUser(ulong id)
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
			ted::UserDB *t = new ted::UserDB();;
			t->LoadUser(cmd);
			UserDB::AddUserDB(t);
			return t;
		}
	}
	catch (SAException& e)
	{

	}
	return 0;
}

DBHandler* DBHandler::Instance()
{
	if (!s_instance)
		s_instance = new DBHandler();
	return s_instance;
};

}
}
