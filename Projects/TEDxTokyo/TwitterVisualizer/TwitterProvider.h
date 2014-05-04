

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

class TwitterProvider
{
protected:
	TwitterProviderImpl* m_impl;
public:
	TwitterProvider();
	virtual~TwitterProvider();

	TwitterUserProfile* GetUserByName(const core::stringw& name);
	TwitterUserProfile* GetUserByID(IDType id);
	TwitterTweet* GetTweetByID(uint ID);
	void GetTweets(const core::stringw& keyword, uint since,uint count,
		std::vector<TwitterTweet*>& tweets) ;

	bool IsAuthorized();

};

}
}

#endif // TwitterProvider_h__
