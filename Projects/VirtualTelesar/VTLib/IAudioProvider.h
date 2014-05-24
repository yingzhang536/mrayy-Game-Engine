
#ifndef IAudioProvider_h__
#define IAudioProvider_h__

#include "ICommunicationProvider.h"
#include "ISoundStreamFrame.h"

namespace mray
{
namespace VT
{


class IAudioProvider:public ICommunicationProvider
{
	DECLARE_RTTI;
protected:
public:
	static const GUID ProviderType;
public:
	IAudioProvider(){}
	virtual~IAudioProvider(){}

	virtual GUID GetProviderType() const { return ProviderType; }

	//
	virtual sound::ISoundStreamFrame* GetLastSoundFrame(int i) = 0;

	// Number of audio sources this provider has
	virtual int GetSourcesCount() = 0;
};

}
}

#endif // IAudioProvider_h__
