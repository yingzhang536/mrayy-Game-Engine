

/********************************************************************
	created:	2014/04/09
	created:	9:4:2014   21:44
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\DBHandler.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	DBHandler
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __DBHandler__
#define __DBHandler__



namespace mray
{
namespace ted
{
	class TweetDB;
	class UserDB;

class DBHandler
{
protected:
	static DBHandler* s_instance;

	SAConnection m_connection;

	void _LoadUsersDB();
	void _LoadTweetDB();

	bool _connect();
public:
	DBHandler();
	virtual~DBHandler();

	void LoadDB();

	bool IsConnected();

	TweetDB* RequestTweet(ulong id);
	UserDB* RequestUser(ulong id);

	static DBHandler* Instance();
};

}
}


#endif
