

#include "stdafx.h"
#include "SQLDBHandler.h"

#include "DataTypes.h"
#include "mySqlManager.h"
#include "ISQLResult.h"
#include "KMDComment.h"
#include "KMDUser.h"
#include "StringUtil.h"
#include "SessionDetails.h"
#include "AppData.h"

namespace mray
{
namespace kmd
{


void SQLDBHandler::_LoadComments()
{

	try
	{

		db::ISQLResult* res= m_connection->ExecBlockingCmdWithRet("select  id,Publisher pub,Project p,Text txt,Time tm from "+m_tb,false);
		if (!res)
			return;
		
		int rows = 0;
		while (res->NextRow())
		{
			++rows;
			KMDComment *t = new KMDComment();
			LoadComment(res,t);;
			if (t)
				KMDComment::AddKMDComment(t);
		}
		printf("Comment Rows:%d/%d\n", KMDComment::KMDCommentList.size(), rows);
	}
	catch (std::exception& e)
	{
		printf("%s\n", e.what());

	}
}

SQLDBHandler::SQLDBHandler()
{
	m_sqlManager = new db::mySqlManager();
	m_connection=m_sqlManager->CreateConnection();

}
SQLDBHandler::~SQLDBHandler()
{
	m_sqlManager->ClearConnections();
	m_connection=0;
	delete m_sqlManager;
}
bool SQLDBHandler::_connect()
{
	if (IsConnected())
		return true;;
	try
	{
		core::string host = gAppData.GetValue("DB", "Host");
		m_db = gAppData.GetValue("DB", "DB");
		m_tb = gAppData.GetValue("DB", "Table");
		core::string user = gAppData.GetValue("DB", "User");
		core::string pwd = gAppData.GetValue("DB", "PWD");
		m_connection->Connect(host, user, pwd, m_db, 3306, 0);

		return m_connection->IsConnected();
	}
	catch (std::exception& e)
	{
		printf("Failed to connect to DB:%s\n", e.what());
	}
	return false;
}
void SQLDBHandler::_disconnect()
{
	if (!IsConnected())
		return ;;
	try
	{
		m_connection->Disconnect();

	}
	catch (std::exception& e)
	{
		printf("Failed to disconnect to DB:%s\n", e.what());
	}
}

void SQLDBHandler::LoadDB()
{
	if (!_connect())
	{
		return;
	}
	//_LoadComments();
}
bool SQLDBHandler::IsConnected()
{
	return m_connection->IsConnected();
}



KMDComment* SQLDBHandler::RequestComment(IDType id)
{

	try
	{
		core::string cstr = "select id,Publisher pub,Project p,Text txt,Time tm from " + m_tb+" where id=" + core::StringConverter::toString(id);

		db::ISQLResult* res= m_connection->ExecBlockingCmdWithRet(cstr, false);

		if (!res)
			return 0;

		KMDComment* c=new KMDComment();
		LoadComment(res, c);
		return c;
	}
	catch (std::exception& e)
	{
	}
	return 0;
}
std::vector<KMDComment*> SQLDBHandler::LoadComments(IDType sinceID)
{
	std::vector<KMDComment*> ret;

	try
	{
		core::string cstr = "select id,Publisher pub,Project p,Text txt,Time tm from " + m_tb + " where id>" + core::StringConverter::toString(sinceID);

		db::ISQLResult* res = m_connection->ExecBlockingCmdWithRet(cstr, false);


		if (!res)
			return ret;
		KMDComment *t = 0;
		while (res->NextRow())
		{
			if (!t)
				t = new KMDComment();
			LoadComment(res, t);;
			if (t->project)
			{
				KMDComment::AddKMDComment(t);
				ret.push_back(t);
				t = 0;
			}
		}
		if (t)
			delete t;
		return ret;
	}
	catch (std::exception& e)
	{
	}
	return ret;
}

void ParseDBDate(const core::string& str, core::DateTime& dt)
{
	std::vector<core::string> splits1=core::StringUtil::Split(str, " ");

	std::vector<core::string> dateSplits = core::StringUtil::Split(splits1[0], "-");
	std::vector<core::string> timeSplits = core::StringUtil::Split(splits1[1], ":");

	dt.SetDate(core::StringConverter::toInt(dateSplits[0]),
		core::StringConverter::toInt(dateSplits[1]),
		core::StringConverter::toInt(dateSplits[2]));

	dt.SetTime(core::StringConverter::toInt(timeSplits[0]),
		core::StringConverter::toInt(timeSplits[1]),
		core::StringConverter::toInt(timeSplits[2]));

}

void SQLDBHandler::LoadComment(db::ISQLResult* res, KMDComment* c)
{
	c->ID = core::StringConverter::toUInt((*res)[0]);
	c->user = KMDUser::GetUserByName((*res)[1]);
	c->project = SessionDetails::GetSession(core::StringUtil::Trim((*res)[2], " "));
	if (c->project)
		c->project->AddComment(c);
	c->text = ConvertToUtf16((*res)[3]);
	c->text.replaceChar('\n', ' ');
	ParseDBDate((*res)[4], c->date);
}

}
}
