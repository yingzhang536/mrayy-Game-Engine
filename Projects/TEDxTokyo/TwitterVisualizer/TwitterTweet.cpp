
#include "stdafx.h"
#include "TwitterTweet.h"

#include "IDBHandler.h"
#include "SQLAPI.h"
#include "TwitterUserProfile.h"
#include "StringUtil.h"

namespace mray
{
namespace ted
{
class UserD;


std::map<IDType, uint> TwitterTweet::TweetIDMap;
std::vector<TwitterTweet*> TwitterTweet::TwitterTweetList;

void TwitterTweet::AddTwitterTweet(TwitterTweet* t)
{
	TwitterTweetList.push_back(t);
	TweetIDMap[t->ID] = TwitterTweetList.size() - 1;
}
TwitterTweet* TwitterTweet::GetTweetByID(IDType id)
{
	std::map<IDType, uint>::iterator it = TweetIDMap.find(id);
	if (it == TweetIDMap.end())
	{
		TwitterTweet* u = IDBHandler::getInstance().RequestTweet(id);
		if (u)
			AddTwitterTweet(u);
		return u;
	}
	return TwitterTweetList[it->second];

}

void TwitterTweet::LoadXML(xml::XMLElement* e)
{
	ID = core::StringConverter::toUInt(e->getValueString("ID"));
	text = ConvertToUtf16(e->getValueString("text"));
	core::CDate::Parse(e->getValueString("date"), date);
	retweets = core::StringConverter::toUInt(e->getValueString("retweets"));
	user = TwitterUserProfile::GetUserByID(core::StringConverter::toUInt(e->getValueString("user")),false);
}
void TwitterTweet::SaveXML(xml::XMLElement* e)
{
	xml::XMLElement* elem = new xml::XMLElement("User");
	e->addSubElement(elem);

	elem->addAttribute("ID", core::StringConverter::toString(ID));
	elem->addAttribute("user", core::StringConverter::toString(user->ID));
	elem->addAttribute("text", ConvertToUtf8(text));
	elem->addAttribute("date", core::CDate::ToString(date));
	elem->addAttribute("retweets", core::StringConverter::toString(retweets));

	xml::XMLElement* hashtags = new xml::XMLElement("Hashtags");
	xml::XMLElement* urls = new xml::XMLElement("Urls");
	xml::XMLElement* user_mentions = new xml::XMLElement("UserMentions");
	elem->addSubElement(hashtags);
	elem->addSubElement(urls);
	elem->addSubElement(user_mentions);

	for (int i = 0; i < entities.hashTags.size(); ++i)
	{
		xml::XMLElement* h = new xml::XMLElement("Hash");
		hashtags->addSubElement(h);
		h->addAttribute("Value", ConvertToUtf8(entities.hashTags[i]));
	}
	for (int i = 0; i < entities.urls.size(); ++i)
	{
		xml::XMLElement* h = new xml::XMLElement("Url");
		urls->addSubElement(h);
		h->addAttribute("Value", entities.urls[i]);
	}
	for (int i = 0; i < entities.user_mentions.size(); ++i)
	{
		xml::XMLElement* h = new xml::XMLElement("User");
		user_mentions->addSubElement(h);
		h->addAttribute("Value", core::StringConverter::toString(entities.user_mentions[i]->ID));
	}
}


}
}

