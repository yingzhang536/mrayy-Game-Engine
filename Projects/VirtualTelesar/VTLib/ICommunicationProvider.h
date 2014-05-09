
#ifndef ICommunicationProvider_h__
#define ICommunicationProvider_h__

#include <RTTI.h>

namespace mray
{
namespace VT
{
	enum class EProviderStatus
	{
		Waiting,
		Connected,
		Failed
	};
	// base class for communication providers for the robot.
	// When connecting to a robot, the robot will ask for a set of providers to be initiated
	// Providers such as: Vision, Control, Audio, ... etc
class ICommunicationProvider
{
	DECLARE_RTTI;
protected:
public:
	ICommunicationProvider(){}
	virtual~ICommunicationProvider(){}

	virtual GUID GetProviderType() const = 0;

	virtual const core::string& GetProviderName() const= 0;

	virtual void ConnectToRobot() = 0;

	virtual EProviderStatus QueryState() = 0;

};



}
}

#endif // ICommunicationProvider_h__

