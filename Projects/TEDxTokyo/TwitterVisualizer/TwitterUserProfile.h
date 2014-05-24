

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

#include "DataTypes.h"

class SACommand;

namespace mray
{
namespace ted
{

class TwitterTweet;

class TwitterUserProfile
{
protected:
public:
	static std::map<IDType, uint> UserIDMap;
	static std::vector<TwitterUserProfile*> TwitterUserProfileList;
	static void AddTwitterUserProfile(TwitterUserProfile* t);
	static TwitterUserProfile* GetUserByID(IDType id,bool localOnly);
public:

	IDType ID;
	core::stringw name;
	core::stringw displayName;
	core::string imageUrl;
	core::string imageUrlHi;
//	core::string imageUrlLow;
	int followersCount;
	core::stringw place;
	core::stringw URL;

	std::vector<TwitterTweet*> tweets;
	std::vector<TwitterTweet*> replies;

	TwitterUserProfile() :ID(0), followersCount(0){}
	virtual~TwitterUserProfile(){}

	void LoadXML(xml::XMLElement* e);
	void SaveXML(xml::XMLElement* e);

};

}
}


#endif
