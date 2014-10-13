

/********************************************************************
	created:	2013/02/18
	created:	18:2:2013   14:02
	filename: 	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\ISerialPort.h
	file path:	C:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	ISerialPort
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __ISerialPort__
#define __ISerialPort__

#include "IStream.h"



namespace mray
{
namespace OS
{

	struct SerialPortInfo
	{
	public:
		core::string Name;
		core::string Description;
		core::string HardwareID;
	};

	class ISerialPort;

	class MRAY_DLL SerialPortStream:public IStream
	{
	protected:
		ISerialPort* m_serialPort;
		virtual uint calcSizeInternal(){return sizeof(SerialPortStream);}

	public:
		SerialPortStream(ISerialPort* sp);
		virtual~SerialPortStream();

		virtual bool canRead();
		virtual bool canWrite();
		virtual bool canSeek();
		//! get size of file
		virtual int length();
		virtual int getPos();

		virtual bool isOpen();
		virtual int read(void*data,size_t size);
		virtual int write(const void*data,size_t size);
		virtual int seek(int bytes,EStreamSeek mode);
		virtual bool eof();
		virtual void close();

	};
class MRAY_DLL ISerialPort
{
protected:
public:
	ISerialPort(){}
	virtual~ISerialPort(){}

	virtual bool OpenByName(const core::string& port,int baudRate)=0;
	virtual void Close()=0;

	virtual void SetTimeOut(uint read_timeout, uint write_timeout) = 0;
	virtual void GetTimeOut(uint &read_timeout, uint &write_timeout) = 0;
	virtual int Write(const void* data,size_t size)=0;
	virtual int Read(void* data,size_t size)=0;

	virtual uint GetBaudRate() = 0;

	virtual uint AvailableData() = 0;
	virtual bool IsOpen()=0;
	virtual void Flush(bool flushIn,bool flushOut)=0;

	virtual IStream* GetStream()=0;



};

class ISerialPortService
{
public:
	virtual ISerialPort* CreateSerialPort() = 0;
	virtual std::vector<SerialPortInfo> EnumAvaliablePorts(bool rescan)=0;
};

}
}


#endif
