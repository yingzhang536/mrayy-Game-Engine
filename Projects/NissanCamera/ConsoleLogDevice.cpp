
#include "stdafx.h"
#include "ConsoleLogDevice.h"


namespace mray
{
namespace NCam
{


void ConsoleLogDevice::log(const core::string &msg, const core::string &info, ELogLevel level)
{
	log(msg + ":" + info, level);
}
void ConsoleLogDevice::log(const core::string &msg, ELogLevel level)
{
	m_console->AddToHistory(msg ,video::SColor(1));

}

void ConsoleLogDevice::startSection(const core::string &name)
{

}
void ConsoleLogDevice::endSection(bool Success)
{

}

void ConsoleLogDevice::close()
{

}

void ConsoleLogDevice::flush()
{

}

}
}