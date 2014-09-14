
#ifndef ___SFModSound___
#define ___SFModSound___


#include <mTypes.h>
#include <Point3d.h>
#include <mString.h>
#include <ISoundStreamFrame.h>

#include <fmod\fmod.hpp>
#include <fmod\fmod_errors.h>

#include <ISound.h>
#include <GCPtr.h>

#include "compileConfig.h"

namespace mray{
namespace sound
{

	class SFModSoundManager;

	class SFModSoundStream;
/*! 
   Sound Node Class
*/
class MRAY_SOUND_DLL SFModSound:public ISound
{
	math::vector3d lastPos;
	math::vector3d position;
	math::vector3d velocity;
	math::vector3d direction;
	float m_pitch;
	float m_volume;
	bool m_looping;
	float max_distance;
	float min_distance;

	FMOD::Channel*channel;

	SFModSoundManager*soundSystem;

	GCPtr<SFModSoundStream> dataFormat;
	bool m_bAttachToListener;
	bool dieAfterEnd;

public:
	SFModSound(SFModSoundManager*sndMngr,const core::string&name,math::vector3d pos=math::vector3d(0,0,0),math::vector3d vel=math::vector3d(0,0,0),math::vector3d dir=math::vector3d(0,0,0));
	virtual ~SFModSound();

	virtual bool setPosition(const math::vector3d& pos);
	virtual math::vector3d getPosition(){return position;}

	virtual bool setVelocity(const math::vector3d& vel);
	virtual math::vector3d getVelocity(){return velocity;}

	virtual bool setDirection(const math::vector3d& dir);
	virtual math::vector3d getDirection(){return direction;}

	virtual bool setVolume(float vol);
	virtual float getVolume(){return m_volume;}

	virtual bool setMaxDistance(float max_dist);
	virtual float getMaxDistance(){return max_distance;}

	virtual bool setMinDistance(float min_dist);
	virtual float getMinDistance(){return min_distance;}

	virtual bool setPitch(float pitch);
	virtual float getPitch();

	virtual bool setLooping(bool loop);
	virtual bool getLooping(){return m_looping;}

	virtual void setSoundBuffer(GCPtr<ISoundStreamFrame>data);
	virtual GCPtr<ISoundStreamFrame> getSoundBuffer();

	virtual void QueueSoundBuffer(ISoundStreamFrame*data);
	virtual void UnqueueSoundBuffer(ISoundStreamFrame*data);


	virtual bool isAttachedToListener(){return m_bAttachToListener;return true;}
	virtual bool AttachedToListener(bool a){m_bAttachToListener=a;return true;}

	virtual bool setDieAfterEnd(bool s){dieAfterEnd=s;return true;}
	virtual bool isDieAfterEnd(){return dieAfterEnd;}

	virtual bool setMode(ESoundType mode);
	virtual ESoundType getMode();

	virtual bool isPlaying();
	virtual void play();
	virtual void pause();
	virtual void stop();
	virtual void rewind();


	virtual void seek(float time);
	virtual uint getTimeLength();
	virtual uint getTimePosition();

	virtual void update(float dt);


	virtual void GetSamples(float* samples, float count, int channel) ;
	virtual void GetSpectrum(float* samples, float count, int channel, EFFT_WindowType window) ;
	
	virtual xml::XMLElement* exportXMLSettings(xml::XMLElement* e);
	virtual xml::XMLElement* loadXMLSettings(xml::XMLElement*e);

};

}
}


#endif


