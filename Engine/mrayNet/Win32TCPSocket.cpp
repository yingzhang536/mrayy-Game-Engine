
#include "stdafx.h"

#include "Win32TCPSocket.h"
#include "Win32Network.h"
#include "Win32Socket.h"
#include <ITimer.h>
#include <IThread.h>
#include <IThreadFunction.h>
#include <TraceManager.h>
#include <IThreadManager.h>
#include <MutexLocks.h>

namespace mray{
namespace network{

	
class Win32TCPSocketProcessThread:public OS::IThreadFunction
{
	Win32TCPSocket*m_socket;
public:
	Win32TCPSocketProcessThread(Win32TCPSocket*s){
		m_socket=s;
		m_stopThread=0;
	}

	bool m_stopThread;

protected:
	virtual void setup(){
	}
	virtual void execute(OS::IThread*caller,void*arg){
		while(caller->isActive()){

			m_socket->update();

		//	m_socket->update();
/*
			if(m_socket->m_sleepInterval>0){
				WSAWaitForMultipleEvents(1,&m_socket->m_reciveEvent,0,m_socket->m_sleepInterval,0);
			}*/
		}
	}
};

Win32TCPSocket::Win32TCPSocket(Win32Socket*sock){
	m_socket=((Win32Network*)INetwork::getInstancePtr())->createSocket(EProtoType::EPT_TCP);
	m_thread=0;

	m_attachedSock=sock;
	m_threadFunc=new Win32TCPSocketProcessThread(this);

	m_toCloseMutex=OS::IThreadManager::getInstance().createMutex();
}

Win32TCPSocket::~Win32TCPSocket(){
	stopSocket();
	delete m_threadFunc;

	std::list<SPacket*>::iterator it;
	
	for(it=	m_graveyardPackets.begin();it!=m_graveyardPackets.end();++it)
		delete *it;

	for(it=	m_recivedPackets.begin();it!=m_recivedPackets.end();++it)
		delete *it;

	for(it=	m_sendPackets.begin();it!=m_sendPackets.end();++it)
		delete *it;

}
void Win32TCPSocket::deletePeer(STCPPeer*peer){
	if(!peer)return;
	Win32Network*net=(Win32Network*)INetwork::getInstancePtr();
	net->closeSocket(peer->m_socket);
	
}

SPacket* Win32TCPSocket::getOrCreatePacket()
{
	std::list<SPacket*>::iterator it=m_graveyardPackets.begin();
	if(it==m_graveyardPackets.end())
		return new SPacket();
	SPacket*ret=*it;
	ret->data.resize(0);
	m_graveyardPackets.erase(it);
	return ret;
}


void Win32TCPSocket::closeConnection(){
	stopSocket();
	if(m_thread)
	{
		OS::IThreadManager::getInstance().killThread(m_thread);
		delete m_thread;
	}
}
bool Win32TCPSocket::isConnected()
{
	return m_socket != INVALID_SOCKET;
}

bool Win32TCPSocket::startSocket(int port,int maxIncomingConnection,int sleepInterval){
	traceFunction(eNetwork);
	if(m_thread)return false;
	m_maxConnection=maxIncomingConnection;
	Win32Network*net=(Win32Network*)INetwork::getInstancePtr();
	if(maxIncomingConnection>0){
		//net->setSocketOption(m_socket,ESocketOption::ESO_NONBLOCK,true);
		if(m_socket==INVALID_SOCKET){
			
			return false;
		}
		NetAddress addr(EAdressType::EAT_Any,port);
		if(net->inner_bind(m_socket,addr)==false){
			
			return false;
		}
		net->inner_listen(m_socket,maxIncomingConnection);
	}

	m_sleepInterval=sleepInterval;

	m_thread=OS::IThreadManager::getInstance().createThread(m_threadFunc);
	m_thread->start(0);

	m_localAddress=net->getSockAddr(m_socket);

	
	return true;
}

void Win32TCPSocket::stopSocket(){
	if(!m_thread)return;
	traceFunction(eNetwork);

	if(!INetwork::isExist())
		return;

	Win32Network*net=(Win32Network*)INetwork::getInstancePtr();
	net->closeSocket(m_socket);
	m_socket=INVALID_SOCKET;
	m_thread->terminate();
	while(m_thread->isActive())
		Sleep(10);

	OS::ScopedLock locker(m_peersMutex);
	std::list<STCPPeer*>::iterator it = m_connectedPeers.begin();
	for(;it!=m_connectedPeers.end();++it){
		net->closeSocket((*it)->m_socket);
		delete *it;
	}
	m_connectedPeers.clear();

	
}

void Win32TCPSocket::sendPacket(void*data,uint size,const NetAddress&addr){
	traceFunction(eNetwork);
	int res=0;
	OS::ScopedLock locker(m_peersMutex);
	std::list<STCPPeer*>::iterator peerIt;
	if(addr==NetAddress::BroadcastAddr){
		for(peerIt=m_connectedPeers.begin();peerIt!=m_connectedPeers.end();++peerIt)
			res=::send((*peerIt)->m_socket,(const char*)data,size,0);
	}else{
		for(peerIt=m_connectedPeers.begin();peerIt!=m_connectedPeers.end();++peerIt)
			if((*peerIt)->address==addr){
				res=::send((*peerIt)->m_socket,(const char*)data,size,0);
				break;
			}
		
	}
	
}
void Win32TCPSocket::send(void*data,uint size,bool imediate,const NetAddress&addr){
	if(!m_thread)return;
	if(!m_connectedPeers.size() && m_maxConnection>1 || m_socket==INVALID_SOCKET)return;
	if(!data)return;

	traceFunction(eNetwork);

	SPacket*pack=getOrCreatePacket();
	pack->data.resize(size);
	pack->data.write(&size,sizeof(size));
	pack->data.write(data,size);
	pack->remoteAddress=addr;

	if(!imediate){
		m_sendPackets.push_back(pack);
	}else{
		sendPacket(pack->data.pointer(),pack->data.size(),pack->remoteAddress);
		m_graveyardPackets.push_back(pack);
	}
	
}
SPacket*Win32TCPSocket::popMessage(){
	if(!m_thread)return 0;
	if(!m_recivedPackets.size())return 0;

	std::list<SPacket*>::iterator recvIT=m_recivedPackets.end();--recvIT;
	SPacket*pack=(*recvIT);
	m_recivedPackets.erase(recvIT);
	return pack;
}
Win32TCPSocket::STCPPeer*Win32TCPSocket::getPeer(const NetAddress&addr){
	OS::ScopedLock locker(m_peersMutex);
	std::list<STCPPeer*>::iterator it = m_connectedPeers.begin();
	for(;it!=m_connectedPeers.end();++it){
		if((*it)->address==addr)return *it;
	}
	return 0;
}

bool Win32TCPSocket::peerConnected(const NetAddress&addr){
	return getPeer(addr)!=0;
}

void Win32TCPSocket::GetConnectedPeers(std::list<NetAddress>& peers)
{
	OS::ScopedLock locker(m_peersMutex);
	std::list<STCPPeer*>::iterator it = m_connectedPeers.begin();
	for (; it != m_connectedPeers.end(); ++it){
		peers.push_back((*it)->address);
	}

}
const NetAddress *Win32TCPSocket::connect(const NetAddress&addr){
	if(getPeer(addr))return 0;
	traceFunction(ePhysics);
	Win32Network*net=(Win32Network*)INetwork::getInstancePtr();
	SOCKET sock=net->createSocket(EProtoType::EPT_TCP);
	if(!net->inner_connect(sock,addr)){
		net->closeSocket(sock);
		
		return 0;
	}
	net->setSocketOption(sock,ESocketOption::ESO_NONBLOCK,true);
	STCPPeer *peer=new STCPPeer();
	peer->m_socket=sock;
	peer->address = addr;
	{

		OS::ScopedLock locker(m_peersMutex);
		m_connectedPeers.push_back(peer);
	}


	if(!m_thread){
		m_thread=OS::IThreadManager::getInstance().createThread(m_threadFunc);
		m_thread->start(0);
	}

	
	return &peer->address;
}

const NetAddress *Win32TCPSocket::connect(const core::string& host, ushort port){
	NetAddress addr;

	Win32Network*net=(Win32Network*)INetwork::getInstancePtr();
	if(!net->getHostAddress(host.c_str(),addr))
		return 0;

	return connect(addr);

}

const NetAddress*Win32TCPSocket::getAddress(){
	return &m_localAddress;
}

void Win32TCPSocket::update(){

	std::list<SPacket*>::iterator sendIT;
	std::list<SPacket*>::iterator recvIT;
	std::list<NetAddress>::iterator closeIT;
	std::list<STCPPeer*>::iterator peerIt;
	std::list<STCPPeer*>::iterator peerIt2;

	Win32Network*net=(Win32Network*)INetwork::getInstancePtr();

	fd_set readFD;
	fd_set exceptionFD;
	SOCKET largestSock;
	timeval tv;
	SOCKET newSock;

	const int BUFF_SIZE=8096;
	char dataBuffer[BUFF_SIZE];
	
	sendIT=m_sendPackets.begin();
	for(;sendIT!=m_sendPackets.end();++sendIT){
		sendPacket((*sendIT)->data.pointer(),(*sendIT)->data.size(),(*sendIT)->remoteAddress);
		(*sendIT)->data.clear();
		m_graveyardPackets.push_back(*sendIT);
	}
	m_sendPackets.clear();

	closeIT=m_toClosePeers.begin();
	if(closeIT!=m_toClosePeers.end()){
		for(peerIt=m_connectedPeers.begin();peerIt!=m_connectedPeers.end();++peerIt){
			if((*peerIt)->address==(*closeIT)){
				OS::ScopedLock locker(m_peersMutex);
				deletePeer((*peerIt));
				break;
			}
		}
		{
			OS::ScopedLock locker(m_toCloseMutex);
			m_toClosePeers.erase(closeIT);
		}
	}

	FD_ZERO(&readFD);
	FD_ZERO(&exceptionFD);

	largestSock=0;
	if(m_socket!=INVALID_SOCKET){
		FD_SET(m_socket,&readFD);
		FD_SET(m_socket,&exceptionFD);
		largestSock=m_socket;
	}
	
	for(peerIt=m_connectedPeers.begin();peerIt!=m_connectedPeers.end();++peerIt){
		FD_SET((*peerIt)->m_socket,&readFD);
		FD_SET((*peerIt)->m_socket,&exceptionFD);
		if((*peerIt)->m_socket>largestSock){
			largestSock=(*peerIt)->m_socket;
		}
	}

	tv.tv_sec=0;
	tv.tv_usec=25000;
	int selectRes=select(largestSock+1,&readFD,0,&exceptionFD,&tv);
	if(selectRes>0){
		if(m_socket!=INVALID_SOCKET && FD_ISSET(m_socket,&readFD)){
			NetAddress addr;
			newSock=net->inner_accept(m_socket,&addr);
			if(newSock!=INVALID_SOCKET){
				STCPPeer *peer=new STCPPeer();
				peer->address=addr;
				peer->m_socket=newSock;
				m_connectedPeers.push_back(peer);
				FD_SET(newSock,&readFD);
				FD_SET(newSock,&exceptionFD);
			}
		}else 
		{
			for(peerIt=m_connectedPeers.begin();peerIt!=m_connectedPeers.end();){
				bool remove=0;
				if(FD_ISSET((*peerIt)->m_socket,&exceptionFD)){
					m_toClosePeers.push_back((*peerIt)->address);
					deletePeer(*peerIt);
					remove=1;
				}else if(FD_ISSET((*peerIt)->m_socket,&readFD)){
					uint sz=0;
					int len=0;
					do{
						len=recv((*peerIt)->m_socket,(char*)&sz,sizeof(sz),0);
						if(len>0){
							len=recv((*peerIt)->m_socket,dataBuffer,sz,0);

							GCPtr<SPacket> pack=new SPacket();
							pack->data.resize(len+1);
							pack->data.write(dataBuffer,len);
							pack->data[len]=0;
							pack->remoteAddress=(*peerIt)->address;
							pack->data.reset();
							if (m_attachedSock)
								m_attachedSock->parsePacket(pack->remoteAddress,&pack->data);
							m_recivedPackets.push_back(pack);
						}else if(len==0){
							m_toClosePeers.push_back((*peerIt)->address);
							deletePeer(*peerIt);
							remove=1;
							break;
						}

					}while(len!=-1);
				}
				if(remove){
					peerIt2=peerIt;++peerIt2;
					m_connectedPeers.erase(peerIt);
					peerIt=peerIt2;
				}else
					++peerIt;
			}
		}
	}else if(selectRes==0){
		Sleep(m_sleepInterval);
	}
}

}
}





