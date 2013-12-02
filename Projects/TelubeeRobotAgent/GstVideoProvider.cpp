

#include "stdafx.h"
#include "GstVideoProvider.h"

#define GST_DISABLE_DEPRECATED
#include <gst/gst.h>
#include <gst/gstpad.h>

#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
#include <gst/app/gstappsrc.h>

#include <glib-object.h>
#include <glib.h>
#include <algorithm>

#include <ILogManager.h>
#include <StringConverter.h>
#include <PixelUtil.h>
#include <IThreadManager.h>
#include "IUDPClient.h"
#include "INetwork.h"

namespace mray
{

	static bool plugin_registered = false;
	static bool gst_inited = false;
class GstVideoProviderImpl
{
protected:
public:

	GstElement*			gstSrc;
	GstElement  *		gstSink;
	GstElement 	*		gstPipeline;
	GstBus*		bus;
	GTimer*		timer;
	bool				bPlaying;
	bool				bLoaded;
	bool				bPaused;
	bool isAppSink;

	int sourceid;

	video::IVideoGrabber* src;

	network::IUDPClient* m_udpClient;
	network::NetAddress m_targetAddress;

	ulong m_counter;
public:

	static gboolean push_data(GstVideoProviderImpl *data) {
		data->NeedData();
#if 0

		GstBuffer *buffer;
		GstFlowReturn ret;
		int i;
		guchar *raw;
		gint num_samples = PIXElS_COUNT * 3 * sizeof(guchar);
		gfloat freq;

		gdouble ms = g_timer_elapsed(data->timer, NULL);
		if (ms > 1.0 / 30.0) {
			/* Create a new empty buffer */
			//buffer = gst_buffer_new_and_alloc(num_samples);

			raw = (guchar*)malloc(num_samples);
			//raw = (guint8 *)GST_BUFFER_DATA(buffer)
			/* Set its timestamp and duration */

			memset(image_pixels, 0, PIXElS_COUNT * 3);
			for (int i = 50; i < 100; ++i)
			{
				for (int j = 100; j < 150; ++j)
				{
					int index = (((i + data->offset) % HEIGHT)*WIDTH + j) * 3;
					image_pixels[index] = 255;
				}
			}
			data->offset += 10;


			for (i = 0; i < num_samples; i++) {
				raw[i] = (guchar)(image_pixels[i]);
			}
			data->dataptr += num_samples;
			data->dataptr = data->dataptr%PIXElS_COUNT;
			data->num_samples += num_samples;

			buffer = gst_buffer_new();
			GST_BUFFER_MALLOCDATA(buffer) = raw;
			GST_BUFFER_SIZE(buffer) = num_samples;
			GST_BUFFER_DATA(buffer) = GST_BUFFER_MALLOCDATA(buffer);
			GST_BUFFER_TIMESTAMP(buffer) = gst_util_uint64_scale(data->num_samples, GST_SECOND, 1);
			GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(ChunkSize, GST_SECOND, 1);

			//		g_print("Data Count=%d\n", data->num_samples);

			GstCaps* caps = gst_caps_new_simple("video/x-raw-rgb",
				"rate", G_TYPE_INT, 90000,
				"bpp", G_TYPE_INT, 24,
				"depth", G_TYPE_INT, 24,
				"width", G_TYPE_INT, WIDTH,
				"height", G_TYPE_INT, HEIGHT,
				NULL);
			//	GST_BUFFER_CAPS(buffer) = caps;
			/* Push the buffer into the appsrc */
			//ret=gst_app_src_push_buffer((GstAppSrc*)data->src, buffer);
			g_signal_emit_by_name(data->src, "push-buffer", buffer, &ret);

			/* Free the buffer now that we are done with it */
			//	gst_buffer_unref(buffer);
			//free(raw);
			if (ret != GST_FLOW_OK) {
				/* We got some error, stop sending data */
				return FALSE;
			}
			g_timer_start(data->timer);
			return TRUE;

		}
#endif

		return true;
	}


	/* This signal callback triggers when appsrc needs data. Here, we add an idle handler
	* to the mainloop to start pushing data into the appsrc */
	static void start_feed(GstElement *source, guint size, GstVideoProviderImpl *data) {
		if (data->sourceid == 0) {
			data->sourceid = g_idle_add((GSourceFunc)push_data, data);
		}
	}

