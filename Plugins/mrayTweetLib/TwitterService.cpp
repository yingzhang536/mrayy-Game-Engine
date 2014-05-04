

#include "stdafx.h"
#include "TwitterService.h"

#include "ILogManager.h"

#include <twitcurl.h>
#include <exception>
#include "json/json.h"
#include "utf8/utf8.h"


namespace mray
{
namespace Twitter
{
	bool utf8_to_utf16(const core::string& in, core::stringw& ret)
	{
		ret = L"";
		utf8::utf8to16(in.begin(), in.end(), std::back_inserter(ret));
		return true;
	}
std::wstring utf8_to_utf162(const std::string& utf8)
	{
		std::vector<unsigned long> unicode;
		size_t i = 0;
		while (i < utf8.size())
		{
			unsigned long uni;
			size_t todo;
			bool error = false;
			unsigned char ch = utf8[i++];
			if (ch <= 0x7F)
			{
				uni = ch;
				todo = 0;
			}
			else if (ch <= 0xBF)
			{
				throw std::logic_error("not a UTF-8 string");
			}
			else if (ch <= 0xDF)
			{
				uni = ch & 0x1F;
				todo = 1;
			}
			else if (ch <= 0xEF)
			{
				uni = ch & 0x0F;
				todo = 2;
			}
			else if (ch <= 0xF7)
			{
				uni = ch & 0x07;
				todo = 3;
			}
			else
			{
				throw std::logic_error("not a UTF-8 string");
			}
			for (size_t j = 0; j < todo; ++j)
			{
				if (i == utf8.size())
					throw std::logic_error("not a UTF-8 string");
				unsigned char ch = utf8[i++];
				if (ch < 0x80 || ch > 0xBF)
					throw std::logic_error("not a UTF-8 string");
				uni <<= 6;
				uni += ch & 0x3F;
			}
			if (uni >= 0xD800 && uni <= 0xDFFF)
				throw std::logic_error("not a UTF-8 string");
			if (uni > 0x10FFFF)
				throw std::logic_error("not a UTF-8 string");
			unicode.push_back(uni);
		}
		std::wstring utf16;
		for (size_t i = 0; i < unicode.size(); ++i)
		{
			unsigned long uni = unicode[i];
			if (uni <= 0xFFFF)
			{
				utf16 += (wchar_t)uni;
			}
			else
			{
				uni -= 0x10000;
				utf16 += (wchar_t)((uni >> 10) + 0xD800);
				utf16 += (wchar_t)((uni & 0x3FF) + 0xDC00);
			}
		}
		return utf16;
	}

	core::stringw ConvertToStringW(const char* multibyte, int cnt)
	{
		if (cnt == -1)
			cnt = strlen(multibyte);
		wchar_t buffer[1024];
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, multibyte, cnt + 1, buffer, 1024);
		return buffer;

	}
struct TwitterServiceImpl
{
public:
	twitCurl tObj; //twitterObject

	std::string myOAuthAccessTokenKey;
	std::string myOAuthAccessTokenSecret;

	Json::Reader reader;

