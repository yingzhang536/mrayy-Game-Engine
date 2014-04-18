#include "stdafx.h"
#include "FSLSound.h"

#include "FSLManager.h"

namespace mray{
namespace sound{

FSLSound::FSLSound(FSLManager* manager,const core::string&name,FSLsound snd):ISound(name)
{
	m_snd=snd;
	m_isAttachedToListener=0;
	m_dieAfterEnd=0;
	m_data=0;
	m_manager=manager;
	m_playedSamples=0;
	m_playing=false;
}
FSLSound::~FSLSound()
{
	std::queue<ISoundStreamFrame*>::container_type::const_iterator it= m_queueBuffers._Get_container().begin();
	for(;it!=m_queueBuffers._Get_container().end();++it)
	{
		FSLbuffer b= ((FSLSoundBuffer*)(*it))->getBuffer();
		fslSoundUnqueueBuffers(m_snd,1,&b);
		delete *it;
	}

	for(it=m_playingQueue._Get_container().begin();it!=m_playingQueue._Get_container().end();++it)
	{
		FSLbuffer b= ((FSLSoundBuffer*)(*it))->getBuffer();
		fslSoundUnqueueBuffers(m_snd,1,&b);
		delete *it;
	}
	fslFreeSound(m_snd,false);
}

bool FSLSound::setPosition(const math::vector3d& pos)
{
	fslSoundSetPosition(m_snd,pos.x,pos.y,pos.z);
	return true;
}
math::vector3d FSLSound::getPosition(){
	math::vector3d v;
	fslSoundGetPosition(m_snd,v.x,v.y,v.z);
	return v;
}

bool FSLSound::setVelocity(const math::vector3d& vel)
{
	fslSoundSetVelocity(m_snd,vel.x,vel.y,vel.z);
	return true;
}
math::vector3d FSLSound::getVelocity()
{
	math::vector3d vel;
	fslSoundGetVelocity(m_snd,vel.x,vel.y,vel.z);
	return vel;
}

bool FSLSound::setDirection(const math::vector3d& dir)
{
	fslSoundSetConeOrientation(m_snd,dir.x,dir.y,dir.z);
	return true;
}
math::vector3d FSLSound::getDirection()
{
	math::vector3d dir;
	fslSoundGetConeOrientation(m_snd,dir.x,dir.y,dir.z);
	return dir;
}

bool FSLSound::setVolume(float vol)
{
	fslSoundSetGain(m_snd,vol);
	return true;
}
float FSLSound::getVolume()
{
	return fslSoundGetGain(m_snd);
}

bool FSLSound::setMaxDistance(float max_dist){

	fslSoundSetMaxDistance(m_snd,max_dist);
	return true;
}
float FSLSound::getMaxDistance()
{
	return fslSoundGetMaxDistance(m_snd);
}

bool FSLSound::setMinDistance(float min_dist)
{
	return true;
}
float FSLSound::getMinDistance()
{
	return 1;
}

bool FSLSound::setPitch(float pitch)
{
	fslSoundSetPitch(m_snd,pitch);
	return true;
}
float FSLSound::getPitch()
{
	return fslSoundGetPitch(m_snd);
}

bool FSLSound::setLooping(bool loop)
{
	fslSoundSetLooping(m_snd,loop);
	return true;
}
bool FSLSound::getLooping()
{
	return fslSoundIsLooping(m_snd);
}


bool FSLSound::isAttachedToListener()
{
	return m_isAttachedToListener;
}
bool FSLSound::AttachedToListener(bool a)
{
	m_isAttachedToListener=a;
	return true;
}

bool FSLSound::setDieAfterEnd(bool s)
{
	m_dieAfterEnd=s;
	return true;
}
bool FSLSound::isDieAfterEnd(){
	return m_dieAfterEnd;
}

bool FSLSound::setMode(ESoundType mode)
{
	m_mode=mode;
	return true;
}
ESoundType FSLSound::getMode()
{
	return m_mode;
}
bool FSLSound::isPlaying(){
	//return fslSoundIsPlaying(m_snd);
	return m_playing || fslSoundIsPlaying(m_snd);
}
void FSLSound::play(){
	fslSoundPlay(m_snd);
	m_playing=true;
}
void FSLSound::pause(){
	fslSoundPause(m_snd);
	m_playing=false;
}
void FSLSound::stop(){
	fslSoundStop(m_snd);
	m_playing=false;
}
void FSLSound::rewind(){
	fslSoundRewind(m_snd);
}

void FSLSound::update(float dt){
	math::vector3d position=getPosition();

	math::vector3d velocity=(lastPos-position)*dt;
	setVelocity(velocity);

	lastPos=position;

	if(isPlaying())
	{
		int cnt=fslSoundGetProcessedBuffersCount(m_snd);
		while(cnt-- && m_playingQueue.size()>0)
		{
			ISoundStreamFrame* stream= m_playingQueue.front();
			FSLSoundBuffer*data=dynamic_cast<FSLSoundBuffer*>(stream);
			FSLbuffer buf=data->getBuffer();
			//printf("<-:%d\n",(int)buf);
			fslSoundUnqueueBuffers(m_snd,1,&buf);
			m_playedSamples+=data->GetSamplesCount();
			delete data;
			m_playingQueue.pop();
		}
		if(!fslSoundIsPlaying(m_snd))
			fslSoundPlay(m_snd);
		if(m_queueBuffers.size())
		{
			ISoundStreamFrame* stream= m_playingQueue.back();
			FSLSoundBuffer*data=dynamic_cast<FSLSoundBuffer*>(stream);
			FSLbuffer buf=data->getBuffer();
			fslSoundQueueBuffers(m_snd,1,&buf);
			if(!fslSoundIsPlaying(m_snd))
				fslSoundPlay(m_snd);
			m_queueBuffers.pop();
			m_playingQueue.push(data);
			//printf("Q:%d\n",buf);
		}else
		{
			//if(m_playingQueue.empty())
			//	stop();
		}
	}
}


void FSLSound::seek(float time){
	fslSoundSetBufferPosition(m_snd,time);
}
uint FSLSound::getTimeLength(){
	return fslSoundGetBufferLength(m_snd);
}
uint FSLSound::getTimePosition(){
	return fslSoundGetBufferPosition(m_snd);
}

void FSLSound::setSoundBuffer(ISoundStreamFrame*data){
	m_data=dynamic_cast<FSLSoundBuffer*>(data);
	if(m_data){
		fslSetSoundBuffer(m_snd,m_data->getBuffer());
	}else
		fslSetSoundBuffer(m_snd,0);
}
ISoundStreamFrame*FSLSound::getSoundBuffer(){
	return m_data;
}
void FSLSound::QueueSoundBuffer(ISoundStreamFrame*data)
{
	FSLSoundBuffer*d=dynamic_cast<FSLSoundBuffer*>(data);
	if(d){
		FSLbuffer b=d->getBuffer();
		fslSoundQueueBuffers(m_snd,1,&b);
		m_playingQueue.push(d);
		//printf("->:%d\n",(int)b);
	}
}
void FSLSound::UnqueueSoundBuffer(ISoundStreamFrame*data)
{
	FSLSoundBuffer*d=dynamic_cast<FSLSoundBuffer*>(data);
	if(m_data){
		FSLbuffer b=d->getBuffer();
		fslSoundUnqueueBuffers(m_snd,1,&b);
		std::queue<ISoundStreamFrame*>::container_type::const_iterator it= m_queueBuffers._Get_container().begin();
		for(;it!=m_queueBuffers._Get_container().end();++it)
		{
			if(*it==data)
			{
				//m_queueBuffers._Get_container().erase(it);
				break;
			}
		}
	}
}

xml::XMLElement* FSLSound::exportXMLSettings(xml::XMLElement* e)
{
	xml::XMLElement* elem=ISound::exportXMLSettings(e);
	if(m_data)
	{
		elem->addAttribute(mT("Source"),m_data->fileName);
	}

	return elem;
}
xml::XMLElement* FSLSound::loadXMLSettings(xml::XMLElement*e)
{
	xml::XMLElement* elem=ISound::loadXMLSettings(e);
	xml::XMLAttribute*attr;
	attr=e->getAttribute(mT("Source"));
	if(attr){
		core::string file=attr->value;
		bool streaming=false;
		setSoundBuffer(m_manager->getOrCreate(file,streaming,getMode()));
	}

	return elem;
}


}
}


