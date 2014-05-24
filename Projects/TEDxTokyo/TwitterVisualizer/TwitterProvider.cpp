
#include "stdafx.h"
#include "TwitterProvider.h"

#include "TwitterTweet.h"
#include "TwitterUserProfile.h"

#include "TwitterService.h"
#include "AppData.h"

#include "CMutexVar.h"
#include "IThreadFunction.h"
#include "IThread.h"
#include "IThreadManager.h"

namespace mray
{
namespace ted
{


	class GetTweetsAsyncedData
	{
	public:
		core::stringw keyword;
		uint since;
		uint count;
		ITwitterProviderListener* callback;
	};


class TwitterProviderImpl:public OS::IThreadFunction
{
public:

	Twitter::TwitterService* service;
	bool authorized;
	typedef std::list<GetTweetsAsyncedData> TweetsAsyncList;
	OS::CMutexVar<TweetsAsyncList> tweetsReq;
	OS::IThreadPtr asyncThread;

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

		asyncThread = OS::IThreadManager::getInstance().createThread(this);
		asyncThread->start(0);
		tweetsReq.setMutex(OS::IThreadManager::getInstance().createMutex());
	}

	~TwitterProviderImpl()
	{
		asyncThread->terminate();
		OS::IThreadManager::getInstance().killThread(asyncThread);
		asyncThread = 0;
	}
	virtual void execute(OS::IThread*caller, void*arg)
	{
		while (caller->isActive())
		{
			if (tweetsReq().size() == 0)
			{
				Sleep(50);
				continue;;
			}
			GetTweetsAsyncedData d;
			tweetsReq.lock();
			d=*tweetsReq().begin();
			tweetsReq().erase(tweetsReq().begin());
			tweetsReq.unlock();

			std::vector<TwitterTweet*> tweets;
			this->GetTweetsSynced(d.keyword, d.since, d.count, tweets);
			d.callback->OnTweetsLoaded(tweets);

		}
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
			out.imageUrlHi = out.imageUrl.substr(0, i) +"_bigger"+ out.imageUrl.substr(i + 7, out.imageUrl.size() - (i + 7));
		//	out.imageUrlLow = out.imageUrl.substr(0, i) + out.imageUrl.substr(i + 7, out.imageUrl.size() - (i + 7));
		}
	}
	void Parse(const Twitter::Tweet& in, TwitterTweet& out)
	{
		out.ID = in.id;
		out.date.SetDate(in.created_at.GetDate());
		out.date.SetTime(in.created_at.GetTime());
		out.retweets = in.retweet_count;
		out.text = ConvertToUtf16(in.text);
		out.user = TwitterUserProfile::GetUserByID(in.user.id, true);
		if (!out.user)
		{
			out.user = new TwitterUserProfile();
			Parse(in.user, *out.user);
			TwitterUserProfile::AddTwitterUserProfile(out.user);
			out.user->tweets.push_back(&out);
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
		{
			out.replyToUser = TwitterUserProfile::GetUserByID(in.in_reply_to_user_id, false);
			if (out.replyToUser)
				out.replyToUser->replies.push_back(&out);
		}
		if (in.in_reply_to_status_id != 0)
		{
			out.replyToTweet = TwitterTweet::GetTweetByID(in.in_reply_to_status_id, false);

			if (out.replyToTweet)
				out.replyToTweet->replies.push_back(&out);
		}
	}

	void GetTweetsSynced(const core::stringw& keyword, IDType since, uint count, std::vector<TwitterTweet*>& tweets)
	{
		Twitter::TweetSearchResult res;
		Twitter::TwitterSearchOptions op;
		op.Q = ConvertToUtf8(keyword);
		op.count = count;
		op.sinceID = since;
		if (!service->Search(op, res))
			return;

		tweets.reserve(res.tweets.size());
		for (int i = 0; i < res.tweets.size(); ++i)
		{
			TwitterTweet* t = new TwitterTweet();
			Parse(res.tweets[i], *t);
			TwitterTweet::AddTwitterTweet(t);
			tweets.push_back(t);
		}
		//resolve IDs
		for (int i = 0; i < res.tweets.size(); ++i)
		{
			ResolveID(res.tweets[i], *tweets[i]);
		}
	}
	void GetTweetsAsynced(const core::stringw& keyword, IDType since, uint count, ITwitterProviderListener* callback)
	{
		GetTweetsAsyncedData d;
		d.keyword = keyword;
		d.since = since;
		d.count = count;
		d.callback = callback;
		tweetsReq.lock();
		tweetsReq().push_back(d);
		tweetsReq.unlock();
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
TwitterTweet* TwitterProvider::GetTweetByID(IDType ID)
{
	return 0;

}

bool TwitterProvider::IsAuthorized()
{
	return m_impl->authorized;
}

void TwitterProvider::GetTweetsSynced(const core::stringw& keyword, IDType since, uint count, std::vector<TwitterTweet*>& tweets)
{
	m_impl->GetTweetsSynced(keyword, since, count, tweets);
}

void TwitterProvider::GetTweetsAsynced(const core::stringw& keyword, IDType since, uint count, ITwitterProviderListener* callback)
{
	m_impl->GetTweetsAsynced(keyword, since, count, callback);
}

}
}