	TwitterServiceImpl()
	{
	}
};


TwitterService::TwitterService()
{
	m_impl = new TwitterServiceImpl;
}

TwitterService::~TwitterService()
{
	delete m_impl;
}


bool TwitterService::Init(const core::string& consumerKey, const core::string& consumerSecret)
{
	m_impl->tObj.getOAuth().setConsumerKey(consumerKey.c_str());
	m_impl->tObj.getOAuth().setConsumerSecret(consumerSecret.c_str());
	return true;
}

bool TwitterService::AuthenticateWith(const core::string& token, const core::string& tokenSecret)
{
	std::string err;

	m_impl->tObj.getOAuth().setOAuthTokenKey(token.c_str());
	m_impl->tObj.getOAuth().setOAuthTokenSecret(tokenSecret.c_str());

	if (!m_impl->tObj.oAuthAccessToken())
		return false;


	m_impl->tObj.getOAuth().getOAuthTokenKey(m_impl->myOAuthAccessTokenKey);
	m_impl->tObj.getOAuth().getOAuthTokenSecret(m_impl->myOAuthAccessTokenSecret);

	if (!m_impl->tObj.accountVerifyCredGet())
	{
		m_impl->tObj.getLastCurlError(err);
		gLogManager.log("TwitterService::AuthenticateWith() - " + err,ELL_WARNING);
		return false;
	}
	else
	{
		m_impl->tObj.getLastWebResponse(err);
		gLogManager.log("TwitterService::AuthenticateWith() - " + err, ELL_SUCCESS);
	}
	return true;
}



static bool parseUser(Json::Value &value, User& u)
{
	u.id = value["id"].asDouble();
	u.favourites_count = value["favourites_count"].asUInt();
	u.followers_count = value["followers_count"].asUInt();
	u.friends_count = value["friends_count"].asUInt();
	u.location = value["location"].asString();
	u.profile_image_url = value["profile_image_url"].asString();
//	utf8_to_utf16(value["name"].asCString(), u.name);
// 	utf8_to_utf16(value["screen_name"].asCString(), u.screen_name);
// 	utf8_to_utf16(value["description"].asCString(), u.description);
	u.screen_name = value["screen_name"].asCString();
	u.description = value["description"].asCString();
	u.name = value["name"].asCString();
	u.statuses_count = value["statuses_count"].asUInt();
	u.url = value["url"].asString();
	u.location = value["location"].asString();
	/*
	const char* created_At = value["created_at"].asCString();
	char cDay[5];
	char cMonth[5];
	memcpy(cDay, created_At, 3);
	cDay[4] = 0;
	memcpy(cMonth, created_At+4, 3);
	cDay[4] = 0;
	int d = 0, m = 0, y = 0, h = 0, mi = 0, sec = 0, gmt = 0;
	sscanf_s(created_At+9, "%d %d:%d:%d +%4d %d", d, h, mi, sec, gmt, y);
	*/
	return true;
}

static bool parseUserMention(Json::Value& value, Tweet::UserMention& u)
{

	u.id = value["id"].asDouble();
// 	utf8_to_utf16(value["name"].asCString(), u.name);
// 	utf8_to_utf16(value["screen_name"].asCString(), u.screen_name);

	u.name = value["name"].asCString();
	u.screen_name = value["screen_name"].asCString();
	return true;
}
static bool parseTweet(Json::Value &value, Tweet& tweet)
{
	tweet.id = value["id"].asDouble();
	//tweet.created_at = value["created_at"].as();
	tweet.in_reply_to_screen_name = value["in_reply_to_screen_name"].asString();
	tweet.in_reply_to_status_id = value["in_reply_to_status_id"].asDouble();
	tweet.in_reply_to_user_id = value["in_reply_to_user_id"].asDouble();
	//utf8_to_utf16(value["text"].asCString(), tweet.text);
	tweet.text = value["text"].asCString();
	parseUser(value["user"], tweet.user);

	//entities
	Json::Value& entities = value["entities"];
	Json::Value& hashtags = entities["hashtags"];
	Json::Value& urls = entities["urls"];
	Json::Value& user_mentions = entities["user_mentions"];
	core::stringw tmp;
	for (int i = 0; i < hashtags.size(); ++i)
	{
		//utf8_to_utf16(hashtags[i]["text"].asCString(), tmp);
		//tmp = hashtags[i]["text"].asCString();
		tweet.entities.hashTags.push_back(hashtags[i]["text"].asCString());
	}

	for (int i = 0; i < urls.size(); ++i)
	{
		//utf8_to_utf16(urls[i]["url"].asCString(), tmp);
		tweet.entities.urls.push_back(urls[i]["url"].asCString());
	}

	Tweet::UserMention um;
	for (int i = 0; i < user_mentions.size(); ++i)
	{
		parseUserMention(user_mentions[i], um);
		tweet.entities.user_mentions.push_back(um);
	}
	return true;
}
static bool
parseTweets(Json::Value &value, std::vector<Tweet>& tweets)
{
	if (value.type() != Json::arrayValue)
	{
		return false;
	}

	Tweet t;
	for (int i = 0; i < value.size();++i)
	{
		try
		{
			if (parseTweet(value[i], t))
				tweets.push_back(t);
		}
		catch (std::exception* e)
		{
		}
	}
	return true;
}


static void
printValueTree(Json::Value &value, const std::string &path = ".")
{
	switch (value.type())
	{
	case Json::nullValue:
		printf("%s=null\n", path.c_str());
		break;
	case Json::intValue:
		printf("%s=%d\n", path.c_str(), value.asInt());
		break;
	case Json::uintValue:
		printf("%s=%u\n", path.c_str(), value.asUInt());
		break;
	case Json::realValue:
		printf("%s=%.16g\n", path.c_str(), value.asDouble());
		break;
	case Json::stringValue:
		printf("%s=\"%s\"\n", path.c_str(), value.asString().c_str());
		break;
	case Json::booleanValue:
		printf("%s=%s\n", path.c_str(), value.asBool() ? "true" : "false");
		break;
	case Json::arrayValue:
	{
							 printf("%s=[]\n", path.c_str());
							 int size = value.size();
							 for (int index = 0; index < size; ++index)
							 {
								 static char buffer[16];
								 sprintf_s(buffer, "[%d]", index);
								 printValueTree(value[index], path + buffer);
							 }
	}
		break;
	case Json::objectValue:
	{
							  printf("%s={}\n", path.c_str());
							  Json::Value::Members members(value.getMemberNames());
							  std::sort(members.begin(), members.end());
							  std::string suffix = *(path.end() - 1) == '.' ? "" : ".";
							  for (Json::Value::Members::iterator it = members.begin();
								  it != members.end();
								  ++it)
							  {
								  const std::string &name = *it;
								  printValueTree(value[name], path + suffix + name);
							  }
	}
		break;
	default:
		break;
	}
}

bool TwitterService::Search(const TwitterSearchOptions& op, TweetSearchResult& tweets)
{
	core::string Q = op.Q;
	core::string count = "100";
	core::string sinceID = "";
	bool includeEnts = op.include_entities;

	if (Q.length() == 0)
		return false;
	if (Q[0] == '#')
	{
		includeEnts = true;
		Q = Q.substr(1, Q.length());
		if (Q.length() == 0)
			return false;
	}
	if (op.count != 0)
		count = core::StringConverter::toString(op.count);
	if (op.sinceID != 0)
		sinceID = core::StringConverter::toString(op.sinceID);
	if (!m_impl->tObj.search(Q, count, sinceID,includeEnts))
	{
		std::string err;
		m_impl->tObj.getLastCurlError(err);
		gLogManager.log("TwitterService::Search() - " + err, ELL_WARNING);
		return false;
	}


	std::string reply;
	m_impl->tObj.getLastWebResponse(reply);

	Json::Value root;
	m_impl->reader.parse(reply, root);
	try
	{
		tweets.max_id = root["search_metadata"]["max_id"].asDouble();
		parseTweets(root["statuses"], tweets.tweets);
	}
	catch (std::exception& e)
	{
		return false;
	}
	return true;
}

bool TwitterService::GetTweet(const GetTweetOptions& op, Tweet& t)
{

	std::string id = "";
	bool includeEnts = op.include_entities;

	// 	if (Q.length() == 0)
	// 		return false;
	if (op.id == 0)
		return false;

		id = core::StringConverter::toString(op.id);
		if (!m_impl->tObj.statusShowById(id))
	{
		std::string err;
		m_impl->tObj.getLastCurlError(err);
		gLogManager.log("TwitterService::GetTweet() - " + err, ELL_WARNING);
		return false;
	}


	std::string reply;
	m_impl->tObj.getLastWebResponse(reply);

	Json::Value root;
	m_impl->reader.parse(reply, root);
	try
	{
		parseTweet(root, t);
		//		printValueTree(root);
		// 		tweets.max_id = root["search_metadata"]["max_id"].asDouble();
		// 		parseTweets(root["statuses"], tweets.tweets);
	}
	catch (std::exception& e)
	{
		return false;
	}
	return true;
}

bool TwitterService::SearchForUser(const UserSearchOptions& op, UserSearchResult& u)
{
	std::string id = "";
	std::string name = op.screenName;
	bool includeEnts = op.include_entities;

// 	if (Q.length() == 0)
// 		return false;
	if (op.id != 0)
		id = core::StringConverter::toString(op.id);
	if (!m_impl->tObj.userGet(id != "" ? id : name, id != ""))
	{
		std::string err;
		m_impl->tObj.getLastCurlError(err);
		gLogManager.log("TwitterService::SearchForUser() - " + err, ELL_WARNING);
		return false;
	}


	std::string reply;
	m_impl->tObj.getLastWebResponse(reply);

	Json::Value root;
	m_impl->reader.parse(reply, root);
	try
	{
		parseUser(root, u.u);
//		printValueTree(root);
// 		tweets.max_id = root["search_metadata"]["max_id"].asDouble();
// 		parseTweets(root["statuses"], tweets.tweets);
	}
	catch (std::exception& e)
	{
		return false;
	}
	return true;
}
}
}

