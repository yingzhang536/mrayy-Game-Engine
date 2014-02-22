

/********************************************************************
	created:	2014/02/17
	created:	17:2:2014   4:16
	filename: 	C:\Development\mrayEngine\Projects\AugTel\DataCommunicator.h
	file path:	C:\Development\mrayEngine\Projects\AugTel
	file base:	DataCommunicator
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __DataCommunicator__
#define __DataCommunicator__





#include "IUDPClient.h"

namespace mray
{
namespace AugTel
{

class DataCommunicator
{
protected:

	network::IUDPClient* m_client;
	OS::IThread* m_thread;
public:
	DataCommunicator();
	virtual~DataCommunicator();

	void Start(int port);
	void Stop();

	int _Process();
};

}
}


#endif
