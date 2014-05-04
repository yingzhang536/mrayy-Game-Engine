


#ifndef TwitterDataStructure_h__
#define TwitterDataStructure_h__

#include <vector>
#include "DateTime.h"

namespace mray
{
namespace Twitter
{

	typedef uint IDType;

	class User
	{
	public:
		User() :id(0), statuses_count(0), favourites_count(0), friends_count(0), followers_count(0)
		{
		}
		IDType id;

		core::string name;
		core::string screen_name;

		core::DateTime created_at;
		core::string description;
		core::string url;

		uint statuses_count;
		uint favourites_count;
		uint friends_count;
		uint followers_count;

		core::string location;
		core::string profile_image_url;

	};

class Tweet
{
public:


	Tweet() :id(0), in_reply_to_status_id(0), in_reply_to_user_id(0), favorite_count(0), retweet_count(0)
	{

	}

	IDType id;

	core::DateTime created_at;
	uint favorite_count;

	core::string in_reply_to_screen_name;
	IDType in_reply_to_status_id;
	IDType in_reply_to_user_id;

	core::string lang;

	uint retweet_count;

	core::string text;

	User user;

	struct UserMention
	{
		IDType id;
		core::string name;
		core::string screen_name;
	};

	struct Entities
	{
		std::vector<core::string> hashTags;
		std::vector<core::string> urls;
		std::vector<UserMention> user_mentions;
	}entities;
};


class TweetSearchResult
{
public:

	uint max_id;
	std::vector<Tweet> tweets;
};


class UserSearchResult
{
public:

	User u;
};


}
}

#endif // TwitterDataStructure_h__
