

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
#include "IThread.h"
#include "SerialEx.h"


namespace mray
{
namespace OS
{

class Win32SerialPort:public ISerialPort
{
protected:
	CSerialEx* m_port;
	IStream* m_stream;
public:
	Win32SerialPort();
	virtual~Win32SerialPort();


	virtual bool OpenByName(const core::string& port, EBaudRate baudRate);

	virtual void Close();
	virtual bool WaitForData() ;

	virtual void SetTimeOut(uint read_timeout, uint write_timeout);
	virtual void GetTimeOut(uint &read_timeout, uint &write_timeout);
	virtual uint GetBaudRate() ;
	virtual int Write(const void* data,size_t size);
	virtual int Read(void* data,size_t size);

	virtual uint AvailableData();
	virtual bool IsOpen();
	virtual void Flush(bool flushIn,bool flushOut);

	virtual IStream* GetStream();

	//internal
	void __OnSerialEvent(CSerial::EEvent eEvent, CSerial::EError eError);

};


class Win32SerialPortService:public ISerialPortService
{
protected:
	bool bPortsEnumerated;
	std::vector <SerialPortInfo> devices;
public:
	Win32SerialPortService();
	virtual ISerialPort* CreateSerialPort()
	{
		return new Win32SerialPort();
	}
	std::vector<SerialPortInfo> EnumAvaliablePorts(bool rescan = false);
};


}
}


#endif
