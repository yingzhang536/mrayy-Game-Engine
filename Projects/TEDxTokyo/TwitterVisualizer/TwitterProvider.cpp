
#include "stdafx.h"
#include "TwitterProvider.h"

#include "TwitterTweet.h"
#include "TwitterUserProfile.h"

#include "TwitterService.h"
#include "AppData.h"

namespace mray
{
namespace ted
{



class TwitterProviderImpl
{
public:

	Twitter::TwitterService* service;
	bool authorized;

	TwitterProviderImpl()
	{
		authorized = false;
		service = new Twitter::TwitterService();

		service->Init(	AppData::getInstance().GetValue("Twitter", "ConsumerKey"),
						AppData::getInstance().GetValue("Twitter", "ConsumerSecret"));

		if (!service->AuthenticateWith(AppData::getInstance().GetValue("Twitter", "Token"),
			AppData::getInstance().GetValue("Twitter", "TokenSecret")))
		{
			gLogManager.log("Twitter Authentication failed!", ELL_WARNING);
		}else
			authorized=true;
	}

	void Parse(const Twitter::User& in, TwitterUserProfile& out)
	{
		out.ID = in.id;
		out.displayName = ConvertToUtf16(in.screen_name);
		out.name = ConvertToUtf16(in.name);
		out.place = ConvertToUtf16(in.location);
		out.followersCount = in.followers_count;
		out.URL = ConvertToUtf16(in.url); 
		out.imageUrl = in.profile_image_url;

		int i = out.imageUrl.find("_normal");
		if (i != -1)
		{
			out.imageUrlHi = out.imageUrl.substr(0, i) +"_small"+ out.imageUrl.substr(i + 7, out.imageUrl.size() - (i + 7));
		//	out.imageUrlLow = out.imageUrl.substr(0, i) + out.imageUrl.substr(i + 7, out.imageUrl.size() - (i + 7));
		}
	}
	void Parse(const Twitter::Tweet& in, TwitterTweet& out)
	{
		out.ID = in.id;
		out.date = in.created_at.GetDate();
		out.retweets = in.retweet_count;
		out.text = ConvertToUtf16(in.text);
		out.user = TwitterUserProfile::GetUserByID(in.user.id, true);
		if (!out.user)
		{
			out.user = new TwitterUserProfile();
			Parse(in.user, *out.user);
			TwitterUserProfile::AddTwitterUserProfile(out.user);
		}
		for (int i = 0; i < in.entities.hashTags.size(); ++i)
			out.entities.hashTags.push_back(ConvertToUtf16(in.entities.hashTags[i]));

		for (int i = 0; i < in.entities.urls.size(); ++i)
			out.entities.urls.push_back(in.entities.urls[i]);

		for (int i = 0; i < in.entities.user_mentions.size(); ++i)
		{
			TwitterUserProfile* u = TwitterUserProfile::GetUserByID(in.entities.user_mentions[i].id, true);
			if (u)
			{
				out.entities.user_mentions.push_back(u);
			}
			else
			{
			}

		}
	}
	void ResolveID(const Twitter::Tweet& in, TwitterTweet& out)
	{

		if (in.in_reply_to_user_id != 0)
			out.replyToUser = TwitterUserProfile::GetUserByID(in.in_reply_to_user_id, false);
		if (in.in_reply_to_status_id != 0)
			out.replyToTweet = TwitterTweet::GetTweetByID(in.in_reply_to_status_id, false);
	}

};


TwitterProvider::TwitterProvider()
{
	m_impl = new TwitterProviderImpl;
}
TwitterProvider::~TwitterProvider()
{
	delete m_impl;
}

TwitterUserProfile* TwitterProvider::GetUserByName(const core::stringw& name)
{

	return 0;
}
TwitterUserProfile* TwitterProvider::GetUserByID(IDType id)
{

	return 0;
}
TwitterTweet* TwitterProvider::GetTweetByID(uint ID)
{
	return 0;

}

bool TwitterProvider::IsAuthorized()
{
	return m_impl->authorized;
}

void TwitterProvider::GetTweets(const core::stringw& keyword, uint since, uint count, std::vector<TwitterTweet*>& tweets)
{
	Twitter::TweetSearchResult res;
	Twitter::TwitterSearchOptions op;
	op.Q = ConvertToUtf8(keyword);
	op.count = count;
	op.sinceID = since;
	if (!m_impl->service->Search(op, res))
		return;

	tweets.reserve(res.tweets.size());
	for (int i = 0; i < res.tweets.size();++i)
	{
		TwitterTweet* t = new TwitterTweet();
		m_impl->Parse(res.tweets[i], *t);
		TwitterTweet::AddTwitterTweet(t);
		tweets.push_back(t);
	}
	//resolve IDs
	for (int i = 0; i < res.tweets.size(); ++i)
	{
		m_impl->ResolveID(res.tweets[i], *tweets[i]);
	}
}

}
}
