#include "stdafx.h"



#include "ofGstUtils.h"
//#include "ofUtils.h"


#define GST_DISABLE_DEPRECATED
#include <gst/gst.h>
#include <gst/gstpad.h>

#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include <glib-object.h>
#include <glib.h>
#include <algorithm>

#include <ILogManager.h>
#include <StringConverter.h>
#include <PixelUtil.h>
#include <IThreadManager.h>
#include "CMySrc.h"
#include "CMySink.h"

#define OF_LOOP_NONE 0
#define  OF_LOOP_NORMAL 1
#define OF_LOOP_PALINDROME 2



class ofGstUtilsImpl
{
public:
	ofGstUtilsImpl()
	{

		bLoaded = false;
		speed = 1;
		bPaused = false;
		bIsMovieDone = false;
		bPlaying = false;
		loopMode = OF_LOOP_NONE;
		bFrameByFrame = false;

		gstPipeline = NULL;
		gstSink = NULL;

		durationNanos = 0;

		isAppSink = false;
	}
	// callbacks to get called from gstreamer
#if GST_VERSION_MAJOR==0
	virtual GstFlowReturn preroll_cb(GstBuffer * buffer);
	virtual GstFlowReturn buffer_cb(GstBuffer * buffer);
#else
	virtual GstFlowReturn preroll_cb(GstSample * buffer);
	virtual GstFlowReturn buffer_cb(GstSample * buffer);
#endif
	virtual void 		  eos_cb();

	static void startGstMainLoop();
	static void stopGstMainLoop();
	static GMainLoop * getGstMainLoop();
	static bool			busFunction(GstBus * bus, GstMessage * message, ofGstUtilsImpl * app);
	bool				gstHandleMessage(GstBus * bus, GstMessage * message);

	void stop();
	void close();
	void play();

	void setPaused(bool _bPause);
	void 	setSpeed(float speed);

	bool 				bPlaying;
	bool 				bPaused;
	bool				bIsMovieDone;
	bool 				bLoaded;
	bool 				bFrameByFrame;
	int			loopMode;

	GstElement  *		gstSink;
	GstElement 	*		gstPipeline;

	float				speed;
	gint64				durationNanos;
	bool				isAppSink;

	ofGstAppSink * 		appsink;
	bool				isStream;

	class ofGstMainLoopThread : public OS::IThreadFunction{
	public:
		GMainLoop *main_loop;
		ofGstMainLoopThread()
			:main_loop(NULL)
		{

		}

		void setup(){
			//			startThread();
		}
		void execute(OS::IThread*caller, void*arg){
			main_loop = g_main_loop_new(NULL, FALSE);
			g_main_loop_run(main_loop);
			printf("GST Thread shutdown\n");
		}
	};

	static ofGstMainLoopThread * mainLoop;
	static OS::IThread* mainLoopThread;

	GstBus * bus;
};
ofGstUtilsImpl::ofGstMainLoopThread * ofGstUtilsImpl::mainLoop=0;
OS::IThread * ofGstUtilsImpl::mainLoopThread=0;

void ofGstUtilsImpl::startGstMainLoop(){
	if (!mainLoop){
		mainLoop = new ofGstMainLoopThread;
	}
	if (mainLoopThread)
		stopGstMainLoop();
	mainLoopThread= OS::IThreadManager::getInstance().createThread(mainLoop);
	mainLoopThread->start(0);
	//mainLoop->start();
	
}

void ofGstUtilsImpl::stopGstMainLoop(){
	
	if (!mainLoop)
		return;

	g_main_loop_quit(mainLoop->main_loop);
	bool running = g_main_loop_is_running(mainLoop->main_loop);
	g_main_loop_unref(mainLoop->main_loop);
	delete mainLoop;
	mainLoop = 0;
	if (!mainLoopThread)
		return;
	OS::IThreadManager::getInstance().killThread(mainLoopThread);
	delete mainLoopThread;
	mainLoopThread = 0;

}

GMainLoop * ofGstUtilsImpl::getGstMainLoop(){
	return mainLoop->main_loop;
}



//-------------------------------------------------
//----------------------------------------- gstUtils
//-------------------------------------------------

static bool plugin_registered = false;
static bool gst_inited = false;


// called when the appsink notifies us that there is a new buffer ready for
// processing

static GstFlowReturn on_new_buffer_from_source(GstAppSink * elt, void * data){
#if GST_VERSION_MAJOR==0
	GstBuffer *buffer = gst_app_sink_pull_buffer(GST_APP_SINK(elt));
#else
	GstSample *buffer = gst_app_sink_pull_sample(GST_APP_SINK(elt));
#endif
	return (GstFlowReturn)((ofGstUtils*)data)->buffer_cb(buffer);
}

static GstFlowReturn on_new_preroll_from_source(GstAppSink * elt, void * data){
#if GST_VERSION_MAJOR==0
	GstBuffer *buffer;
#else
	GstSample *buffer;
#endif
	buffer = gst_app_sink_pull_preroll(GST_APP_SINK(elt));
	return (GstFlowReturn)((ofGstUtils*)data)->preroll_cb(buffer);
}

static void on_eos_from_source(GstAppSink * elt, void * data){
	((ofGstUtils*)data)->eos_cb();
}

