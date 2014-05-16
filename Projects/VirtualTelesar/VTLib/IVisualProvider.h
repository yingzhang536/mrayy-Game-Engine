

#ifndef IVisualProvider_h__
#define IVisualProvider_h__

#include "ICommunicationProvider.h"
#include "ITexture.h"

namespace mray
{
namespace VT
{


class IVisualProvider :public ICommunicationProvider
{
	DECLARE_RTTI;
protected:
public:
	static const GUID ProviderType;
public:
	IVisualProvider(){}
	virtual~IVisualProvider(){}

	virtual GUID GetProviderType() const { return ProviderType; }

	// retrieve the last i'th picture 
	virtual video::ITextureCRef GetLastPicture(int i) = 0;

	// Number of cameras this provider has
	virtual int GetSourcesCount() = 0;
};


}
}

#endif // IVisualProvider_h__
