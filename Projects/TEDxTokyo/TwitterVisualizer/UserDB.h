

/********************************************************************
	created:	2014/04/09
	created:	9:4:2014   21:42
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\UserD.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	UserD
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __UserD__
#define __UserD__


class SACommand;

namespace mray
{
namespace ted
{

class TweetDB;

class UserDB
{
protected:
public:
	static std::map<ulong, uint> UserIDMap;
	static std::vector<UserDB*> UserDBList;
	static void AddUserDB(UserDB* t);
	static UserDB* GetUserByID(ulong id);
public:

	long ID;
	core::stringw name;
	core::stringw displayName;
	core::string imageUrl;
	core::string imageUrlHi;
	int followersCount;
	core::stringw place;
	core::stringw URL;

	std::vector<TweetDB*> tweets;

	UserDB() :ID(0), followersCount(0){}
	virtual~UserDB(){}

	void LoadUser(SACommand& cmd);

};

}
}


#endif