	/* This callback triggers when appsrc has enough data and we can stop sending.
	* We remove the idle handler from the mainloop */
	static void stop_feed(GstElement *source, GstVideoProviderImpl *data) {
		if (data->sourceid != 0) {
			g_source_remove(data->sourceid);
			data->sourceid = 0;
		}
	}

	/* The appsink has received a buffer */
	static GstFlowReturn new_buffer(GstAppSink * sink, void * data) {
		GstBuffer *buffer;

		/* Retrieve the buffer */
		g_signal_emit_by_name(sink, "pull-buffer", &buffer);
		if (buffer) {
			/* The only thing we do in this example is print a * to indicate a received buffer */
			//	Sleep(10);
			((GstVideoProviderImpl*)data)->SendData(buffer);
			gst_buffer_unref(buffer);
		}
		return GST_FLOW_OK;
	}
	/* The appsink has received a buffer */
	static GstFlowReturn new_preroll(GstAppSink * sink, void * data) {
		GstBuffer *buffer;

		/* Retrieve the buffer */
		buffer = gst_app_sink_pull_preroll(GST_APP_SINK(sink));
		if (buffer) {
			/* The only thing we do in this example is print a * to indicate a received buffer */

			guint8* data = GST_BUFFER_DATA(buffer);
			int size = GST_BUFFER_SIZE(buffer);
			gst_buffer_unref(buffer);
		}
		return GST_FLOW_OK;
	}
public:

	GstVideoProviderImpl()
	{


		if (!gst_inited){
			gst_init(NULL, NULL);
			gst_inited = true;
			gLogManager.log("ofGstUtils - gstreamer inited", ELL_INFO, EVL_Heavy);
		}
		/*
		if (!plugin_registered){
			gst_plugin_register_static(GST_VERSION_MAJOR, GST_VERSION_MINOR,
				"appsink", (char*)"Element application sink",
				appsink_plugin_init, "0.1", "LGPL", "ofVideoPlayer", "openFrameworks",
				"http://openframeworks.cc/");
			plugin_registered = true;
		}*/

		bPlaying = false;
		bLoaded = false;
		bPaused = false;
		sourceid = 0;
		timer = g_timer_new();

		m_udpClient = network::INetwork::getInstance().createUDPClient();


	}
	virtual~GstVideoProviderImpl()
	{
		Stop();
	}

	bool NeedData()
	{
		gdouble ms = g_timer_elapsed(timer, NULL);
		if (ms < 1.0 / 20.0)
			return true;
		if (!src->GrabFrame())return true;
		const video::ImageInfo* image= src->GetLastFrame();

		GstBuffer* buffer = gst_buffer_new();
		GST_BUFFER_SIZE(buffer) = image->imageDataSize;
		GST_BUFFER_DATA(buffer) = image->imageData;
		GST_BUFFER_TIMESTAMP(buffer) = gst_util_uint64_scale(m_counter, GST_SECOND, 1);
		++m_counter;
//		GST_BUFFER_DURATION(buffer) = gst_util_uint64_scale(image->imageDataSize, GST_SECOND, 1);
		
		GstFlowReturn ret;
		g_signal_emit_by_name(gstSrc, "push-buffer", buffer, &ret);

		/* Free the buffer now that we are done with it */
		if (ret != GST_FLOW_OK) {
			/* We got some error, stop sending data */
			return FALSE;
		}
		g_timer_start(timer);
		return true;
	}

	void SendData(GstBuffer* buffer)
	{
		guint8* data = GST_BUFFER_DATA(buffer);
		int size = GST_BUFFER_SIZE(buffer);
		m_udpClient->SendTo(&m_targetAddress, (char*)data, size);
		printf("data sent: %d\n", size);
	}

	void SetDataSource(video::IVideoGrabber* src)
	{
		this->src = src;
	}
	void StreamDataTo(const network::NetAddress& target)
	{
		m_targetAddress = target;
	}

