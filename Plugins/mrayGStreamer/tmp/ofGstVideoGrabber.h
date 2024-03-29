#pragma once

#include "ofGstUtils.h"

struct ofGstFramerate{
  int numerator;
  int denominator;
};

struct ofGstVideoFormat{
  core::string mimetype;
  core::string format_name;
  int    width;
  int    height;
  std::vector<ofGstFramerate> framerates;
  ofGstFramerate choosen_framerate;
};

struct ofGstDevice{
  core::string video_device;
  core::string gstreamer_src;
  core::string product_name;
  std::vector<ofGstVideoFormat> video_formats;
  int current_format;
};

struct ofGstCamData{
  std::vector<ofGstDevice> webcam_devices;
  bool bInited;
};

class ofGstVideoGrabber//: public ofBaseVideoGrabber
{
public:
	ofGstVideoGrabber();
	~ofGstVideoGrabber();

	/// needs to be called before initGrabber
	bool setPixelFormat(video::EPixelFormat pixelFormat);
	video::EPixelFormat	getPixelFormat();
	
	void videoSettings(){};//TODO: what is this??

	void listDevices();
	void setDeviceID(int id);
	void setDesiredFrameRate(int framerate);
	bool initGrabber(int w, int h);

	void 			update();
	bool 			isFrameNew();

	unsigned char * getPixels();
	video::ImageInfo*	getPixelsRef();

	float 			getHeight();
	float 			getWidth();
	void 			close();

	void			setVerbose(bool bVerbose);

	ofGstVideoUtils *	getGstVideoUtils();
private:
	ofGstVideoFormat&	selectFormat(int w, int h, int desired_framerate);

	ofGstCamData		camData;
	bool				bIsCamera;
	int					attemptFramerate;
	int					deviceID;
	video::EPixelFormat		internalPixelFormat;
	ofGstVideoUtils		videoUtils;
};
