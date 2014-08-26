


#ifndef ___Win32Network___
#define ___Win32Network___

#include "INetwork.h"
#include "NetAddress.h"
#include "CompileConfig.h"
#include "ISocket.h"

#include <winsock2.h>

namespace mray{
namespace network{


class MRAY_NET_DLL Win32Network:public INetwork
{
	
	const int SYNC_NUMBER;

	void checkForError();
public:
	Win32Network();


	virtual~Win32Network();
	int getSyncNumber();


	static void CreateAddress(const NetAddress&addr,sockaddr_in& ret);


	IWebRequest* CreateWebRequest();
	ISocket* createSocket(const core::string&name,bool isHost,bool Reliable);
	virtual IUDPClient* createUDPClient();
	virtual IReliableSocket* createTCPSocket();

	void closeSocket(SOCKET s);
	void shutdownSocket(SOCKET s);

	static SOCKET createSocket(EProtoType proto);
	static SOCKET openUDPSocket(const char* netInterface,int port);
	static void attachUDPSocket(SOCKET s,const char* netInterface,int port);

	NetAddress getSockAddr(SOCKET s);

	static void toSockaddr(const NetAddress &addr,sockaddr*sAddr);

	void toAddress(const sockaddr*sAddr,NetAddress &addr);

	static bool inner_getHostAddress(const char*name,NetAddress &addr);
	bool getHostAddress(const char*name,NetAddress &addr);

	bool getHostIP(const NetAddress &addr,char*name,int maxLen);
	bool getHostName(const NetAddress &addr,char*name,int maxLen);

	static bool setSocketOption(SOCKET s,ESocketOption option,int val);
	
	static bool enableBroadcast(SOCKET s,bool enable);

	//get all ip addresses for this pc
	int getLocalIPs(char ipLst[8][16]);

	ulong getLocalAddress();

	int getSocketPort(SOCKET s);
	bool getSocketAddress(SOCKET s,sockaddr_in&out);


	static bool inner_connect(SOCKET s,const NetAddress&addr);
	static bool inner_bind(SOCKET s,const NetAddress&addr);
	static bool inner_listen(SOCKET s,int maxIncomingConnections);
	static SOCKET inner_accept(SOCKET s,NetAddress*addr);
	static int inner_sendto(SOCKET s,const char*buff,int buff_size,
		const NetAddress * toAddress,int flags=0);
	static int inner_receivefrom(SOCKET s,CMsgBuffer*msg,
		NetAddress * fromAddress,int flags=0);
	static int inner_receivefrom(SOCKET s,void*data,int len,
		NetAddress * fromAddress,int flags=0);
	static int inner_broadcast(SOCKET s,int port,char*buff,int buff_size);


	int sendto(ISocket*s,const char*buff,int buff_size,
		const NetAddress * toAddress,int flags=0);

	//condition : composition of ESocketWait enum wich we want to check for
	//return : composition of ESocketWait enum as result of the test
	static int wait(SOCKET s,uint timeout,int  condition);
};

}
}


#endif