	bool Start()
	{
		close();
		startGstMainLoop();


		gstPipeline = gst_parse_launch("appsrc name=src ! ffmpegcolorspace name = mpeg !x264enc name = enc  !rtph264pay name = pay !appsink name = sink", NULL);
		if (!gstPipeline)
		{
			gLogManager.log("pipleine couldn't be created!",ELL_ERROR);
			return false;
		}
		gstSrc = gst_bin_get_by_name(GST_BIN(gstPipeline), "src");
		GstElement* mpg = gst_bin_get_by_name(GST_BIN(gstPipeline), "mpeg");
		GstElement* enc = gst_bin_get_by_name(GST_BIN(gstPipeline), "enc");
		GstElement* pay = gst_bin_get_by_name(GST_BIN(gstPipeline), "pay");
		GstElement* sink = gst_bin_get_by_name(GST_BIN(gstPipeline), "sink");
		gstSink = sink;
		g_signal_connect(gstPipeline, "deep-notify", G_CALLBACK(gst_object_default_deep_notify), NULL);

		g_signal_connect(gstSrc, "need-data", G_CALLBACK(start_feed), this);
		g_signal_connect(gstSrc, "enough-data", G_CALLBACK(stop_feed), this);

		/* Configure appsink */

		g_object_set(sink, "emit-signals", TRUE, NULL);
 //		g_signal_connect(sink, "new-buffer", G_CALLBACK(new_buffer), this);
// 		g_signal_connect(sink, "new-preroll", G_CALLBACK(new_preroll), this);
		g_object_set(G_OBJECT(sink), "async", FALSE, "sync", FALSE, NULL);
	
		g_object_set(G_OBJECT(gstSink), "emit-signals", FALSE, "sync", FALSE, (void*)NULL);
		//gst_app_sink_set_drop(GST_APP_SINK(gstSink),1);
		//gst_app_sink_set_max_buffers(GST_APP_SINK(gstSink),2);

		GstAppSinkCallbacks gstCallbacks;
		gstCallbacks.eos = 0;
		gstCallbacks.new_preroll = &new_preroll;
		gstCallbacks.new_buffer = &new_buffer;

		gst_app_sink_set_callbacks(GST_APP_SINK(gstSink), &gstCallbacks, this, NULL);

		GstCaps* caps;
		/*
		caps = gst_caps_new_simple("video/x-raw-yuv",
			"rate", G_TYPE_INT, 180000,
			//"bpp", G_TYPE_INT, 24,
			//"depth", G_TYPE_INT, 24,
			"width", G_TYPE_INT, src->GetFrameSize().x,
			"height", G_TYPE_INT, src->GetFrameSize().y,
			NULL);
		*/
		caps = gst_video_format_new_caps(GST_VIDEO_FORMAT_BGR, src->GetFrameSize().x, src->GetFrameSize().y, 10, 1, 4, 3);
		gst_app_src_set_caps(GST_APP_SRC(gstSrc), caps);
		
		m_counter = 0;
		m_udpClient->Close();
		m_udpClient->Open();
	//	setPipelineWithSink(gstPipeline, sink, true);
		startPipeline();
		play();
		return true;
	}
	void Stop()
	{
		close();
		delete m_udpClient;

	}


	bool setPipelineWithSink(GstElement * pipeline, GstElement * sink, bool isStream_){
		startGstMainLoop();

		gstPipeline = pipeline;
		gstSink = sink;

		if (gstSink){
			gst_base_sink_set_sync(GST_BASE_SINK(gstSink), true);
		}

		if (gstSink && core::string(gst_plugin_feature_get_name(GST_PLUGIN_FEATURE(gst_element_get_factory(gstSink)))) == "appsink"){
			isAppSink = true;
		}
		else{
			isAppSink = false;
		}

		return true;
	}

	bool startPipeline(){

		bPaused = true;

		if (gst_element_set_state(GST_ELEMENT(gstPipeline), GST_STATE_READY) == GST_STATE_CHANGE_FAILURE) {
			gLogManager.log("ofGstUtils - startPipeline(): unable to set pipeline to ready", ELL_WARNING);
			return false;
		}
		if (gst_element_get_state(GST_ELEMENT(gstPipeline), NULL, NULL, 10 * GST_SECOND) == GST_STATE_CHANGE_FAILURE){
			gLogManager.log("ofGstUtils - startPipeline(): unable to get pipeline ready status", ELL_WARNING);
			return false;
		}

		// pause the pipeline
		if (gst_element_set_state(GST_ELEMENT(gstPipeline), GST_STATE_PAUSED) == GST_STATE_CHANGE_FAILURE) {
			gLogManager.log("ofGstUtils - startPipeline(): unable to pause pipeline", ELL_WARNING);
			return false;
		}

	
		bus = gst_pipeline_get_bus(GST_PIPELINE(gstPipeline));

		if (bus){
			gst_bus_add_watch(bus, (GstBusFunc)busFunction, this);
		}



		return true;
	}

