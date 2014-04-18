

#ifndef ___SFModSoundManager___
#define ___SFModSoundManager___


#include <ISoundManager.h>
#include <mArray.h>

#include <ObjectFactory.h>

#	include <fmod\fmod.hpp>
#	include <fmod\fmod_errors.h>

#include "compileConfig.h"

namespace mray{

namespace sound{

	class SFModSoundStream;
/*! 
  add/remove sounds
*/
class MRAY_SOUND_DLL SFModSoundManager:public ISoundManager
{
	
	FMOD::System     *m_system;

	bool reportError(FMOD_RESULT error);

	sound::ISoundListener* m_listener;

	float distance_factor;
	float m_dfactor;
	float m_dvelocity;

	SoundsList soundsList;

	std::vector<ISoundStreamFrame*>sndDataFiles;
	ISoundStreamFrame*getSound(const core::string&filename);


	FMOD::ChannelGroup *m_music;
	FMOD::ChannelGroup *m_statePlaying;
	FMOD::ChannelGroup *m_stateStopped;
	FMOD::ChannelGroup *m_stateStartup;

public:
	SFModSoundManager();
	virtual ~SFModSoundManager();


	FMOD::System*getFModSystem();

	virtual ISoundPtr CreateManualSound(const core::string&name);

	virtual ISoundStreamFrame* CreateManualStream();
	SFModSoundStream* createDataFile(const core::string&filename,bool isStream,ESoundType type);
	SFModSoundStream* getOrCreate(const core::string&filename,bool isStream,ESoundType type);

	ISoundPtr playSound(const core::string&sndName,const math::vector3d& pos,bool isStream,float vol,bool loop=0,ESoundType type=ESNDT_3D);

	ISoundPtr loadSound(const core::string&filename,bool isStream,ESoundType type=ESNDT_3D);

	ISoundListener* createListener();
	void setActiveListener(ISoundListener* l);
	ISoundListener* getActiveListener();


	void setVolume(float v);

	int getCurrentSoundsStreaming();

	int getMaxSounds();

	ulong getUsedMemory();

	const SoundsList&getSoundsList(){return soundsList;}

	void addSound(ISoundCRef snd);
	void removeSound(ISound*snd);

	void setDistanceFactor(float factor);
	float getDistanceFactor(){return distance_factor;}

	void runSounds(float dt);


	void setDopplerParameters(float factor,float velocity);
	void setSpeedOfSound(float v);
	void setMetersPerUnit(float meters);


	void stopAllSounds();
	void playAllSounds();
	void pauseAllSounds();
	void resumeAllSounds();

	void Clear();
};



class MRAY_SOUND_DLL FModManagerCreater:public ObjectFactory<ISoundManager>
{
public:
	static const core::string Type;

	virtual const core::string& GetType();
	virtual ISoundManager* CreateObject();
	virtual core::string getDescription();
};


}
}


#endif




