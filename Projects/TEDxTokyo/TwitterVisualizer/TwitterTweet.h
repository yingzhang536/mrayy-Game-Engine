

/********************************************************************
	created:	2014/03/31
	created:	31:3:2014   11:32
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\TwitterTweet.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	TwitterTweet
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __TwitterTweet__
#define __TwitterTweet__


#include "DataTypes.h"
#include "DateTime.h"

class SACommand;
namespace mray
{
namespace ted
{
	class TwitterUserProfile;

class TwitterTweet
{
protected:
public:
	static std::map<IDType, uint> TweetIDMap;
	static std::vector<TwitterTweet*> TwitterTweetList;
	static void AddTwitterTweet(TwitterTweet* t);
	static TwitterTweet* GetTweetByID(IDType id,bool localOnly);

public:
	TwitterTweet() :ID(0), user(0), reTweets(0), replyToTweet(0), replyToUser(0)
	{}
	virtual~TwitterTweet(){}

	IDType ID;
	TwitterUserProfile* user;
	core::stringw text;
	core::DateTime date;
	int reTweets;

	TwitterTweet* replyToTweet;
	TwitterUserProfile* replyToUser;

	std::vector<TwitterTweet*> replies;

	struct Entities
	{
		std::vector<core::stringw> hashTags;
		std::vector<core::string> urls;
		std::vector<TwitterUserProfile*> user_mentions;
	}entities;

	bool HasHashtag(const core::stringw& tag)
	{
		for (int i = 0; i < entities.hashTags.size(); ++i)
		{
			if (entities.hashTags[i].equals_ignore_case(tag))
				return true;
		}
		return false;
	}

	bool HasUserMention(const core::stringw& name);

	void LoadXML(xml::XMLElement* e);
	void SaveXML(xml::XMLElement* e);
};

}
}


#endif
