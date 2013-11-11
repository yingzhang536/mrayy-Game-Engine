

/********************************************************************
	created:	2011/01/16
	created:	16:1:2011   21:38
	filename: 	i:\Programing\GameEngine\mrayEngine\mraySound\SFModSoundStream.h
	file path:	i:\Programing\GameEngine\mrayEngine\mraySound
	file base:	SFModSoundStream
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef SFModSoundStream_h__
#define SFModSoundStream_h__

#include "CompileConfig.h"
#include "ISoundStreamFrame.h"

#include <fmod\fmod.hpp>
#include <fmod\fmod_errors.h>

namespace mray
{
namespace sound
{
	class SFModSoundManager;

class MRAY_SOUND_DLL SFModSoundStream:public ISoundStreamFrame
{
private:
protected:
public:
	FMOD::Sound*m_sound;
	bool m_isStreaming;
	ESoundType type;
	core::string filename;
	SFModSoundManager*manager;

	char* m_streamData;

	SFModSoundStream(const core::string&name);

	virtual~SFModSoundStream();


	virtual void* GetSamples();

	virtual uint GetSamplesCount();
	virtual uint GetFrequency();
	virtual uint GetChannels();
	virtual ESampleFormat GetFormat();

	virtual bool SetStreamFrameData(void* data,uint size,uint freq,ESampleFormat fmt,int channels);


	virtual void seek(float time);
	virtual int streamData(int size);
	virtual ISoundStreamFrame* duplicate();
};

}
}

#endif // SFModSoundStream_h__
