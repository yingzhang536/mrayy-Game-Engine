#ifndef OFGSTUTILS_H_
#define OFGSTUTILS_H_

#include <gst/gst.h>
#include <pthread.h>
#include <mstring.h>
//#include "ofConstants.h"
using namespace mray;

typedef struct{
	GMainLoop 		*	loop;
	GstElement 		*	pipeline;
	unsigned char 	*	pixels;				// 24 bit: rgb
	unsigned			width, height;
	unsigned			totalsize;
	pthread_mutex_t 	buffer_mutex;
	bool				bHavePixelsChanged;

	guint64				durationNanos;
	guint64				nFrames;
	int					pipelineState;
	float				speed;

	guint64				lastFrame;
}ofGstVideoData;

typedef struct
{
  int numerator;
  int denominator;
} ofGstFramerate;

typedef struct
{
  core::string mimetype;
  int    width;
  int    height;
  std::vector<ofGstFramerate> framerates;
  ofGstFramerate choosen_framerate;
} ofGstVideoFormat;

typedef struct
{
  core::string video_device;
  core::string gstreamer_src;
  core::string product_name;
  std::vector<ofGstVideoFormat> video_formats;
  int current_format;
} ofGstDevice;

typedef struct
{
  std::vector<ofGstDevice> webcam_devices;
  bool bInited;
} ofGstCamData;

class ofGstUtils {
public:
	ofGstUtils();
	virtual ~ofGstUtils();

	bool loadMovie(core::string uri);
	bool Connect(const core::string& ip,int remotePort,int localPort);

	void listDevices();
	void setDeviceID(unsigned id);
	bool initGrabber(int w, int h, int framerate=-1);

	bool setPipeline(core::string pipeline, int bpp=3, bool isStream = false);
	bool setPipelineWithSink(core::string pipeline);

	bool isFrameNew();
	unsigned char * getPixels();
	void update();

	void play();
	void setPaused(bool bPause);

	int	getCurrentFrame();
	int	getTotalNumFrames();

	void firstFrame();
	void nextFrame();
	void previousFrame();

	int getHeight();
	int getWidth();

	float getPosition();
	float getSpeed();
	float getDuration();
	bool  getIsMovieDone();

	void setPosition(float pct);
	void setVolume(int volume);
	void setLoopState(int state);
	void setSpeed(float speed);
	void setFrame(int frame);  // frame 0 = first frame...

	void setFrameByFrame(bool bFrameByFrame);

	void close();

protected:
	void                seek_lock();
	void                seek_unlock();
	void 				gstHandleMessage();
	bool 				allocate();
	bool				startPipeline();
	ofGstVideoFormat&	selectFormat(int w, int h, int desired_framerate);

	bool 				bStarted;
	bool 				bPlaying;
	bool 				bPaused;
	bool 				bIsFrameNew;			// if we are new
	bool				bIsMovieDone;
	int					loopMode;

	ofGstVideoData 		gstData;
	ofGstCamData		camData;

	bool				bIsStream;
	bool				bIsCamera;
	bool				bIsCustomWithSink;

	GstElement  *		gstSink;
	GstElement 	*		gstPipeline;


	bool				posChangingPaused;

	int 				width, height,bpp;
	bool 				bLoaded;
	//bool				allocated;				// so we know to free pixels or not
	bool				bRemote;
	pthread_mutex_t 	seek_mutex;


	// common with gstdata
	unsigned char 	*	pixels;				// 24 bit: rgb
	bool				bHavePixelsChanged;

	gint64				durationNanos;
	guint64				nFrames;
	int					pipelineState;
	float				speed;

	bool 				bFrameByFrame;

	int fps_n;
	int fps_d;

	//camera only
	int					deviceID;

};

#endif /* OFGSTUTILS_H_ */
