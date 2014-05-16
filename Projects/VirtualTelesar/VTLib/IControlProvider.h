



/********************************************************************
	created:	2014/05/14
	created:	14:5:2014   21:04
	filename: 	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib\IControlProvider.h
	file path:	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib
	file base:	IControlProvider
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IControlProvider__
#define __IControlProvider__


#include "ICommunicationProvider.h"


namespace mray
{
namespace VT
{


class IControlProvider :public ICommunicationProvider
{
	DECLARE_RTTI;
protected:
public:
	static const GUID ProviderType;
public:
	IControlProvider(){}
	virtual~IControlProvider(){}

	virtual GUID GetProviderType() const { return ProviderType; }

};


}
}

#endif
