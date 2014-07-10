

#ifndef TwitterProvider_h__
#define TwitterProvider_h__

#include "DataTypes.h"

namespace mray
{
namespace ted
{

	class TwitterTweet;
	class TwitterUserProfile;

	class TwitterProviderImpl;

	class ITwitterProviderListener
	{
	public:
		virtual void OnTweetsLoaded(const std::vector<TwitterTweet*>& Tweets){}
	};

class TwitterProvider
{
protected:
	TwitterProviderImpl* m_impl;
public:
	TwitterProvider();
	virtual~TwitterProvider();

	TwitterUserProfile* GetUserByName(const core::stringw& name);
	TwitterUserProfile* GetUserByID(IDType id);
	TwitterTweet* GetTweetByID(IDType ID);
	void GetTweetsSynced(const core::stringw& keyword, IDType since, uint count,
		std::vector<TwitterTweet*>& Tweets) ;

	void GetTweetsAsynced(const core::stringw& keyword, IDType since, uint count,
		ITwitterProviderListener* callback);


	bool IsAuthorized();

};

}
}

#endif // TwitterProvider_h__