static gboolean appsink_plugin_init(GstPlugin * plugin)
{
	gst_element_register(plugin, "appsink", GST_RANK_NONE, GST_TYPE_APP_SINK);

	return TRUE;
}



ofGstUtils::ofGstUtils() {
	m_impl = new ofGstUtilsImpl();

	m_impl->isStream = false;

	m_impl->appsink = NULL;

#if GLIB_MINOR_VERSION<32
	if (!g_thread_supported()){
		g_thread_init(NULL);
	}
#endif

	if (!gst_inited){
		gst_init(NULL, NULL);
		gst_inited = true;
		gLogManager.log("ofGstUtils - gstreamer inited",ELL_INFO,EVL_Heavy);
	}
	if (!plugin_registered){
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"appsink", (char*)"Element application sink",
			appsink_plugin_init, "0.1", "LGPL", "ofVideoPlayer", "openFrameworks",
			"http://openframeworks.cc/");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysrc", (char*)"Element application src",
			_GstMySrcClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "TELUBee",
			"");
		gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
			"mysink", (char*)"Element application sink",
			_GstMySinkClass::plugin_init, "0.1", "LGPL", "GstVideoProvider", "TELUBee",
			"");
		plugin_registered = true;
	}

}

ofGstUtils::~ofGstUtils() {
	close();
	delete m_impl;
}

