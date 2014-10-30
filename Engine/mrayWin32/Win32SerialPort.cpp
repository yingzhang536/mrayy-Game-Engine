
#include "stdafx.h"
#include "Win32SerialPort.h"
#include "IThreadFunction.h"
#include "IThreadManager.h"


namespace mray
{
namespace OS
{

	class Win32SerialPortImpl :public CSerialEx
	{
		Win32SerialPort* m_owner;
	public:
		Win32SerialPortImpl(Win32SerialPort* o)
		{
			m_owner = o;
		}
	protected:
		virtual void OnEvent(EEvent eEvent, EError eError)
		{
			m_owner->__OnSerialEvent(eEvent, eError);
		}

	};

Win32SerialPort::Win32SerialPort()
{
	m_port = new Win32SerialPortImpl(this);
	m_stream=new SerialPortStream(this);
}
Win32SerialPort::~Win32SerialPort()
{
	Close();
	delete m_port;
	delete m_stream;
}


bool Win32SerialPort::OpenByName(const core::string& port, ISerialPort::EBaudRate baudRate)
{
	if (m_port->IsOpen())
		Close();
	m_port->Open(port.c_str(), 0, 0, true);
	m_port->Setup((CSerial::EBaudrate)baudRate);

	 m_port->SetMask(CSerial::EEventBreak |
		CSerial::EEventCTS |
		CSerial::EEventDSR |
		CSerial::EEventError |
		CSerial::EEventRing |
		CSerial::EEventRLSD |
		CSerial::EEventRecv);
	 m_port->SetupReadTimeouts(CSerial::EReadTimeoutNonblocking);
	return m_port->IsOpen();
}
void Win32SerialPort::Close()
{
	m_port->Close();

}
bool Win32SerialPort::WaitForData()
{
	return false;
}

uint Win32SerialPort::GetBaudRate()
{
	return m_port->GetBaudrate();
}

void Win32SerialPort::SetTimeOut(uint read_timeout, uint write_timeout)
{
	//m_port->SetupReadTimeouts( serial::Timeout::max(), read_timeout, 0, write_timeout, 0);
}
void Win32SerialPort::GetTimeOut(uint &read_timeout, uint &write_timeout)
{
/*
	serial::Timeout to= m_port->getTimeout();
	read_timeout = to.read_timeout_constant;
	write_timeout = to.write_timeout_constant;*/
}

int Win32SerialPort::Write(const void* data,size_t size)
{
	DWORD pdwWritten = 0;
	 m_port->Write((const unsigned char*)data, size, &pdwWritten);
	 return pdwWritten;
}
int Win32SerialPort::Read(void* data,size_t size)
{
	DWORD readed = 0;
	m_port->Read((unsigned char*)data, size, &readed);
	return readed;
}

uint Win32SerialPort::AvailableData()
{
	return m_port->AvailableBytes();
}
bool Win32SerialPort::IsOpen()
{
	return m_port->IsOpen();
}
void Win32SerialPort::Flush(bool flushIn,bool flushOut)
{
	/*
	if (flushIn)
		m_port->flushInput();
	if (flushOut)
		m_port->flushOutput();*/
}

IStream* Win32SerialPort::GetStream()
{
	return m_stream;
}

void Win32SerialPort::__OnSerialEvent(CSerial::EEvent eEvent, CSerial::EError eError)
{
	if (eEvent == CSerial::EEventRecv)
	{
		OnDataArrived(this);
	}
}

Win32SerialPortService::Win32SerialPortService()
{
	bPortsEnumerated = false;
}
std::vector<SerialPortInfo> Win32SerialPortService::EnumAvaliablePorts(bool rescan)
{
	if (!rescan && bPortsEnumerated)
		return devices;
	devices.clear();

	std::vector<CSerial::SerialPortInfo> ports = CSerial::ListPorts();

	for (int i = 0; i < ports.size(); ++i)
	{
		SerialPortInfo ifo;
		ifo.Name = ports[i].port;
		ifo.Description = ports[i].description;
		ifo.HardwareID = ports[i].hardware_id;
		devices.push_back(ifo);
	}

	bPortsEnumerated = true;
	return devices;
}

}
}

