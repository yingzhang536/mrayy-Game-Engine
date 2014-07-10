
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



XMLDBHandler::XMLDBHandler(const core::string& usersXML, const core::string& TweetsXML)
{
	m_usersPath = usersXML;
	m_TweetsPath = TweetsXML;
}

XMLDBHandler::~XMLDBHandler()
{
}


void XMLDBHandler::LoadDB()
{
	_loadUsers();
	_loadTweets();
}

void XMLDBHandler::SaveDB()
{
	_saveUsers();
	_saveTweets();
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
	if (!tree.load(m_TweetsPath))
		return;

	xml::XMLElement* e = tree.getSubElement("Tweets");
	if (!e)
		return;
	e = e->getSubElement("Tweet");
	while (e)
	{
		TwitterTweet* t = new TwitterTweet();
		t->LoadXML(e);
		TwitterTweet::AddTwitterTweet(t);
		e = e->nextSiblingElement("Tweet");
	}
}

void XMLDBHandler::_saveUsers()
{

	GCPtr<xml::XMLElement> root = new xml::XMLElement("Users");

	for (int i = 0; i < TwitterUserProfile::TwitterUserProfileList.size(); ++i)
	{
		TwitterUserProfile::TwitterUserProfileList[i]->SaveXML(root);
	}
	xml::XMLWriter w;
	w.addElement(root);
	core::string str= w.flush();
	OS::IStreamPtr stream = gFileSystem.openFile(m_usersPath, OS::TXT_WRITE);
	if (!stream)
		return;
	OS::StreamWriter ww(stream);
	ww.writeString(str);
	stream->close();
}

void XMLDBHandler::_saveTweets()
{

	GCPtr<xml::XMLElement> root = new xml::XMLElement("Tweets");

	for (int i = 0; i < TwitterTweet::TwitterTweetList.size(); ++i)
	{
		TwitterTweet::TwitterTweetList[i]->SaveXML(root);
	}
	xml::XMLWriter w;
	w.addElement(root);
	core::string str = w.flush();
	OS::IStreamPtr stream = gFileSystem.openFile(m_TweetsPath, OS::TXT_WRITE);
	if (!stream)
		return;
	OS::StreamWriter ww(stream);
	ww.writeString(str);
	stream->close();

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