	void close(){
		if (bPlaying){
			stopPlay();
		}
		if (bLoaded){
			gst_element_set_state(GST_ELEMENT(gstPipeline), GST_STATE_NULL);
			gst_element_get_state(gstPipeline, NULL, NULL, 2 * GST_SECOND);
			// gst_object_unref(gstSink); this crashes, why??

			gst_object_unref(gstPipeline);
			gstPipeline = NULL;
			gstSink = NULL;
		}

		bLoaded = false;

	}
	void play()
	{
		setPaused(false);
	}
	void setPaused(bool _bPause)
	{
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
	void stopPlay()
	{
		if (!bPlaying)
			return;
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
	static void startGstMainLoop(){
		static bool initialized = false;
		if (!initialized){
			mainLoop = new GstMainLoopThread;

			mainLoopThread = OS::IThreadManager::getInstance().createThread(mainLoop);
			mainLoopThread->start(0);
			//mainLoop->start();
			initialized = true;
		}
	}

	static GMainLoop * getGstMainLoop(){
		return mainLoop->main_loop;
	}

	bool HandleMessage(GstBus * bus, GstMessage * msg);

	static bool			busFunction(GstBus * bus, GstMessage * message, GstVideoProviderImpl * app)
	{
		return app->HandleMessage(bus, message);
	}
	static gboolean appsink_plugin_init(GstPlugin * plugin)
	{
		gst_element_register(plugin, "appsink", GST_RANK_NONE, GST_TYPE_APP_SINK);

		return TRUE;
	}


	class GstMainLoopThread : public OS::IThreadFunction{
	public:
		GMainLoop *main_loop;
		GstMainLoopThread()
			:main_loop(NULL)
		{

		}

		void setup(){
		}
		void execute(OS::IThread*caller, void*arg){
			main_loop = g_main_loop_new(NULL, FALSE);
			g_main_loop_run(main_loop);
		}
	};
	static GstMainLoopThread * mainLoop;
	static OS::IThread* mainLoopThread;
};

GstVideoProviderImpl::GstMainLoopThread * GstVideoProviderImpl::mainLoop = 0;
OS::IThread * GstVideoProviderImpl::mainLoopThread = 0;


bool GstVideoProviderImpl::HandleMessage(GstBus * bus, GstMessage * msg){

	// 	gLogManager.log("ofGstUtils - gstHandleMessage(): got " << GST_MESSAGE_TYPE_NAME(msg)
	// 		<< " message from " << GST_MESSAGE_SRC_NAME(msg));

	switch (GST_MESSAGE_TYPE(msg)) {

	case GST_MESSAGE_BUFFERING:
		gint pctBuffered;
		gst_message_parse_buffering(msg, &pctBuffered);
		break;

#if GST_VERSION_MAJOR==0
	case GST_MESSAGE_DURATION:{
								  GstFormat format = GST_FORMAT_TIME;
	}break;
#else
	case GST_MESSAGE_DURATION_CHANGED:
		gst_element_query_duration(gstPipeline, GST_FORMAT_TIME, &durationNanos);
		break;

#endif

	case GST_MESSAGE_STATE_CHANGED:{
									   GstState oldstate, newstate, pendstate;
									   gst_message_parse_state_changed(msg, &oldstate, &newstate, &pendstate);
									   if (newstate == GST_STATE_PAUSED && !bPlaying){
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
		
		break;

	default:
		//ofLogVerbose("ofGstUtils") << "gstHandleMessage(): unhandled message from " << GST_MESSAGE_SRC_NAME(msg);
		break;
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////


GstVideoProvider::GstVideoProvider()
{
	m_impl = new GstVideoProviderImpl;
}

GstVideoProvider::~GstVideoProvider()
{
	delete m_impl;
}


void GstVideoProvider::SetDataSource(video::IVideoGrabber* src)
{
	m_impl->SetDataSource(src);
}

void GstVideoProvider::StreamDataTo(const network::NetAddress& target)
{
	m_impl->StreamDataTo(target);
}


void GstVideoProvider::Start()
{
	m_impl->Start();
}

void GstVideoProvider::Stop()
{
	m_impl->Stop();
}

}