#if GST_VERSION_MAJOR==0
GstFlowReturn ofGstUtilsImpl::preroll_cb(GstBuffer * buffer){
#else
GstFlowReturn ofGstUtils::preroll_cb(GstSample * buffer){
#endif
	bIsMovieDone = false;
	if (appsink) return (GstFlowReturn)appsink->on_preroll(buffer);
	else return GST_FLOW_OK;
}


#if GST_VERSION_MAJOR==0
GstFlowReturn ofGstUtilsImpl::buffer_cb(GstBuffer * buffer){
#else
GstFlowReturn ofGstUtilsImpl::buffer_cb(GstSample * buffer){
#endif
	bIsMovieDone = false;
	if (appsink) return (GstFlowReturn)appsink->on_buffer(buffer);
	else return GST_FLOW_OK;
}

void ofGstUtilsImpl::eos_cb(){
	bIsMovieDone = true;
	if (appsink) appsink->on_eos();
}

bool ofGstUtils::setPipelineWithSink(const core::string& pipeline, const core::string& sinkname, bool isStream){
	ofGstUtilsImpl::startGstMainLoop();

	gchar* pipeline_string =
		g_strdup((pipeline).c_str());

	GError * error = NULL;
	m_impl->gstPipeline = gst_parse_launch(pipeline_string, &error);

	gLogManager.log("ofGstUtils - setPipelineWithSink(): gstreamer pipeline: " + core::string(pipeline_string),ELL_INFO);
	if (error != NULL){
		gLogManager.log("ofGstUtils - setPipelineWithSink(): couldn't create pipeline: " + core::string(error->message), ELL_WARNING);
		return false;
	}

	if (sinkname != ""){
		m_impl->gstSink = gst_bin_get_by_name(GST_BIN(m_impl->gstPipeline), sinkname.c_str());

		if (!m_impl->gstSink){
			gLogManager.log("ofGstUtils - setPipelineWithSink(): couldn't get sink from string pipeline",ELL_WARNING);
		}
	}

	return setPipelineWithSink(m_impl->gstPipeline, m_impl->gstSink, isStream);
}

bool ofGstUtils::setPipelineWithSink(GstElement * pipeline, GstElement * sink, bool isStream_){
	ofGstUtilsImpl::startGstMainLoop();

	m_impl->gstPipeline = pipeline;
	m_impl->gstSink = sink;
	m_impl->isStream = isStream_;

	if (m_impl->gstSink){
	//	gst_base_sink_set_sync(GST_BASE_SINK(m_impl->gstSink), true);
	}

	if (m_impl->gstSink && core::string(gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(gst_element_get_factory(m_impl->gstSink)))) == "appsink"){
		m_impl->isAppSink = true;
	}
	else{
		m_impl->isAppSink = false;
	}

	return true;
}

void ofGstUtils::setFrameByFrame(bool _bFrameByFrame){
	m_impl->bFrameByFrame = _bFrameByFrame;
	if (m_impl->gstSink){
		g_object_set(G_OBJECT(m_impl->gstSink), "sync", !m_impl->bFrameByFrame, (void*)NULL);
	}
}

bool ofGstUtils::isFrameByFrame(){
	return m_impl->bFrameByFrame;
}

bool ofGstUtils::startPipeline(){

	m_impl->bPaused = true;
	m_impl->speed = 1.0f;


	if (gst_element_set_state(GST_ELEMENT(m_impl->gstPipeline), GST_STATE_READY) == GST_STATE_CHANGE_FAILURE) {
		gLogManager.log("ofGstUtils - startPipeline(): unable to set pipeline to ready", ELL_WARNING);
		return false;
	}
	
	if (gst_element_get_state(GST_ELEMENT(m_impl->gstPipeline), NULL, NULL, 10 * GST_SECOND) == GST_STATE_CHANGE_FAILURE){
		gLogManager.log("ofGstUtils - startPipeline(): unable to get pipeline ready status", ELL_WARNING);
		return false;
	}/**/

	// pause the pipeline
	if (gst_element_set_state(GST_ELEMENT(m_impl->gstPipeline), GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
		gLogManager.log("ofGstUtils - startPipeline(): unable to pause pipeline", ELL_WARNING);
		return false;
	}

	// wait for paused state to query the duration
	if (!m_impl->isStream){
		GstState state = GST_STATE_PAUSED;
		if (gst_element_get_state(m_impl->gstPipeline, &state, NULL, 2 * GST_SECOND) == GST_STATE_CHANGE_FAILURE){
			gLogManager.log("ofGstUtils - startPipeline(): unable to get pipeline paused state", ELL_WARNING);
			return false;
		}
		m_impl->bPlaying = true;
		m_impl->bLoaded = true;
	}

	m_impl->bus = gst_pipeline_get_bus(GST_PIPELINE(m_impl->gstPipeline));

	if (m_impl->bus){
		gst_bus_add_watch(m_impl->bus, (GstBusFunc)m_impl->busFunction, m_impl);
	}



	if (m_impl->isAppSink){
		gLogManager.log("ofGstUtils - startPipeline(): attaching callbacks",ELL_INFO,EVL_Heavy);
		// set the appsink to not emit signals, we are using callbacks instead
		// and frameByFrame to get buffers by polling instead of callback
		g_object_set(G_OBJECT(m_impl->gstSink), "emit-signals", FALSE, "sync", !m_impl->bFrameByFrame, (void*)NULL);
		//gst_app_sink_set_drop(GST_APP_SINK(gstSink),1);
		//gst_app_sink_set_max_buffers(GST_APP_SINK(gstSink),2);

		if (!m_impl->bFrameByFrame){
			GstAppSinkCallbacks gstCallbacks;
			gstCallbacks.eos = &on_eos_from_source;
			gstCallbacks.new_preroll = &on_new_preroll_from_source;
#if GST_VERSION_MAJOR==0
			gstCallbacks.new_buffer = &on_new_buffer_from_source;
#else
			gstCallbacks.new_sample = &on_new_buffer_from_source;
#endif

			gst_app_sink_set_callbacks(GST_APP_SINK(m_impl->gstSink), &gstCallbacks, this, NULL);
		}

	}

	if (!m_impl->isStream){
		setSpeed(1.0);
	}


	return true;
}

void ofGstUtils::play(){
	m_impl->play();
}

void ofGstUtilsImpl::play(){
	setPaused(false);

	//this is if we set the speed first but it only can be set when we are playing.
	if (!isStream) setSpeed(speed);
}

void ofGstUtils::setPaused(bool _bPause){
	m_impl->setPaused(_bPause);
}

void ofGstUtilsImpl::setPaused(bool _bPause){
	bPaused = _bPause;
	//timeLastIdle = ofGetElapsedTimeMillis();
	if (bLoaded){
		if (bPlaying){
			if (bPaused){
				gst_element_set_state(gstPipeline, GST_STATE_PAUSED);
			}
			else{
				gst_element_set_state(gstPipeline, GST_STATE_PLAYING);
			}
		}
		else{
			GstState state = GST_STATE_PAUSED;
			gst_element_set_state(gstPipeline, state);
			gst_element_get_state(gstPipeline, &state, NULL, 2 * GST_SECOND);
			if (!bPaused){
				gst_element_set_state(gstPipeline, GST_STATE_PLAYING);
			}
			bPlaying = true;
		}
	}
}

bool 	ofGstUtils::isPaused(){ return m_impl->bPaused; }
bool 	ofGstUtils::isLoaded(){ return m_impl->bLoaded; }
bool 	ofGstUtils::isPlaying(){ return m_impl->bPlaying; }
void ofGstUtils::stop(){
	m_impl->stop();
}
void ofGstUtilsImpl::stop(){
	if (!bPlaying) return;
	GstState state = GST_STATE_PAUSED;
	if (!bPaused){
		gst_element_set_state(gstPipeline, state);
		gst_element_get_state(gstPipeline, &state, NULL, 2 * GST_SECOND);
	}
	state = GST_STATE_READY;
	gst_element_set_state(gstPipeline, state);
	gst_element_get_state(gstPipeline, &state, NULL, 2 * GST_SECOND);
	bPlaying = false;
	bPaused = true;

}

float ofGstUtils::getPosition(){
	if (m_impl->gstPipeline){
		gint64 pos = 0;
#if GST_VERSION_MAJOR==0
		GstFormat format = GST_FORMAT_TIME;
		if (!gst_element_query_position(GST_ELEMENT(m_impl->gstPipeline), &format, &pos)){
			gLogManager.log("ofGstUtils - getPosition(): couldn't query position",ELL_WARNING);
			return -1;
		}
#else
		if (!gst_element_query_position(GST_ELEMENT(gstPipeline), GST_FORMAT_TIME, &pos)){
			ofLogVerbose("ofGstUtils") << "getPosition(): couldn't query position";
			return -1;
		}
#endif
		return (float)pos / (float)m_impl->durationNanos;
	}
	else{
		return -1;
	}
}

float ofGstUtils::getSpeed(){
	return m_impl->speed;
}

float ofGstUtils::getDuration(){
	return (float)getDurationNanos() / (float)GST_SECOND;
}

ulong ofGstUtils::getDurationNanos(){
	GstFormat format = GST_FORMAT_TIME;

#if GST_VERSION_MAJOR==0
	if (!gst_element_query_duration(getPipeline(), &format, &m_impl->durationNanos))
		gLogManager.log("ofGstUtils - getDurationNanos(): couldn't query time duration",ELL_WARNING);
#else
	if (!gst_element_query_duration(getPipeline(), format, &durationNanos))
		ofLogWarning("ofGstUtils") << "getDurationNanos(): couldn't query time duration";
#endif
	return m_impl->durationNanos;

}

bool  ofGstUtils::getIsMovieDone(){
	if (m_impl->isAppSink){
		return gst_app_sink_is_eos(GST_APP_SINK(m_impl->gstSink));
	}
	else{
		return m_impl->bIsMovieDone;
	}
}

void ofGstUtils::setPosition(float pct){
	//pct = CLAMP(pct, 0,1);// check between 0 and 1;
	GstFormat format = GST_FORMAT_TIME;
	GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH);
	gint64 pos = (guint64)((double)pct*(double)m_impl->durationNanos);

	/*if(bPaused){
	seek_lock();
	gst_element_set_state (gstPipeline, GST_STATE_PLAYING);
	posChangingPaused=true;
	seek_unlock();
	}*/
	if (m_impl->speed>0){
		if (!gst_element_seek(GST_ELEMENT(m_impl->gstPipeline), m_impl->speed, format,
			flags,
			GST_SEEK_TYPE_SET,
			pos,
			GST_SEEK_TYPE_SET,
			-1)) {
			gLogManager.log("ofGstUtils - setPosition(): unable to seek",ELL_WARNING);
		}
	}
	else{
		if (!gst_element_seek(GST_ELEMENT(m_impl->gstPipeline), m_impl->speed, format,
			flags,
			GST_SEEK_TYPE_SET,
			0,
			GST_SEEK_TYPE_SET,
			pos)) {
			gLogManager.log("ofGstUtils - setPosition(): unable to seek",ELL_WARNING);
		}
	}
}

void ofGstUtils::setVolume(float volume){
	gdouble gvolume = volume;
	g_object_set(G_OBJECT(m_impl->gstPipeline), "volume", gvolume, (void*)NULL);
}

void ofGstUtils::setLoopState(int state){
	m_impl->loopMode = state;
}

int	ofGstUtils::getLoopState(){ return m_impl->loopMode; }
void ofGstUtils::setSpeed(float _speed){
	m_impl->setSpeed(_speed);
}
void ofGstUtilsImpl::setSpeed(float _speed){

	GstFormat format = GST_FORMAT_TIME;
	GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_SKIP | GST_SEEK_FLAG_ACCURATE | GST_SEEK_FLAG_FLUSH);
	gint64 pos;

	if (_speed == 0){
		gst_element_set_state(gstPipeline, GST_STATE_PAUSED);
		return;
	}
#if GST_VERSION_MAJOR==0
	if (!gst_element_query_position(GST_ELEMENT(gstPipeline), &format, &pos) || pos<0){
		//gLogManager.log("ofGstUtils") << "setSpeed(): couldn't query position";
		return;
	}
#else
	if (!gst_element_query_position(GST_ELEMENT(gstPipeline), format, &pos) || pos<0){
		//gLogManager.log("ofGstUtils") << "setSpeed(): couldn't query position";
		return;
	}
#endif

	speed = _speed;
	//pos = (float)gstData.lastFrame * (float)fps_d / (float)fps_n * GST_SECOND;

	if (!bPaused)
		gst_element_set_state(gstPipeline, GST_STATE_PLAYING);

	if (speed>0){
		if (!gst_element_seek(GST_ELEMENT(gstPipeline), speed, format,
			flags,
			GST_SEEK_TYPE_SET,
			pos,
			GST_SEEK_TYPE_SET,
			-1)) {
			gLogManager.log("ofGstUtils - setSpeed(): unable to change speed",ELL_WARNING);
		}
	}
	else{
		if (!gst_element_seek(GST_ELEMENT(gstPipeline), speed, format,
			flags,
			GST_SEEK_TYPE_SET,
			0,
			GST_SEEK_TYPE_SET,
			pos)) {
			gLogManager.log("ofGstUtils - setSpeed(): unable to change speed",ELL_WARNING);
		}
	}

	gLogManager.log("ofGstUtils - setSpeed(): speed changed to " + core::StringConverter::toString(speed), ELL_INFO, EVL_Heavy);

}

