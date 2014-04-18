

/********************************************************************
	created:	2014/03/31
	created:	31:3:2014   11:32
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\TweetDB.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	TweetDB
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __TweetDB__
#define __TweetDB__


#include "DataTypes.h"

class SACommand;
namespace mray
{
namespace ted
{
	class UserDB;

class TweetDB
{
protected:
public:
	static std::map<ulong, uint> TweetIDMap;
	static std::vector<TweetDB*> TweetDBList;
	static void AddTweetDB(TweetDB* t);
	static TweetDB* GetTweetByID(ulong id);

	void _innerLoad(SACommand& cmd);
public:
	TweetDB() :ID(0), user(0), retweets(0)
	{}
	virtual~TweetDB(){}

	ulong ID;
	UserDB* user;
	core::UTFString text;
	core::DateTime date;
	int retweets;

	static TweetDB* LoadTweet(SACommand& cmd);
};

}
}


#endif
