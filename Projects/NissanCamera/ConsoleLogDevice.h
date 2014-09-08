

#ifndef ConsoleLogDevice_h__
#define ConsoleLogDevice_h__

#include "ILogDevice.h"
#include "GUIConsole.h"

namespace mray
{
namespace NCam
{
	
class ConsoleLogDevice:public ILogDevice
{
protected:

	GUI::GUIConsole* m_console;

public:
	ConsoleLogDevice(GUI::GUIConsole*c):m_console(c){}
	virtual ~ConsoleLogDevice(){}

	virtual void log(const core::string &msg, const core::string &info, ELogLevel level) ;
	virtual void log(const core::string &msg, ELogLevel level) ;

	virtual void startSection(const core::string &name) ;
	virtual void endSection(bool Success) ;

	virtual void close() ;

	virtual void flush() ;
	
};

}
}

#endif // ConsoleLogDevice_h__
