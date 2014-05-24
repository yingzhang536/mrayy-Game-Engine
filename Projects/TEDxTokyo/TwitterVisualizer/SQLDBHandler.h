

/********************************************************************
	created:	2014/04/09
	created:	9:4:2014   21:44
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\SQLDBHandler.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	SQLDBHandler
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __SQLDBHandler__
#define __SQLDBHandler__

#include "IDBHandler.h"

namespace mray
{
namespace ted
{

class SQLDBHandler :public IDBHandler
{
protected:
	SAConnection m_connection;

	void _LoadUsersDB();
	void _LoadTwitterTweet();

	bool _connect();


	void LoadUser(SACommand& cmd, TwitterUserProfile& u);
	void LoadTweet(SACommand& cmd, TwitterTweet& t);
public:
	SQLDBHandler();
	virtual~SQLDBHandler();

	void LoadDB();

	bool IsConnected();

	virtual TwitterTweet* RequestTweet(IDType id);
	virtual TwitterUserProfile* RequestUser(IDType id);;


};

}
}


#endif
