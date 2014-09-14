
/********************************************************************
	created:	2009/08/05
	created:	5:8:2009   6:46
	filename: 	i:\Programing\GameEngine\mrayEngine\mraySound\FSLSound.h
	file path:	i:\Programing\GameEngine\mrayEngine\mraySound
	file base:	FSLSound
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___FSLSound___
#define ___FSLSound___

#include <ISound.h>
#include <FreeSL.h>
#include "compileConfig.h"
#include "FSLSoundBuffer.h"
#include <queue>
namespace mray{
namespace sound{

	class FSLManager;

class MRAY_SOUND_DLL FSLSound:public ISound
{
private:
protected:
	FSLsound m_snd;
	bool m_isAttachedToListener;
	bool m_dieAfterEnd;
	ESoundType m_mode;
	math::vector3d lastPos;

	GCPtr<FSLSoundBuffer> m_data;
	FSLManager* m_manager;
	uint m_playedSamples;
	bool m_playing;

	std::queue<ISoundStreamFrame*> m_queueBuffers;
	std::queue<ISoundStreamFrame*> m_playingQueue;

public:
	FSLSound(FSLManager* manager,const core::string&name,FSLsound snd);
	virtual~FSLSound();

	virtual bool setPosition(const math::vector3d& pos);
	virtual math::vector3d getPosition();

	virtual bool setVelocity(const math::vector3d& vel);
	virtual math::vector3d getVelocity();

	virtual bool setDirection(const math::vector3d& dir);
	virtual math::vector3d getDirection();

	virtual bool setVolume(float vol);
	virtual float getVolume();

	virtual bool setMaxDistance(float max_dist);
	virtual float getMaxDistance();

	virtual bool setMinDistance(float min_dist);
	virtual float getMinDistance();

	virtual bool setPitch(float pitch);
	virtual float getPitch();

	virtual bool setLooping(bool loop);
	virtual bool getLooping();


	virtual bool isAttachedToListener();
	virtual bool AttachedToListener(bool a);

	virtual bool setDieAfterEnd(bool s);
	virtual bool isDieAfterEnd();

	virtual bool setMode(ESoundType mode);
	virtual ESoundType getMode();

	virtual bool isPlaying();
	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual void rewind();

	virtual void update(float dt);

	virtual void seek(float time);
	virtual uint getTimeLength();
	virtual uint getTimePosition();

	virtual void setSoundBuffer(GCPtr<ISoundStreamFrame>data);
	virtual GCPtr<ISoundStreamFrame> getSoundBuffer();

	virtual void QueueSoundBuffer(ISoundStreamFrame*data);
	virtual void UnqueueSoundBuffer(ISoundStreamFrame*data);

	void GetSamples(float* samples, float count, int channel){};
	void GetSpectrum(float* samples, float count, int channel, EFFT_WindowType window){};

	virtual xml::XMLElement* exportXMLSettings(xml::XMLElement* e);
	virtual xml::XMLElement* loadXMLSettings(xml::XMLElement*e);
};

}
}


#endif //___FSLSound___