void ofGstUtilsImpl::close(){
	if (bPlaying){
		stop();
	}
	if (bLoaded){
		gst_element_set_state(GST_ELEMENT(gstPipeline), GST_STATE_NULL);
		gst_element_get_state(gstPipeline, NULL, NULL, 2 * GST_SECOND);
		// gst_object_unref(gstSink); this crashes, why??

		gst_object_unref(gstPipeline);
		gstPipeline = NULL;
		gstSink = NULL;
	}
	stopGstMainLoop();

	bLoaded = false;
}

static core::string getName(GstState state){
	switch (state){
	case   GST_STATE_VOID_PENDING:
		return "void pending";
	case   GST_STATE_NULL:
		return "null";
	case   GST_STATE_READY:
		return "ready";
	case   GST_STATE_PAUSED:
		return "paused";
	case   GST_STATE_PLAYING:
		return "playing";
	default:
		return "";
	}
}

bool ofGstUtilsImpl::busFunction(GstBus * bus, GstMessage * message, ofGstUtilsImpl * gstUtils){
	return gstUtils->gstHandleMessage(bus, message);
}

bool ofGstUtilsImpl::gstHandleMessage(GstBus * bus, GstMessage * msg){
	if (appsink && appsink->on_message(msg)) return true;

// 	gLogManager.log("ofGstUtils - gstHandleMessage(): got " << GST_MESSAGE_TYPE_NAME(msg)
// 		<< " message from " << GST_MESSAGE_SRC_NAME(msg));

	switch (GST_MESSAGE_TYPE(msg)) {

	case GST_MESSAGE_BUFFERING:
		gint pctBuffered;
		gst_message_parse_buffering(msg, &pctBuffered);
		//gLogManager.log("ofGstUtils - gstHandleMessage(): buffering " << pctBuffered;
		/*if(pctBuffered<100){
		gst_element_set_state (gstPipeline, GST_STATE_PAUSED);
		}else if(!bPaused){
		gst_element_set_state (gstPipeline, GST_STATE_PLAYING);
		}*/
		break;

#if GST_VERSION_MAJOR==0
	case GST_MESSAGE_DURATION:{
								  GstFormat format = GST_FORMAT_TIME;
								  gst_element_query_duration(gstPipeline, &format, &durationNanos);
	}break;
#else
	case GST_MESSAGE_DURATION_CHANGED:
		gst_element_query_duration(gstPipeline, GST_FORMAT_TIME, &durationNanos);
		break;

#endif

	case GST_MESSAGE_STATE_CHANGED:{
									   GstState oldstate, newstate, pendstate;
									   gst_message_parse_state_changed(msg, &oldstate, &newstate, &pendstate);
									   if (isStream && newstate == GST_STATE_PAUSED && !bPlaying){
										   bLoaded = true;
										   bPlaying = true;
										   if (!bPaused){
											//   ofLogVerbose("ofGstUtils") << "gstHandleMessage(): setting stream pipeline to play";
											   play();
										   }
									   }

									//   ofLogVerbose("ofGstUtils") << "gstHandleMessage(): " << GST_MESSAGE_SRC_NAME(msg) << " state changed from "
										//   << getName(oldstate) << " to " << getName(newstate) << " (" + getName(pendstate) << ")";
	}break;

	case GST_MESSAGE_ASYNC_DONE:
		//ofLogVerbose("ofGstUtils") << "gstHandleMessage(): async done";
		break;

	case GST_MESSAGE_WARNING: 
	{
								GError *err;
								gchar *debug;
								gst_message_parse_warning(msg, &err, &debug);
								gLogManager.log("ofGstUtils - HandleMessage(): Warning- " + core::string(debug), ELL_WARNING);
								// 								ofLogVerbose("ofGstUtils") << "gstHandleMessage(): embedded video playback halted for plugin, module "
								// 									<< gst_element_get_name(GST_MESSAGE_SRC(msg)) << "  reported: " << err->message;

								g_error_free(err);
								g_free(debug);

	}break;
	case GST_MESSAGE_ERROR: {
								GError *err;
								gchar *debug;
								gst_message_parse_error(msg, &err, &debug);

								gLogManager.log("ofGstUtils - HandleMessage(): Error- " + core::string(debug), ELL_WARNING);
// 								ofLogVerbose("ofGstUtils") << "gstHandleMessage(): embedded video playback halted for plugin, module "
// 									<< gst_element_get_name(GST_MESSAGE_SRC(msg)) << "  reported: " << err->message;

								g_error_free(err);
								g_free(debug);

								gst_element_set_state(GST_ELEMENT(gstPipeline), GST_STATE_NULL);

	}break;

	case GST_MESSAGE_EOS:
	//	ofLogVerbose("ofGstUtils") << "gstHandleMessage(): end of the stream";
		bIsMovieDone = true;


		if (appsink && !isAppSink) appsink->on_eos();

		switch (loopMode){

		case OF_LOOP_NORMAL:{
								GstFormat format = GST_FORMAT_TIME;
								GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT);
								gint64 pos;
#if GST_VERSION_MAJOR==0
								gst_element_query_position(GST_ELEMENT(gstPipeline), &format, &pos);
#else
								gst_element_query_position(GST_ELEMENT(gstPipeline), format, &pos);
#endif
								if (!gst_element_seek(GST_ELEMENT(gstPipeline),
									speed,
									format,
									flags,
									GST_SEEK_TYPE_SET,
									0,
									GST_SEEK_TYPE_SET,
									durationNanos)) {
									gLogManager.log("ofGstUtils - gstHandleMessage(): unable to seek",ELL_WARNING);
								}
		}break;

		case OF_LOOP_PALINDROME:{
									GstFormat format = GST_FORMAT_TIME;
									GstSeekFlags flags = (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT);
									gint64 pos;
#if GST_VERSION_MAJOR==0
									gst_element_query_position(GST_ELEMENT(gstPipeline), &format, &pos);
#else
									gst_element_query_position(GST_ELEMENT(gstPipeline), format, &pos);
#endif
									float loopSpeed;
									if (pos>0)
										loopSpeed = -speed;
									else
										loopSpeed = speed;
									if (!gst_element_seek(GST_ELEMENT(gstPipeline),
										loopSpeed,
										GST_FORMAT_UNDEFINED,
										flags,
										GST_SEEK_TYPE_NONE,
										0,
										GST_SEEK_TYPE_NONE,
										0)) {
										gLogManager.log("ofGstUtils - gstHandleMessage(): unable to seek",ELL_WARNING);
									}
		}break;

		default:
			break;
		}

		break;

	default:
		//ofLogVerbose("ofGstUtils") << "gstHandleMessage(): unhandled message from " << GST_MESSAGE_SRC_NAME(msg);
		break;
	}
	return true;
}

