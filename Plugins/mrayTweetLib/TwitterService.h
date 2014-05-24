

#ifndef TwitterService_h__
#define TwitterService_h__

#include "mString.h"
#include "TwitterOptions.h"
#include "TwitterDataStructure.h"

namespace mray
{
namespace Twitter
{
struct TwitterServiceImpl;

class TwitterService
{
protected:
	TwitterServiceImpl* m_impl;
public:
	TwitterService();
	virtual~TwitterService();

	bool Init(const core::string& consumerKey, const core::string& consumerSecret);
	bool AuthenticateWith(const core::string& token, const core::string& tokenSecret);

	bool Search(const TwitterSearchOptions& op, TweetSearchResult& tweets);
	bool GetTweet(const GetTweetOptions& op, Tweet& t);
	bool SearchForUser(const UserSearchOptions& op, UserSearchResult& u);

};


}
}

#endif // TwitterService_h__
