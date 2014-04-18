

/********************************************************************
	created:	2013/02/18
	created:	18:2:2013   14:13
	filename: 	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\Win32SerialPort.h
	file path:	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	Win32SerialPort
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __Win32SerialPort__
#define __Win32SerialPort__


#include "ISerialPort.h"

class ofSerial;

namespace mray
{
namespace OS
{

class Win32SerialPort:public ISerialPort
{
protected:
	ofSerial* m_sp;
	bool m_open;
	IStream* m_stream;
public:
	Win32SerialPort();
	virtual~Win32SerialPort();


	virtual bool OpenByName(const core::string& port,int baudRate);
	virtual bool OpenByID(int ID,int baudRate);
	virtual void Close();

	virtual int Write(const void* data,size_t size);
	virtual int Read(void* data,size_t size);

	virtual int AvailableData();
	virtual bool IsOpen();
	virtual void Flush(bool flushIn,bool flushOut);

	virtual IStream* GetStream();

};


class Win32SerialPortService:public ISerialPortService
{
public:
	Win32SerialPortService(){}
	std::vector<SerialPortInfo> EnumAvaliablePorts(bool rescan=false);
};


}
}


#endif