GstElement 	* ofGstUtils::getPipeline(){
	return m_impl->gstPipeline;
}

GstElement 	* ofGstUtils::getSink(){
	return m_impl->gstSink;
}

GstElement 	* ofGstUtils::getGstElementByName(const core::string & name){
	return gst_bin_get_by_name(GST_BIN(m_impl->gstPipeline), name.c_str());
}

void ofGstUtils::setSinkListener(ofGstAppSink * appsink_){
	m_impl->appsink = appsink_;
}



int ofGstUtils::preroll_cb(GstBuffer * buffer)
{
	return m_impl->preroll_cb(buffer);
}
int ofGstUtils::buffer_cb(GstBuffer * buffer)
{
	return m_impl->buffer_cb(buffer);
}
void ofGstUtils::eos_cb()
{
	return m_impl->eos_cb();
}

void ofGstUtils::startGstMainLoop()
{
	ofGstUtilsImpl::startGstMainLoop();
}

ulong ofGstUtils::getMinLatencyNanos(){
	GstClockTime minlat = 0, maxlat = 0;
	GstQuery * q = gst_query_new_latency();
	if (gst_element_query(m_impl->gstPipeline, q)) {
		gboolean live;
		gst_query_parse_latency(q, &live, &minlat, &maxlat);
	}
	gst_query_unref(q);
	return minlat;
}

