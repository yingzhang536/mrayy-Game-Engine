

#ifndef IDBHandler_h__
#define IDBHandler_h__

#include "ISingleton.h"
#include "DataTypes.h"

namespace mray
{
namespace ted
{
	class TwitterTweet;
	class TwitterUserProfile;

class IDBHandler :public ISingleton<IDBHandler>
{
protected:
public:
	IDBHandler(){}
	virtual~IDBHandler(){}

	virtual void LoadDB() = 0;
	virtual bool IsConnected() = 0;

	virtual void Close(){}

	virtual TwitterTweet* RequestTweet(IDType id) = 0;
	virtual TwitterUserProfile* RequestUser(IDType id) = 0;

};

}
}

#endif // IDBHandler_h__
