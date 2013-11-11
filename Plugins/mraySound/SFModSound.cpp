#include "stdafx.h" 

#include "SFModSound.h"
#include "SFModSoundManager.h"
#include "SFModSoundStream.h"
 
namespace mray{
namespace sound{

static int BUFFER_SIZE=65536;



SFModSound::SFModSound(SFModSoundManager*sndMngr,const core::string&name,math::vector3d pos,math::vector3d vel,math::vector3d dir):
ISound(name),
soundSystem(sndMngr),m_pitch(1),m_volume(1),m_looping(0),m_bAttachToListener(0),dieAfterEnd(0),dataFormat(0)
,channel(0)
{
/*
#ifdef ___DEBUG___
	setDebugName("SFModSound");
#endif 
*/
	setPosition(pos);
	setVelocity(vel);
	setDirection(dir);
	setVolume(m_volume);
	setMinDistance(1);
	setMaxDistance(10);
}
SFModSound::~SFModSound(){
	stop();
	if(dataFormat)
		dataFormat->m_sound=0;
}

bool SFModSound::setPosition(const math::vector3d& pos){
	position=pos;
	if(isPlaying()){
		channel->set3DAttributes((FMOD_VECTOR*)&pos,(FMOD_VECTOR*)&velocity);
	}
	return true;
}


bool SFModSound::setVelocity(const math::vector3d& vel){
	velocity=vel;
	if(isPlaying())
		channel->set3DAttributes((FMOD_VECTOR*)&position,(FMOD_VECTOR*)&velocity);
	return true;

}

bool SFModSound::setDirection(const math::vector3d& dir){
	direction=dir;
	return true;
}

bool SFModSound::setPitch(float pitch){
	if(pitch<0)
		return false;//should be positive
	m_pitch=pitch;
	channel->setFrequency(pitch);
	return true;
}

float SFModSound::getPitch(){
	channel->getFrequency(&m_pitch);
	return m_pitch;
}

bool SFModSound::setVolume(float vol){
	if(vol<0)
		return false;//should be positive
	m_volume=vol;
	if(isPlaying()){
		channel->setVolume(vol);
	}
	return true;
}
 
bool SFModSound::setLooping(bool loop){
	m_looping=loop;
	if(isPlaying())
		channel->setMode(m_looping? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	return true;

}

bool SFModSound::setMaxDistance(float max_dist){
	if(max_dist<0)
		return false;
	max_distance=max_dist;
	if(!dataFormat)
		return false;

	
	dataFormat->m_sound->set3DMinMaxDistance(min_distance*soundSystem->getDistanceFactor(),max_distance*soundSystem->getDistanceFactor());
	if(isPlaying())
		channel->set3DMinMaxDistance(min_distance*soundSystem->getDistanceFactor(),max_distance*soundSystem->getDistanceFactor());

	return true;
}
bool SFModSound:: setMinDistance(float min_dist){
	if(min_dist<0)
		return false;
	min_distance=min_dist;
	if(!dataFormat)
		return false;

	dataFormat->m_sound->set3DMinMaxDistance(min_distance*soundSystem->getDistanceFactor(),max_distance*soundSystem->getDistanceFactor());
	if(isPlaying())
		channel->set3DMinMaxDistance(min_distance*soundSystem->getDistanceFactor(),max_distance*soundSystem->getDistanceFactor());

	return true;
}

void SFModSound::setSoundBuffer(ISoundStreamFrame*data)
{
	dataFormat=dynamic_cast<SFModSoundStream*>(data);
}


ISoundStreamFrame*SFModSound::getSoundBuffer()
{
	return dataFormat;
}


void SFModSound::QueueSoundBuffer(ISoundStreamFrame*data)
{
}
void SFModSound::UnqueueSoundBuffer(ISoundStreamFrame*data)
{
}
bool SFModSound::setMode(ESoundType mode)
{
	return true;
}
ESoundType SFModSound::getMode()
{
	if(!dataFormat)
		return ESNDT_3D;
	return dataFormat->type;
}

bool SFModSound::isPlaying(){
	
	if(!channel)return 0;
	bool res=0;
	channel->isPlaying(&res);
	return res;
}


void SFModSound::play(){
	if(isPlaying() || !soundSystem)return;
	
	FMOD::System*sys=soundSystem->getFModSystem();//dataFormat->m_sound->(FSOUND_FREE,dataFormat->m_sound,0,1);
	sys->playSound(FMOD_CHANNEL_FREE,dataFormat->m_sound,true,&channel);

	channel->set3DAttributes((FMOD_VECTOR*)&position,(FMOD_VECTOR*)&velocity);
	channel->setVolume(m_volume);
	channel->set3DMinMaxDistance(min_distance*soundSystem->getDistanceFactor(),max_distance*soundSystem->getDistanceFactor());
	channel->setMode(m_looping? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	if(!channel->setPaused(false))
		return;///error happend
}

void SFModSound::pause(){
	if(channel)
		channel->setPaused(1);
}


void SFModSound::stop(){

	if(channel)
		channel->stop();
}
void SFModSound::rewind(){
	stop();
	play();
}


void SFModSound::seek(float time){
	if(dataFormat)
		dataFormat->seek(time);
}
uint SFModSound::getTimeLength(){
	if(!dataFormat)
		return 0;
	uint len=0;
	dataFormat->m_sound->getLength(&len,FMOD_TIMEUNIT_MS);
	return len;
}
uint SFModSound::getTimePosition(){
	return 0;
}


void SFModSound::update(float dt){
	if(m_looping && !isPlaying())
		play();
	velocity=(lastPos-position)*dt;
	setVelocity(velocity);
	if(lastPos!=position){
	}
	lastPos=position;

}

xml::XMLElement* SFModSound::exportXMLSettings(xml::XMLElement* e)
{
	xml::XMLElement* elem=ISound::exportXMLSettings(e);
	if(dataFormat)
	{
		elem->addAttribute(mT("Source"),dataFormat->filename);
		elem->addAttribute(mT("Streaming"),core::StringConverter::toString(dataFormat->m_isStreaming));
	}

	return elem;
}
xml::XMLElement* SFModSound::loadXMLSettings(xml::XMLElement*e)
{
	xml::XMLElement* elem=ISound::loadXMLSettings(e);
	xml::XMLAttribute*attr;
	attr=e->getAttribute(mT("Source"));
	if(attr){
		core::string file=attr->value;
		bool streaming=false;
		attr=e->getAttribute(mT("Streaming"));
		if(attr)
			streaming=core::StringConverter::toBool(attr->value);
		setSoundBuffer(soundSystem->getOrCreate(file,streaming,getMode()));
	}
	return elem;

}

}
}

