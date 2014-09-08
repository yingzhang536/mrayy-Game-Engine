#include "stdafx.h"

#include "TheoraManager.h"
#include <IThreadManager.h>
#include "TheoraVideo.h"
#include "ISoundManager.h"
#include "ISound.h"
#include "ISoundStreamFrame.h"
#include "IFileSystem.h"
#include "ITimer.h"
#include "ILogManager.h"
#include <theoraplayer/TheoraDataSource.h>
#include "TheoraResourceLoader.h"


namespace mray{
namespace video{
	class TheoraAudioInterfaceImpl;

	class TheoraStreamDataSource:public TheoraDataSource
	{
		OS::IStream* m_stream;
	public:
		TheoraStreamDataSource(OS::IStream* s):m_stream(s)
		{
		}
		virtual int read(void* output,int nBytes)
		{
			return m_stream->read(output,nBytes);
		}
		//! returns a string representation of the DataSource, eg 'File: source.ogg'
		virtual std::string repr()		{
			return m_stream->getStreamName().c_str();
		}

		//! position the source pointer to byte_index from the start of the source
		virtual void seek(unsigned long byte_index)		{
			m_stream->seek(byte_index,OS::ESeek_Set);
		}

		//! return the size of the stream in bytes
		virtual unsigned long size()		{
			return m_stream->length();
		}

		//! return the current position of the source pointer
		virtual unsigned long tell()		{
			return m_stream->getPos();
		}

	};

	class TheoraAudioImpl:public TheoraAudioInterface
	{
	protected:
		int m_AudioBufferSize;
		int m_maxAudioBufferSize;
		short* m_AudioTempBuffer;
	public:
		sound::ISoundPtr snd;
		TheoraAudioInterfaceImpl* impl;
		TheoraAudioImpl(TheoraAudioInterfaceImpl*impl,sound::ISoundCRef s, TheoraVideoClip* o,int c,int f):TheoraAudioInterface(o,c,f),snd(s)
		{
			this->impl=impl;
			m_AudioBufferSize=0;
			m_maxAudioBufferSize=mFreq*mNumChannels*2;
			m_AudioTempBuffer=new short[m_maxAudioBufferSize];
		}
		virtual~TheoraAudioImpl()
		{
			delete [] m_AudioTempBuffer;
		}
		virtual void insertData(float* data, int nSamples);

		virtual void destroy() ;
	};
	class TheoraAudioInterfaceImpl:public TheoraAudioInterfaceFactory
	{
	public:
		sound::ISoundManager* mngr;
		std::list<TheoraAudioImpl*> m_toRemove;
		TheoraAudioInterfaceImpl()
		{
			mngr=0;
		}
		~TheoraAudioInterfaceImpl()
		{
			Flush();
		}

		void Flush()
		{
			std::list<TheoraAudioImpl*>::iterator it= m_toRemove.begin();
			for(;it!=m_toRemove.end();++it)
			{
				if(mngr)
				{
					mngr->removeSound((*it)->snd);
				}
				delete *it;
			}
			m_toRemove.clear();
		}
		virtual TheoraAudioInterface* createInstance(TheoraVideoClip* owner, int nChannels, int freq) 
		{
			if(!mngr)
				return 0;
			TheoraAudioImpl* impl=new TheoraAudioImpl(this,mngr->CreateManualSound(owner->getName().c_str()+core::string("_snd")),owner,nChannels,freq);

			return impl;
		}
	};
	void TheoraAudioImpl::destroy() 
	{
		delete [] m_AudioTempBuffer;
		m_AudioTempBuffer=0;
		impl->m_toRemove.push_back(this);
	}

	void TheoraAudioImpl::insertData(float* data, int nSamples)
	{
		for(int i=0;i<nSamples;++i)
		{
			if(m_AudioBufferSize<m_maxAudioBufferSize)
			{
				m_AudioTempBuffer[m_AudioBufferSize++]=(short)(math::clamp<float>(data[i],-1,1)*32767);
			}
			if(m_AudioBufferSize==mFreq*mNumChannels/10)
			{
				sound::ISoundStreamFrame* stream=impl->mngr->CreateManualStream();
				if (stream)
				{
					stream->SetStreamFrameData(m_AudioTempBuffer, m_AudioBufferSize, mFreq, sound::ESF_16, mNumChannels);

					snd->QueueSoundBuffer(stream);
				}
				m_AudioBufferSize = 0;
			}
		}
		if (snd && !snd->isPlaying())
			snd->play();
	}

