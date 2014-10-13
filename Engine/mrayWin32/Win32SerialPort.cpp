
#include "stdafx.h"
#include "Win32SerialPort.h"


namespace mray
{
namespace OS
{

Win32SerialPort::Win32SerialPort()
{
	m_port = new serial::Serial();
	m_stream=new SerialPortStream(this);
}
Win32SerialPort::~Win32SerialPort()
{
	delete m_port;
	delete m_stream;
}


bool Win32SerialPort::OpenByName(const core::string& port,int baudRate)
{
	if (m_port->isOpen())
		Close();
	m_port->setPort(port);
	m_port->setBaudrate(baudRate);
	m_port->open();
	return m_port->isOpen();
}
void Win32SerialPort::Close()
{
	m_port->close();

}

uint Win32SerialPort::GetBaudRate()
{
	return m_port->getBaudrate();
}

void Win32SerialPort::SetTimeOut(uint read_timeout, uint write_timeout)
{
	m_port->setTimeout(serial::Timeout::max(), read_timeout, 0, write_timeout, 0);
}
void Win32SerialPort::GetTimeOut(uint &read_timeout, uint &write_timeout)
{
	serial::Timeout to= m_port->getTimeout();
	read_timeout = to.read_timeout_constant;
	write_timeout = to.write_timeout_constant;
}

int Win32SerialPort::Write(const void* data,size_t size)
{
	return m_port->write((const unsigned char*)data, size);
}
int Win32SerialPort::Read(void* data,size_t size)
{
	return m_port->read((unsigned char*)data, size);
}

uint Win32SerialPort::AvailableData()
{
	return m_port->available();
}
bool Win32SerialPort::IsOpen()
{
	return m_port->isOpen();
}
void Win32SerialPort::Flush(bool flushIn,bool flushOut)
{
	if (flushIn)
		m_port->flushInput();
	if (flushOut)
		m_port->flushOutput();
}

IStream* Win32SerialPort::GetStream()
{
	return m_stream;
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

	std::vector<serial::PortInfo> ports=serial::list_ports();

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