ulong ofGstUtils::getMaxLatencyNanos(){
	GstClockTime minlat = 0, maxlat = 0;
	GstQuery * q = gst_query_new_latency();
	if (gst_element_query(m_impl->gstPipeline, q)) {
		gboolean live;
		gst_query_parse_latency(q, &live, &minlat, &maxlat);
	}
	gst_query_unref(q);
	return maxlat;
}

void ofGstUtils::close()
{
	m_impl->close();
}




//-------------------------------------------------
//----------------------------------------- videoUtils
//-------------------------------------------------



ofGstVideoUtils::ofGstVideoUtils(){
	bIsFrameNew = false;
	bHavePixelsChanged = false;
	bBackPixelsChanged = false;
	buffer = 0;
	prevBuffer = 0;
#if GST_VERSION_MAJOR==1
	GstMapInfo initMapinfo = { 0, };
	mapinfo = initMapinfo;
#endif
	mutex = OS::IThreadManager::getInstance().createMutex();

}

ofGstVideoUtils::~ofGstVideoUtils(){
	close();
}

void ofGstVideoUtils::close(){
	ofGstUtils::close();
	mutex->lock();

	pixels.clear();
	backPixels.clear();
	bIsFrameNew = false;
	bHavePixelsChanged = false;
	bBackPixelsChanged = false;
#if GST_VERSION_MAJOR==0
	if (prevBuffer) gst_buffer_unref(prevBuffer);
	if (buffer) gst_buffer_unref(buffer);
#else
	if (prevBuffer) gst_sample_unref(prevBuffer);
	if (buffer) gst_sample_unref(buffer);
#endif
	prevBuffer = 0;
	buffer = 0;
	mutex->unlock();

}

bool ofGstVideoUtils::isFrameNew(){
	return bIsFrameNew;
}

unsigned char * ofGstVideoUtils::getPixels(){
	return pixels.imageData;
}

video::ImageInfo* ofGstVideoUtils::getPixelsRef(){
	return &pixels;
}

