#pragma once

#include "mString.h"

#include "videoCommon.h"
#include "ImageInfo.h"
#include "IMutex.h"

class ofGstAppSink;
typedef struct _GstElement GstElement;
typedef struct _GstBuffer GstBuffer;
typedef struct _GstMessage GstMessage;


using namespace mray;

//-------------------------------------------------
//----------------------------------------- ofGstUtils
//-------------------------------------------------

class ofGstUtilsImpl;

class ofGstUtils{
public:
	ofGstUtils();
	virtual ~ofGstUtils();

	bool 	setPipelineWithSink(const core::string& pipeline, const core::string& sinkname = "sink", bool isStream = false);
	bool 	setPipelineWithSink(GstElement * pipeline, GstElement * sink, bool isStream = false);
	bool	startPipeline();

	void 	play();
	void 	stop();
	void 	setPaused(bool bPause);
	bool 	isPaused();
	bool 	isLoaded();
	bool 	isPlaying();

	float	getPosition();
	float 	getSpeed();
	float 	getDuration();
	ulong  getDurationNanos();
	bool  	getIsMovieDone();

	void 	setPosition(float pct);
	void 	setVolume(float volume);
	void 	setLoopState(int state);
	int	getLoopState();
	void 	setSpeed(float speed);

	void 	setFrameByFrame(bool bFrameByFrame);
	bool	isFrameByFrame();

	GstElement 	* getPipeline();
	GstElement 	* getSink();
	GstElement 	* getGstElementByName(const core::string & name);
	ulong getMinLatencyNanos();
	ulong getMaxLatencyNanos();

	virtual void close();


	void setSinkListener(ofGstAppSink * appsink);

	static void startGstMainLoop();


	virtual int preroll_cb(GstBuffer * buffer);
	virtual int buffer_cb(GstBuffer * buffer);
	virtual void eos_cb();
protected:
	ofGstUtilsImpl* m_impl;




private:

};





//-------------------------------------------------
//----------------------------------------- videoUtils
//-------------------------------------------------

class ofGstVideoUtils : public  ofGstUtils{
public:

	ofGstVideoUtils();
	virtual ~ofGstVideoUtils();

	bool 			setPipeline(const core::string& pipeline, int bpp = 24, bool isStream = false, int w = -1, int h = -1);

	bool 			allocate(int w, int h, int bpp);

	bool 			isFrameNew();
	unsigned char * getPixels();
	video::ImageInfo*	getPixelsRef();
	void 			update();

	float 			getHeight();
	float 			getWidth();

	void 			close();

	// this events happen in a different thread
	// do not use them for opengl stuff
	/*ofEvent<ofPixels> prerollEvent;
	ofEvent<ofPixels> bufferEvent;
	ofEvent<ofEventArgs> eosEvent;*/

protected:
#if GST_VERSION_MAJOR==0
	// GstFlowReturn
	virtual int preroll_cb(GstBuffer * buffer);
	virtual int buffer_cb(GstBuffer * buffer);
#else
	GstFlowReturn preroll_cb(GstSample * buffer);
	GstFlowReturn buffer_cb(GstSample * buffer);
#endif
	void			eos_cb();


	video::ImageInfo		pixels;				// 24 bit: rgb
	video::ImageInfo		backPixels;
	video::ImageInfo		eventPixels;
private:
	bool			bIsFrameNew;			// if we are new
	bool			bHavePixelsChanged;
	bool			bBackPixelsChanged;
	OS::IMutex*			mutex;
#if GST_VERSION_MAJOR==0
	GstBuffer * 	buffer, *prevBuffer;
#else
	GstSample * 	buffer, *prevBuffer;
	GstMapInfo mapinfo;
#endif
};


//-------------------------------------------------
//----------------------------------------- appsink listener
//-------------------------------------------------

class ofGstAppSink{
public:
	virtual ~ofGstAppSink(){}
#if GST_VERSION_MAJOR==0
	virtual int on_preroll(GstBuffer * buffer);
	virtual int on_buffer(GstBuffer * buffer);
#else
	virtual GstFlowReturn on_preroll(GstSample * buffer){
		return GST_FLOW_OK;
	}
	virtual GstFlowReturn on_buffer(GstSample * buffer){
		return GST_FLOW_OK;
	}
#endif
	virtual void			on_eos(){}

	// return true to set the message as attended so upstream doesn't try to process it
	virtual bool on_message(GstMessage* msg){ return false; };

	// pings when enough data has arrived to be able to get sink properties
	virtual void on_stream_prepared(){};
};

