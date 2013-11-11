#include "stdafx.h"

#include "SFModSoundStream.h"

#include "SFModSoundManager.h"

namespace mray
{
namespace sound
{


SFModSoundStream::SFModSoundStream(const core::string&name):m_sound(0)
{
	setResourceName(name);
	m_isStreaming=0;
	m_streamData=0;
}

SFModSoundStream::~SFModSoundStream(){
	if(m_sound){
		m_sound->release();
		m_sound=0;
	}
	if(m_streamData){
		delete [] m_streamData;
	}
}


uint SFModSoundStream::GetChannels()
{
	return 0;
}
uint  SFModSoundStream::GetFrequency()
{
	return 0;
}
ESampleFormat SFModSoundStream::GetFormat()
{
	return ESF_8;
}

bool SFModSoundStream::SetStreamFrameData(void* data,uint size,uint freq,ESampleFormat fmt,int channels)
{
	return false;
}


void SFModSoundStream::seek(float time){
	m_sound->seekData((uint)time);

}
int SFModSoundStream::streamData(int size){
	return 0;
}
void* SFModSoundStream::GetSamples(){
	return 0;
}
uint SFModSoundStream::GetSamplesCount(){
	return 0;
}
ISoundStreamFrame* SFModSoundStream::duplicate()
{
	return manager->createDataFile(filename.c_str(),m_isStreaming,type);
}


}
}
