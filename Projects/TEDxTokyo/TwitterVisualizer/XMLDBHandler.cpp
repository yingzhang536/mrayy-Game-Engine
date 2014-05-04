
#include "stdafx.h"
#include "XMLDBHandler.h"
#include "XMLTree.h"

#include "TwitterUserProfile.h"
#include "TwitterTweet.h"
#include "AppData.h"
#include "TwitterProvider.h"

namespace mray
{
namespace ted
{



XMLDBHandler::XMLDBHandler(const core::string& usersXML, const core::string& tweetsXML)
{
	m_usersPath = usersXML;
	m_tweetsPath = tweetsXML;
}

XMLDBHandler::~XMLDBHandler()
{
}


void XMLDBHandler::LoadDB()
{
	_loadUsers();
	_loadTweets();
}

void XMLDBHandler::_loadUsers()
{
	xml::XMLTree tree;
	if (!tree.load(m_usersPath))
		return;

	xml::XMLElement* e= tree.getSubElement("Users");
	if (!e)
		return;
	e = e->getSubElement("User");
	while (e)
	{
		TwitterUserProfile* user = new TwitterUserProfile();
		user->LoadXML(e);
		TwitterUserProfile::AddTwitterUserProfile(user);
		e = e->nextSiblingElement("User");
	}
}

void XMLDBHandler::_loadTweets()
{
	xml::XMLTree tree;
	if (!tree.load(m_tweetsPath))
		return;

	xml::XMLElement* e = tree.getSubElement("Tweets");
	if (!e)
		return;
	e = e->getSubElement("Tweet");
	while (e)
	{
		TwitterTweet* user = new TwitterTweet();
		user->LoadXML(e);
		TwitterTweet::AddTwitterTweet(user);
		e = e->nextSiblingElement("Tweet");
	}
}

bool XMLDBHandler::IsConnected()
{
	return true;
}

void XMLDBHandler::Close()
{
	//save everything
}


TwitterTweet* XMLDBHandler::RequestTweet(IDType id)
{
	return AppData::getInstance().tweetProvider->GetTweetByID(id);
}

TwitterUserProfile* XMLDBHandler::RequestUser(IDType id)
{
	return AppData::getInstance().tweetProvider->GetUserByID(id);
}


}
}