void ofGstVideoUtils::update(){
	if (isLoaded()){
		if (!isFrameByFrame()){
			mutex->lock();
			bHavePixelsChanged = bBackPixelsChanged;
			if (bHavePixelsChanged){
				bBackPixelsChanged = false;
				math::Swap(pixels.imageData, backPixels.imageData);

#if GST_VERSION_MAJOR==0
				if (prevBuffer) gst_buffer_unref(prevBuffer);
#else
				if (prevBuffer) gst_sample_unref(prevBuffer);
#endif
				prevBuffer = buffer;
			}

			mutex->unlock();
		}
		else{
#if GST_VERSION_MAJOR==0
			GstBuffer *buffer;

			//get the buffer from appsink
			if (isPaused()) buffer = gst_app_sink_pull_preroll(GST_APP_SINK(getSink()));
			else buffer = gst_app_sink_pull_buffer(GST_APP_SINK(getSink()));

			if (buffer){
				if (pixels.imageDataSize!=0){
					if (prevBuffer) gst_buffer_unref(prevBuffer);
					pixels.setData(GST_BUFFER_DATA(buffer), pixels.Size, pixels.format);

					prevBuffer = buffer;
					bHavePixelsChanged = true;
				}
			}
		}
#else
			GstBuffer *buffer;
			GstSample * sample;

			//get the buffer from appsink
			if (isPaused()){
				sample = gst_app_sink_pull_preroll(GST_APP_SINK(getSink()));
			}
			else{
				sample = gst_app_sink_pull_sample(GST_APP_SINK(getSink()));
			}
			buffer = gst_sample_get_buffer(sample);

			if (buffer){
				if (pixels.isAllocated()){
					if (prevBuffer) gst_sample_unref(prevBuffer);
					gst_buffer_map(buffer, &mapinfo, GST_MAP_READ);
					//TODO: stride = mapinfo.size / height;
					pixels.setFromExternalPixels(mapinfo.data, pixels.getWidth(), pixels.getHeight(), pixels.getNumChannels());
					prevBuffer = sample;
					bHavePixelsChanged = true;
					gst_buffer_unmap(buffer, &mapinfo);
				}
			}
		}
#endif
	}
	else{
		gLogManager.log("ofGstVideoUtils - update(): ofGstVideoUtils not loaded",ELL_WARNING);
	}
	bIsFrameNew = bHavePixelsChanged;
	bHavePixelsChanged = false;
}

float ofGstVideoUtils::getHeight(){
	return pixels.Size.y;
}

float ofGstVideoUtils::getWidth(){
	return pixels.Size.x;
}

bool ofGstVideoUtils::setPipeline(const core::string& pipeline, int bpp, bool isStream, int w, int h){
	core::string caps;

#if GST_VERSION_MAJOR==0
	if (bpp == 8)
		caps = "video/x-raw-gray, depth=8, bpp=8";
	else if (bpp == 32)
		caps = "video/x-raw-rgb, depth=24, bpp=32, endianness=4321, red_mask=0xff0000, green_mask=0x00ff00, blue_mask=0x0000ff, alpha_mask=0x000000ff";
	else
		caps = "video/x-raw-rgb, depth=24, bpp=24, endianness=4321, red_mask=0xff0000, green_mask=0x00ff00, blue_mask=0x0000ff, alpha_mask=0x000000ff";
#else
	if (bpp == 8)
		caps = "video/x-raw, format=GRAY8";
	else if (bpp == 32)
		caps = "video/x-raw, format=RGBA";
	else
		caps = "video/x-raw, format=RGB";
#endif

	if (w != -1 && h != -1){
		caps += ", width=" + core::StringConverter::toString(w) + ", height=" + core::StringConverter::toString(h);
	}

	gchar* pipeline_string =
		g_strdup((pipeline + " ! appsink name=ofappsink caps=\"" + caps + "\"").c_str()); // caps=video/x-raw-rgb

	if ((isStream && (w == -1 || h == -1)) || allocate(w, h, bpp)){
		return setPipelineWithSink(pipeline_string, "ofappsink", isStream);
	}
	else{
		return false;
	}
}

bool ofGstVideoUtils::allocate(int w, int h, int _bpp){
	
	pixels.createData(math::vector3di(w, h, 1), video::EPixel_R8G8B8);
	backPixels.createData(math::vector3di(w, h, 1), video::EPixel_R8G8B8);
	prevBuffer = 0;
	bHavePixelsChanged = 1;

	return true;
}

