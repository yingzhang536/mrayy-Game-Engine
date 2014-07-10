

#ifndef IDBHandler_h__
#define IDBHandler_h__

#include "ISingleton.h"
#include "DataTypes.h"

namespace mray
{
namespace kmd
{
	class KMDComment;

class IDBHandler :public ISingleton<IDBHandler>
{
protected:
public:
	IDBHandler(){}
	virtual~IDBHandler(){}

	virtual void LoadDB() = 0;
	virtual void SaveDB() = 0;
	virtual bool IsConnected() = 0;

	virtual void Close(){}

	virtual KMDComment* RequestComment(IDType id) = 0;

	virtual std::vector<KMDComment*> LoadComments(IDType sinceID) = 0;
};

}
}

#endif // IDBHandler_h__