	class TheoraData
	{
	public:
		virtual~TheoraData()
		{
			delete audioInterface;
			delete theoraManager;
		}
		TheoraAudioInterfaceImpl* audioInterface;
		TheoraVideoManager* theoraManager;
	};

TheoraManager::TheoraManager(int workers)
{
	m_data=new TheoraData();
	m_data->theoraManager=new TheoraVideoManager(workers);
	m_data->audioInterface=new TheoraAudioInterfaceImpl();
	m_data->theoraManager->setAudioInterfaceFactory(m_data->audioInterface);


	m_loaders.push_back(new OGGTheoraResourceLoader());
	m_loaders.push_back(new OGVTheoraResourceLoader());
	for(int i=0;i<m_loaders.size();++i)
		VideoResourceManager::getInstance().AddVideoLoader(m_loaders[i]);
}
TheoraManager::~TheoraManager()
{
	for(int i=0;i<m_loaders.size();++i)
	{
		VideoResourceManager::getInstance().RemoveVideoLoader(m_loaders[i]);
		delete m_loaders[i];
	}
	m_loaders.clear();
	ClearVideos();
	delete m_data;

}
void TheoraManager::setSoundManager(sound::ISoundManager*m)
{
	m_data->audioInterface->mngr=m;
}
sound::ISoundManager* TheoraManager::getSoundManager()
{
	return m_data->audioInterface->mngr;
}

IVideoSourcePtr TheoraManager::CreateVideo(const core::string& name,EVideoOutputMode output_mode,int numPrecachedOverride,bool usePower2Stride)
{
	TheoraVideoClip* clip= m_data->theoraManager->createEmptyVideoClip(name.c_str(),(TheoraOutputMode)output_mode,numPrecachedOverride,usePower2Stride);
	GCPtr<TheoraVideo> ret=new TheoraVideo(clip);
	m_videos.push_back(ret);
	return ret;
}
IVideoSourcePtr TheoraManager::CreateVideoFromFile(const core::string& fileName,EVideoOutputMode output_mode,int numPrecachedOverride,bool usePower2Stride)
{
	core::string path;
	gFileSystem.getCorrectFilePath(fileName,path);
	if(path=="")
	{
		gLogManager.log("TheoraManager::CreateVideoFromFile - failed to load video from file: "+ fileName,ELL_WARNING);
		return IVideoSourcePtr::Null;
	}
	TheoraVideoClip* clip= m_data->theoraManager->createVideoClip(path.c_str(),(TheoraOutputMode)output_mode,numPrecachedOverride,usePower2Stride);
	GCPtr<TheoraVideo> ret=new TheoraVideo(clip);
	ret->createVideoBuffers();

	m_videos.push_back(ret);
	return ret;

}

IVideoSourcePtr TheoraManager::CreateVideoFromStream(OS::IStream* stream,EVideoOutputMode output_mode,int numPrecachedOverride,bool usePower2Stride)
{

	TheoraVideoClip* clip= m_data->theoraManager->createVideoClip(&TheoraStreamDataSource(stream),(TheoraOutputMode)output_mode,numPrecachedOverride,usePower2Stride);
	GCPtr<TheoraVideo> ret=new TheoraVideo(clip);
	ret->createVideoBuffers();

	m_videos.push_back(ret);
	return ret;
}

void TheoraManager::RemoveVideo(IVideoSource* v)
{

	TheoraVideo* tv=(TheoraVideo*)v;
	m_data->theoraManager->destroyVideoClip(tv->getClip());
	VideoList::iterator it;
	for(it=m_videos.begin();it!=m_videos.end();++it)
	{
		if((*it).pointer()==v)
		{
			m_videos.erase(it);
			break;
		}
	}
}

void TheoraManager::ClearVideos()
{
	VideoList::iterator it;
	for(it=m_videos.begin();it!=m_videos.end();++it)
	{
		TheoraVideo* tv=(TheoraVideo*)(*it).pointer();
		m_data->theoraManager->destroyVideoClip(tv->getClip());
	}
	m_videos.clear();
}
void TheoraManager::Update(float dt)
{
	m_data->audioInterface->Flush();
	m_data->theoraManager->update(dt);
}

}
}