#if GST_VERSION_MAJOR==0
int ofGstVideoUtils::preroll_cb(GstBuffer * _buffer){
	guint size = GST_BUFFER_SIZE(_buffer);
	if (pixels.imageData && pixels.Size.x*pixels.Size.y*3/*pixels.getBytesPerPixel()*/ > (int)size){
		gLogManager.log("ofGstVideoUtils - preproll_cb(): error preroll buffer size: " +core::StringConverter::toString(size)
			+ "!= init size: " + core::StringConverter::toString(pixels.Size.x*pixels.Size.y*3),ELL_WARNING);
		gst_buffer_unref(_buffer);
		return GST_FLOW_ERROR;
	}
	mutex->lock();
	if (bBackPixelsChanged && buffer) 
		gst_buffer_unref(buffer);
	if (!pixels.imageData)
	{
		if (m_impl->isStream && m_impl->appsink){
			m_impl->appsink->on_stream_prepared();
		}
		else{
			gLogManager.log("ofGstVideoUtils = preroll_cb(): received a preroll without allocation", ELL_WARNING);
		}
	}
	if (pixels.imageData){
		buffer = _buffer;
		backPixels.setData(GST_BUFFER_DATA(buffer), pixels.Size, pixels.format);
		eventPixels.setData(GST_BUFFER_DATA(buffer), pixels.Size, pixels.format);
		bBackPixelsChanged = true;
		//ofNotifyEvent(prerollEvent, eventPixels);
	}
	mutex->unlock();
	return m_impl->preroll_cb(_buffer);
}
#else
GstFlowReturn ofGstVideoUtils::preroll_cb(GstSample * sample){
	GstBuffer * _buffer = gst_sample_get_buffer(sample);
	gst_buffer_map(_buffer, &mapinfo, GST_MAP_READ);
	guint size = mapinfo.size;
	if (pixels.isAllocated() && pixels.getWidth()*pixels.getHeight()*pixels.getBytesPerPixel() != (int)size){
		gLogManager.log("ofGstVideoUtils") << "preproll_cb(): error preroll buffer size: " << size
			<< "!= init size: " << pixels.getWidth()*pixels.getHeight()*pixels.getBytesPerPixel();
		gst_sample_unref(sample);
		return GST_FLOW_ERROR;
	}
	mutex.lock();
	if (bBackPixelsChanged && buffer) gst_sample_unref(buffer);
	buffer = sample;
	if (pixels.isAllocated()){
		backPixels.setFromExternalPixels(mapinfo.data, pixels.getWidth(), pixels.getHeight(), pixels.getNumChannels());
		eventPixels.setFromExternalPixels(mapinfo.data, pixels.getWidth(), pixels.getHeight(), pixels.getNumChannels());
		bBackPixelsChanged = true;
		mutex.unlock();
		ofNotifyEvent(prerollEvent, eventPixels);
	}
	else{
		if (isStream && appsink){
			appsink->on_stream_prepared();
		}
		else{
			ofLogWarning("ofGstVideoUtils") << "preroll_cb(): received a preroll without allocation";
		}
		mutex.unlock();
	}
	gst_buffer_unmap(_buffer, &mapinfo);
	return ofGstUtils::preroll_cb(sample);
}
#endif

#if GST_VERSION_MAJOR==0
int ofGstVideoUtils::buffer_cb(GstBuffer * _buffer){

	guint size;

	size = GST_BUFFER_SIZE(_buffer);
	if (pixels.imageData && pixels.Size.x*pixels.Size.y * 3 > (int)size){
	//	gLogManager.log("ofGstVideoUtils") << "buffer_cb(): error on new buffer, buffer size: " << size
		//	<< " != init size: " << pixels.getWidth()*pixels.getHeight()*pixels.getBytesPerPixel();
		gst_buffer_unref(_buffer);
		return GST_FLOW_ERROR;
	}
	mutex->lock();
	if (bBackPixelsChanged && buffer)
		gst_buffer_unref(buffer);
	if (pixels.imageDataSize){
		buffer = _buffer;
		backPixels.setData(GST_BUFFER_DATA(buffer), pixels.Size, pixels.format);
		eventPixels.setData(GST_BUFFER_DATA(buffer), pixels.Size, pixels.format);
		bBackPixelsChanged = true;
		mutex->unlock();
	//	ofNotifyEvent(bufferEvent, eventPixels);
	}
	else{
		if (m_impl->isStream && m_impl->appsink){
			m_impl->appsink->on_stream_prepared();
		}
		else{
			gLogManager.log("received a preroll without allocation", ELL_WARNING);
		}
		mutex->unlock();
	}

	return m_impl->buffer_cb(buffer);
}
#else
GstFlowReturn ofGstVideoUtils::buffer_cb(GstSample * sample){
	GstBuffer * _buffer = gst_sample_get_buffer(sample);
	gst_buffer_map(_buffer, &mapinfo, GST_MAP_READ);
	guint size = mapinfo.size;
	if (pixels.isAllocated() && pixels.getWidth()*pixels.getHeight()*pixels.getBytesPerPixel() != (int)size){
		gLogManager.log("ofGstVideoUtils") << "buffer_cb(): error on new buffer, buffer size: " << size
			<< "!= init size: " << pixels.getWidth()*pixels.getHeight()*pixels.getBytesPerPixel();
		gst_sample_unref(sample);
		return GST_FLOW_ERROR;
	}
	mutex.lock();
	if (bBackPixelsChanged && buffer) gst_sample_unref(buffer);
	buffer = sample;
	if (pixels.isAllocated()){
		backPixels.setFromExternalPixels(mapinfo.data, pixels.getWidth(), pixels.getHeight(), pixels.getNumChannels());
		eventPixels.setFromExternalPixels(mapinfo.data, pixels.getWidth(), pixels.getHeight(), pixels.getNumChannels());
		bBackPixelsChanged = true;
		mutex.unlock();
		ofNotifyEvent(bufferEvent, eventPixels);
	}
	else{
		if (isStream && appsink){
			appsink->on_stream_prepared();
		}
		else{
			ofLogWarning("ofGstVideoUtils") << "buffer_cb(): received a preroll without allocation";
		}
		mutex.unlock();
	}
	gst_buffer_unmap(_buffer, &mapinfo);
	return ofGstUtils::buffer_cb(buffer);
}
#endif

void ofGstVideoUtils::eos_cb(){
	m_impl->eos_cb();
// 	ofEventArgs args;
// 	ofNotifyEvent(eosEvent, args);
}


int ofGstAppSink::on_preroll(GstBuffer * buffer){
	return GST_FLOW_OK;
}
int ofGstAppSink::on_buffer(GstBuffer * buffer){
	return GST_FLOW_OK;
}