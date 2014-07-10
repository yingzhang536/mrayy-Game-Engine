

/********************************************************************
	created:	2014/04/09
	created:	9:4:2014   21:44
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\SQLDBHandler.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	SQLDBHandler
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __SQLDBHandler__
#define __SQLDBHandler__

#include "IDBHandler.h"
#include "ISQLManager.h"

namespace mray
{
namespace kmd
{

class SQLDBHandler :public IDBHandler
{
protected:

	db::ISQLManager* m_sqlManager;
	GCPtr<db::ISQLConnection> m_connection;

	void _LoadComments();

	bool _connect();
	void _disconnect();

	void LoadComment(db::ISQLResult* res, KMDComment* t);
public:
	SQLDBHandler();
	virtual~SQLDBHandler();

	void LoadDB();

	bool IsConnected();

	virtual KMDComment* RequestComment(IDType id);
	std::vector<KMDComment*> LoadComments(IDType sinceID);



	virtual void SaveDB() {}

	virtual void Close(){ _disconnect(); }


};

}
}


#endif
