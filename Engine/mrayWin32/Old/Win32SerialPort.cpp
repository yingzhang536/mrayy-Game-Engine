
#include "stdafx.h"
#include "Win32SerialPort.h"
#include "IThreadFunction.h"
#include "IThreadManager.h"


namespace mray
{
namespace OS
{

	class SerialPortThreadFunc :public OS::IThreadFunction
	{
		OS::ISerialPort* m_sp;
	public:
		SerialPortThreadFunc(OS::ISerialPort* sp)
		{
			m_sp = sp;
		}
		virtual void execute(OS::IThread*caller, void*arg)
		{
			while (caller->isActive() && m_sp->IsOpen())
			{
				uint count = 0;
				if (m_sp->WaitForData() && (count = m_sp->AvailableData()) > 0)
				{
					char* buffer = new char[count];
					m_sp->Read(buffer, count);
					m_sp->OnDataArrived(m_sp, buffer, count);
					delete[] buffer;
				}
				else
					OS::IThreadManager::getInstance().sleep(100);
			}
		}
	};
Win32SerialPort::Win32SerialPort()
{
	m_port = new serial::Serial();
	m_stream=new SerialPortStream(this);
	m_thread = OS::IThreadManager::getInstance().createThread(new SerialPortThreadFunc(this));
}
Win32SerialPort::~Win32SerialPort()
{
	Close();
	OS::IThreadManager::getInstance().killThread(m_thread);
	delete m_thread;
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
	if (m_port->isOpen())
	{
		m_thread->start(0);
	}
	return m_port->isOpen();
}
void Win32SerialPort::Close()
{
	m_port->close();
	m_thread->terminate();

}
bool Win32SerialPort::WaitForData()
{
	return m_port->waitReadable();
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

