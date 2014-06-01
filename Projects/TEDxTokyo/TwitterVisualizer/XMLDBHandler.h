

#ifndef XMLDBHandler_h__
#define XMLDBHandler_h__

#include "IDBHandler.h"

namespace mray
{
namespace ted
{


class XMLDBHandler :public IDBHandler
{
protected:
	core::string m_usersPath;
	core::string m_tweetsPath;
	void _loadUsers();
	void _loadTweets();

	void _saveUsers();
	void _saveTweets();
public:
	XMLDBHandler(const core::string& usersXML, const core::string& tweetsXML);
	virtual~XMLDBHandler();


	virtual void LoadDB() ;
	virtual void SaveDB();

	virtual bool IsConnected() ;
	virtual void Close();

	virtual TwitterTweet* RequestTweet(IDType id);
	virtual TwitterUserProfile* RequestUser(IDType id);
};

}
}

#endif // XMLDBHandler_h__
