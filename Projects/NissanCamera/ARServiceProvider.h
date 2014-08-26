


#ifndef ARServiceProvider_h__
#define ARServiceProvider_h__

#include "ListenerContainer.h"
#include "ARDataTypes.h"

namespace mray
{
namespace NCam
{

	class IARServiceListener
	{
	public:
		virtual void OnARContents(ARCommandAddData* cmd){};
		virtual void OnVechicleData(){}
		virtual void OnDeletedGroup(ARCommandDeleteGroup* cmd){}
	};

	class ARProcessThread;


// this class is responsible to provide AR contents to the application from a remote server
class ARServiceProvider:public ListenerContainer<IARServiceListener*>
{
protected:

	network::IReliableSocket* m_connection;
	OS::IThread* m_thread;
	network::NetAddress m_serverAddr;

	std::list<IARCommand*> m_commands;

	OS::IMutex* m_commandsMutex;

	DECLARE_FIRE_METHOD(OnARContents, (ARCommandAddData* cmd), (cmd));
	DECLARE_FIRE_METHOD(OnDeletedGroup, (ARCommandDeleteGroup* cmd), (cmd));
	DECLARE_FIRE_METHOD(OnVechicleData, (), ());


	friend class ARProcessThread;
	void UpdateThread();
public:
	ARServiceProvider();
	virtual ~ARServiceProvider();

	bool Connect(const core::string& host, uint port);
	void Disconnect();
	bool IsConnected();

	void Update();
};

}
}

#endif // ARServiceProvider_h__

