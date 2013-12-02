/*
* ofGstVideoUtils.cpp
*
*  Created on: 16/01/2011
*      Author: arturo
*/

#include "stdafx.h"
#include "ofGstVideoPlayer.h"
#include <gst/video/video.h>
#include <gst/app/gstappsink.h>
#include "ILogManager.h"
#include "INetwork.h"
#include <gst/app/gstappsrc.h>

ofGstVideoPlayer::ofGstVideoPlayer(){
	nFrames = 0;
	internalPixelFormat = video::EPixel_R8G8B8;
	bIsStream = false;
	bIsAllocated = false;
	threadAppSink = false;
	videoUtils.setSinkListener(this);
}

ofGstVideoPlayer::~ofGstVideoPlayer(){
	close();
}

bool ofGstVideoPlayer::setPixelFormat(video::EPixelFormat pixelFormat){
	internalPixelFormat = pixelFormat;
	return true;
}

video::EPixelFormat ofGstVideoPlayer::getPixelFormat(){
	return internalPixelFormat;
}

bool ofGstVideoPlayer::loadMovie(core::string name){
	close();
	if (name.find("file://", 0) != core::string::npos){
		bIsStream = false;
	}
	else if (name.find("://", 0) == core::string::npos){
		//name = "file://" + ofToDataPath(name, true);
		bIsStream = false;
	}
	else{
		bIsStream = true;
	}
	core::string message = "ofGstVideoPlayer - loadMovie(): loading \"" +name + "\"";
	gLogManager.log(message, ELL_INFO,EVL_Heavy);

	ofGstUtils::startGstMainLoop();

#if GST_VERSION_MAJOR==0
	GstElement * gstPipeline = gst_element_factory_make("playbin2", "player");
#else
	GstElement * gstPipeline = gst_element_factory_make("playbin", "player");
#endif
	g_object_set(G_OBJECT(gstPipeline), "uri", name.c_str(), (void*)NULL);

	// create the oF appsink for video rgb without sync to clock
	GstElement * gstSink = gst_element_factory_make("appsink", "app_sink");

	gst_base_sink_set_sync(GST_BASE_SINK(gstSink), true);
	gst_app_sink_set_max_buffers(GST_APP_SINK(gstSink), 8);
	gst_app_sink_set_drop(GST_APP_SINK(gstSink), true);
	gst_base_sink_set_max_lateness(GST_BASE_SINK(gstSink), -1);

#if GST_VERSION_MAJOR==0
	int bpp;
	core::string mime;
	switch (internalPixelFormat){
	case video::EPixel_LUMINANCE8:
		mime = "video/x-raw-gray";
		bpp = 8;
		break;
	case video::EPixel_R8G8B8:
		mime = "video/x-raw-rgb";
		bpp = 24;
		break;
	case video::EPixel_R8G8B8A8:
	case video::EPixel_B8G8R8A8:
		mime = "video/x-raw-rgb";
		bpp = 32;
		break;
	default:
		mime = "video/x-raw-rgb";
		bpp = 24;
		break;
	}

	GstCaps *caps = gst_caps_new_simple(mime.c_str(),
		"bpp", G_TYPE_INT, bpp,
		"depth", G_TYPE_INT, 24,
		"endianness", G_TYPE_INT, 4321,
		"red_mask", G_TYPE_INT, 0xff0000,
		"green_mask", G_TYPE_INT, 0x00ff00,
		"blue_mask", G_TYPE_INT, 0x0000ff,
		"alpha_mask", G_TYPE_INT, 0x000000ff,
		NULL);
#else
	int bpp;
	string mime = "video/x-raw";
	string format;
	switch (internalPixelFormat){
	case OF_PIXELS_MONO:
		format = "GRAY8";
		bpp = 8;
		break;
	case OF_PIXELS_RGB:
		format = "RGB";
		bpp = 24;
		break;
	case OF_PIXELS_RGBA:
		format = "RGBA";
		bpp = 32;
		break;
	case OF_PIXELS_BGRA:
		format = "BGRA";
		bpp = 32;
		break;
	default:
		format = "RGB";
		bpp = 24;
		break;
	}

	GstCaps *caps = gst_caps_new_simple(mime.c_str(),
		"format", G_TYPE_STRING, format.c_str(),
		/*"bpp", G_TYPE_INT, bpp,
		"depth", G_TYPE_INT, 24,
		"endianness",G_TYPE_INT,4321,
		"red_mask",G_TYPE_INT,0xff0000,
		"green_mask",G_TYPE_INT,0x00ff00,
		"blue_mask",G_TYPE_INT,0x0000ff,
		"alpha_mask",G_TYPE_INT,0x000000ff,*/
		NULL);
#endif


	gst_app_sink_set_caps(GST_APP_SINK(gstSink), caps);
	gst_caps_unref(caps);

	if (threadAppSink){
		GstElement * appQueue = gst_element_factory_make("queue", "appsink_queue");
		g_object_set(G_OBJECT(appQueue), "leaky", 0, "silent", 1, (void*)NULL);
		GstElement* appBin = gst_bin_new("app_bin");
		gst_bin_add(GST_BIN(appBin), appQueue);
		GstPad* appQueuePad = gst_element_get_static_pad(appQueue, "sink");
		GstPad* ghostPad = gst_ghost_pad_new("app_bin_sink", appQueuePad);
		gst_object_unref(appQueuePad);
		gst_element_add_pad(appBin, ghostPad);

		gst_bin_add_many(GST_BIN(appBin), gstSink, NULL);
		gst_element_link_many(appQueue, gstSink, NULL);

		g_object_set(G_OBJECT(gstPipeline), "video-sink", appBin, (void*)NULL);
	}
	else{
		g_object_set(G_OBJECT(gstPipeline), "video-sink", gstSink, (void*)NULL);
	}

#ifdef TARGET_WIN32
	GstElement *audioSink = gst_element_factory_make("directsoundsink", NULL);
	g_object_set(G_OBJECT(gstPipeline), "audio-sink", audioSink, (void*)NULL);

#endif


	videoUtils.setPipelineWithSink(gstPipeline, gstSink, bIsStream);
	videoUtils.startPipeline();
	if (!bIsStream) return allocate(bpp);
	else return true;
}

static void
pad_added_cb(GstElement* rtpbin, GstPad* new_pad, GstElement* depay)
{
	GstPad* sinkpad;
	GstPadLinkReturn lres;
	g_print("new payload on pad: %s\n", GST_PAD_NAME(new_pad));
	sinkpad = gst_element_get_static_pad(depay, "sink");
	assert(sinkpad);
	lres = gst_pad_link(new_pad, sinkpad);
	assert(lres == GST_PAD_LINK_OK);
	gst_object_unref(sinkpad);
}
#ifdef OLD_IMPL
bool	ofGstVideoPlayer::Connect(const core::string& ip, int remotePort, int localPort)
{

	GstElement * gstPipeline, *rtp_src, *rtcp_src, *rtcp_sink, *depayloader, *h264_decoder, *csp_filter, *screen_sink, *rtp_bin;
	GstPadLinkReturn lres;
	GstPad* srcpad,
		*sinkpad;
	GstCaps* caps;
	GstBus* bus;


	close();
	
	core::string message = "ofGstVideoPlayer - Connect(): connecting to \"" + ip + "\"";
	gLogManager.log(message, ELL_INFO, EVL_Heavy);

	ofGstUtils::startGstMainLoop();
	bIsStream = true;
	bRemote = true;

	// Create pipeline and attach a callback to it's message bus
	gstPipeline =  gst_pipeline_new("pipeline0");
	g_signal_connect(gstPipeline, "deep-notify", G_CALLBACK(gst_object_default_deep_notify), NULL);
	bus = gst_pipeline_get_bus(GST_PIPELINE(gstPipeline));
	//	gst_bus_add_watch( bus, ( GstBusFunc )gstreamer_bus_callback, pipeline );
	gst_object_unref(GST_OBJECT(bus));

	// Create elements
	rtp_src = gst_element_factory_make("udpsrc", "rtp_src");
	rtcp_src = gst_element_factory_make("udpsrc", "rtcp_src");
	rtcp_sink = gst_element_factory_make("udpsink", "rtcpsink");
	depayloader = gst_element_factory_make("rtph264depay", "h264_deplayloader");
	h264_decoder = gst_element_factory_make("ffdec_h264", "h264_decoder");
	csp_filter = gst_element_factory_make("ffmpegcolorspace", "csp_filter");
	 screen_sink = gst_element_factory_make("appsink", NULL);//( "autovideosink", "screen_sink" );
	rtp_bin = gst_element_factory_make("gstrtpbin", "rtpbin");

	assert(rtp_src);
	assert(rtcp_src);
	assert(rtcp_sink);
	assert(depayloader);
	assert(h264_decoder);
	assert(csp_filter);
	assert(screen_sink);
	assert(rtp_bin);

#if 0
	g_object_set(G_OBJECT(rtp_src), "sockfd", call->rtp_socket, NULL);
	g_object_set(G_OBJECT(rtp_src), "port", (int)call->local_video_port, NULL);
	g_object_set(G_OBJECT(rtp_src), "multicast-group", call->local_ip, NULL);

	g_object_set(G_OBJECT(rtcp_src), "sockfd", call->rtcp_socket, NULL);
	g_object_set(G_OBJECT(rtcp_src), "multicast-group", call->local_ip, NULL);
	g_object_set(G_OBJECT(rtcp_sink), "sockfd", call->rtcp_socket, NULL);
	g_object_set(G_OBJECT(rtcp_sink), "port", call->remote_video_port + 1, "host", call->remote_ip, NULL);
	g_object_set(G_OBJECT(rtcp_sink), "async", FALSE, "sync", FALSE, NULL);
#else

	g_object_set(rtp_src, "port", localPort, NULL);
	g_object_set(G_OBJECT(rtcp_src), "port", localPort, NULL);
	g_object_set(G_OBJECT(rtcp_sink), "port", remotePort, "host", ip.c_str(), NULL);
	g_object_set(G_OBJECT(rtcp_sink), "async", FALSE, "sync", FALSE, NULL);
#endif
	//g_object_set( G_OBJECT( rtp_bin ), "latency", 100, NULL );

	// Add elements to the pipeline. This has to be done prior to linking them
	gst_bin_add_many(GST_BIN(gstPipeline), rtp_src, rtcp_src, rtcp_sink, depayloader, h264_decoder, csp_filter, NULL);

	//gstSink = gst_element_factory_make("appsink", NULL);
	caps = gst_caps_new_simple("video/x-raw-rgb",
		"depth", G_TYPE_INT, 24,
		NULL);
	gst_app_sink_set_caps(GST_APP_SINK(screen_sink), caps);
	gst_caps_unref(caps);
	gst_base_sink_set_sync(GST_BASE_SINK(screen_sink), false);

	g_object_set(G_OBJECT(gstPipeline), "video-sink", screen_sink, (void*)NULL);
#if 0
	if (!gst_element_link_filtered(rtp_src, depayloader, caps))
	{
		assert(!"error linking CAPS\n");
	}

	gst_caps_unref(caps);
#endif

	/* Specify the RTP stream that we expect */
	caps = gst_caps_new_simple("application/x-rtp",
		"clock-rate", G_TYPE_INT, 90000,
		"payload", G_TYPE_INT, 96,
		"encoding-name", G_TYPE_STRING, "H264",
		NULL);
	g_object_set(rtp_src, "caps", caps, NULL);
	gst_caps_unref(caps);

	if (!gst_element_link_many(depayloader, h264_decoder, csp_filter, /*screen_sink,*/ NULL))
	{
		assert(!"error linking elements\n");
	}

	gst_bin_add(GST_BIN(gstPipeline), rtp_bin);

	// now link all to the rtpbin, start by getting an RTP sinkpad for session 0
	srcpad = gst_element_get_static_pad(rtp_src, "src");
	sinkpad = gst_element_get_request_pad(rtp_bin, "recv_rtp_sink_0");
	lres = gst_pad_link(srcpad, sinkpad);
	assert(lres == GST_PAD_LINK_OK);
	gst_object_unref(srcpad);

	// get an RTCP sinkpad in session 0
	srcpad = gst_element_get_static_pad(rtcp_src, "src");
	sinkpad = gst_element_get_request_pad(rtp_bin, "recv_rtcp_sink_0");
	lres = gst_pad_link(srcpad, sinkpad);
	assert(lres == GST_PAD_LINK_OK);
	gst_object_unref(srcpad);
	gst_object_unref(sinkpad);

	// get an RTCP srcpad for sending RTCP back to the sender
	srcpad = gst_element_get_request_pad(rtp_bin, "send_rtcp_src_0");
	sinkpad = gst_element_get_static_pad(rtcp_sink, "sink");
	lres = gst_pad_link(srcpad, sinkpad);
	assert(lres == GST_PAD_LINK_OK);
	gst_object_unref(sinkpad);

	g_signal_connect(rtp_bin, "pad-added", G_CALLBACK(pad_added_cb), depayloader);
	/* give some stats when we receive RTCP */
	//	g_signal_connect( rtp_bin, "on-ssrc-active", G_CALLBACK( on_ssrc_active_cb ),depayloader );

	gst_element_set_state(gstPipeline, GST_STATE_PLAYING);
	bIsStream = true;
	videoUtils.setPipelineWithSink(gstPipeline, screen_sink, bIsStream);
	videoUtils.startPipeline();
	 return true;
}
#else

#define MAX_PACKET 4096

#define BUFF_SIZE (1024)
typedef ofGstVideoPlayer::CConnectData gst_app_t;
static gboolean read_data(gst_app_t *app)
{
	static char TmpPacket[MAX_PACKET];
	GstBuffer *buffer;
	guint8 *ptr;
	gint size;
	GstFlowReturn ret;
	uint len = BUFF_SIZE;
	network::NetAddress address = app->address;
	len = MAX_PACKET;
	gdouble ms = g_timer_elapsed(app->timer, NULL);
	/*if (ms > 1.0 / 20.0) */{
		network::UDPClientError err = app->client->RecvFrom(TmpPacket, &len, &address);
		if (err != network::UDP_SOCKET_ERROR_NONE || len == 0)
		{
			ret = gst_app_src_end_of_stream((GstAppSrc*)app->src);
			return false;
		}
		ptr = (guint8*)g_malloc(len);
		memcpy(ptr, TmpPacket, len);
		g_assert(ptr);
		size = len;
		buffer = gst_buffer_new();
		GST_BUFFER_MALLOCDATA(buffer) = ptr;
		GST_BUFFER_SIZE(buffer) = size;
		GST_BUFFER_DATA(buffer) = GST_BUFFER_MALLOCDATA(buffer);

		//g_signal_emit_by_name((GstAppSrc*)(app->src), "push-buffer", buffer, &ret);

		ret = gst_app_src_push_buffer((GstAppSrc*)(app->src), buffer);

		if (ret != GST_FLOW_OK){
			g_debug("push buffer returned %d for %d bytes \n", ret, size);
			return FALSE;
		}
		g_timer_start(app->timer);
		return TRUE;

	}
	/*
	if (size != BUFF_SIZE){
		ret = gst_app_src_end_of_stream((GstAppSrc*)app->src);
		g_debug("eos returned %d at %d\n", ret, __LINE__);
		return FALSE;
	}*/

	return TRUE;
}

static void start_feed(GstElement * pipeline, guint size, gst_app_t *app)
{
	if (app->sourceID == 0) {
		GST_DEBUG("start feeding");
		app->sourceID = g_idle_add((GSourceFunc)read_data, app);
	}
}

static void stop_feed(GstElement * pipeline, gst_app_t *app)
{
	if (app->sourceID != 0) {
		GST_DEBUG("stop feeding");
		g_source_remove(app->sourceID);
		app->sourceID = 0;
	}
}

static void on_pad_added(GstElement *element, GstPad *pad,gpointer data)
{
	GstPad *sinkpad;
	GstElement *depay = (GstElement *)data;

	/* We can now link this pad with the vorbis-decoder sink pad */
	g_print("Dynamic pad created, linking parse/depay\n");

	sinkpad = gst_element_get_static_pad(depay, "sink");

	gst_pad_link(pad, sinkpad);

	gst_object_unref(sinkpad);

}

bool	ofGstVideoPlayer::Connect(const core::string& ip, int remotePort, int localPort)
{
	GstElement * gstPipeline, *src,*parse,*depay,*dec,*mpeg,*sink;
	GstCaps* caps;
	GstBus* bus;


	close();

	core::string message = "ofGstVideoPlayer - Connect(): connecting to \"" + ip + "\"";
	gLogManager.log(message, ELL_INFO, EVL_Heavy);

	ofGstUtils::startGstMainLoop();
	bIsStream = true;
	bRemote = true;

	// Create pipeline and attach a callback to it's message bus
	gstPipeline = gst_pipeline_new("pipeline0");
	g_signal_connect(gstPipeline, "deep-notify", G_CALLBACK(gst_object_default_deep_notify), NULL);
	bus = gst_pipeline_get_bus(GST_PIPELINE(gstPipeline));
	//	gst_bus_add_watch( bus, ( GstBusFunc )gstreamer_bus_callback, pipeline );
	gst_object_unref(GST_OBJECT(bus));

	// Create elements
	src = gst_element_factory_make("appsrc", "app_src");
	//parse = gst_element_factory_make("pcapparse", "parse");
	depay = gst_element_factory_make("rtph264depay", "depay");
	dec = gst_element_factory_make("ffdec_h264", "dec");
	mpeg = gst_element_factory_make("ffmpegcolorspace", "mpeg");
	sink = gst_element_factory_make("appsink", "sink");//( "autovideosink", "screen_sink" );

	if (!src /*|| !parse*/ || !depay || !dec || !mpeg || !sink)
	{
		gLogManager.log("GstVideoPlayer()- elements not created", ELL_WARNING);
		return false;
	}
	g_object_set(G_OBJECT(sink), "async", FALSE, "sync", FALSE, NULL);

	gst_bin_add_many(GST_BIN(gstPipeline), src/*, parse*/, depay , dec, mpeg, sink, NULL);


	int bpp;
	core::string mime;
	switch (internalPixelFormat){
	case video::EPixel_LUMINANCE8:
		mime = "video/x-raw-gray";
		bpp = 8;
		break;
	case video::EPixel_R8G8B8:
		mime = "video/x-raw-rgb";
		bpp = 24;
		break;
	case video::EPixel_R8G8B8A8:
	case video::EPixel_B8G8R8A8:
		mime = "video/x-raw-rgb";
		bpp = 32;
		break;
	default:
		mime = "video/x-raw-rgb";
		bpp = 24;
		break;
	}

	caps = gst_caps_new_simple(mime.c_str(),
		"bpp", G_TYPE_INT, bpp,
		"depth", G_TYPE_INT, 24,
		NULL);
	/*
	//gstSink = gst_element_factory_make("appsink", NULL);
	caps = gst_caps_new_simple("video/x-raw-rgb",
		"depth", G_TYPE_INT, 24,
		NULL);*/
	gst_app_sink_set_caps(GST_APP_SINK(sink), caps);
	gst_caps_unref(caps);

	gst_base_sink_set_sync(GST_BASE_SINK(sink), true);
	gst_app_sink_set_max_buffers(GST_APP_SINK(sink), 8);
	gst_app_sink_set_drop(GST_APP_SINK(sink), true);
	gst_base_sink_set_max_lateness(GST_BASE_SINK(sink), -1);

	gst_base_sink_set_sync(GST_BASE_SINK(sink), false);

	g_object_set(G_OBJECT(gstPipeline), "video-sink", sink, (void*)NULL);
	/*
	caps = gst_caps_new_simple("video/x-h264",
		NULL);
	g_object_set(depay,"caps", caps,NULL);
	gst_caps_unref(caps);*/
	caps = gst_caps_new_simple("application/x-rtp",
		"media", G_TYPE_STRING, "video",
		"clock-rate", G_TYPE_INT, 90000,
		"payload", G_TYPE_INT, 96,
		"encoding-name", G_TYPE_STRING, "H264",
		NULL);

	g_object_set(src, "caps", caps, NULL);
	gst_caps_unref(caps);

	if (!gst_element_link_many(src, depay, dec, mpeg, sink, NULL))
	{
		gLogManager.log("GstVideoPlayer()- error linking elements",ELL_WARNING);
		return false;
	}
	m_connectData.sink = sink;
	m_connectData.dec = dec;
	m_connectData.src = src;
	m_connectData.mpeg = mpeg;
	m_connectData.sourceID = 0;
	m_connectData.timer = g_timer_new();

	m_connectData.address = network::NetAddress(ip, remotePort);
	m_connectData.client = network::INetwork::getInstance().createUDPClient();
	m_connectData.client->Open(localPort);
	g_signal_connect(src, "need-data", G_CALLBACK(start_feed), &m_connectData);
	g_signal_connect(src, "enough-data", G_CALLBACK(stop_feed), &m_connectData);
	g_signal_connect(src, "pad-added", G_CALLBACK(on_pad_added), &parse);

	if (threadAppSink){
		GstElement * appQueue = gst_element_factory_make("queue", "appsink_queue");
		g_object_set(G_OBJECT(appQueue), "leaky", 0, "silent", 1, (void*)NULL);
		GstElement* appBin = gst_bin_new("app_bin");
		gst_bin_add(GST_BIN(appBin), appQueue);
		GstPad* appQueuePad = gst_element_get_static_pad(appQueue, "sink");
		GstPad* ghostPad = gst_ghost_pad_new("app_bin_sink", appQueuePad);
		gst_object_unref(appQueuePad);
		gst_element_add_pad(appBin, ghostPad);

		gst_bin_add_many(GST_BIN(appBin), sink, NULL);
		gst_element_link_many(appQueue, sink, NULL);

		g_object_set(G_OBJECT(gstPipeline), "video-sink", appBin, (void*)NULL);
	}
	else{
		g_object_set(G_OBJECT(gstPipeline), "video-sink", sink, (void*)NULL);
	}


	//gst_element_set_state(gstPipeline, GST_STATE_PLAYING);
	bIsStream = true;
	videoUtils.setPipelineWithSink(gstPipeline, sink, bIsStream);
	videoUtils.startPipeline();
	return true;
}
#endif
void ofGstVideoPlayer::setThreadAppSink(bool threaded){
	threadAppSink = threaded;
}


bool ofGstVideoPlayer::allocate(int bpp){
	if (bIsAllocated) return true;

	guint64 durationNanos = videoUtils.getDurationNanos();

	nFrames = 0;
	if (GstPad* pad = gst_element_get_static_pad(videoUtils.getSink(), "sink")){
#if GST_VERSION_MAJOR==0
		int width, height;
		if (gst_video_get_size(GST_PAD(pad), &width, &height)){
			if (!videoUtils.allocate(width, height, bpp)) return false;
		}
		else{
			gLogManager.log("ofGstVideoPlayer - allocate(): couldn't query width and height",ELL_WARNING);
			return false;
		}

		const GValue *framerate = gst_video_frame_rate(pad);
		fps_n = 0;
		fps_d = 0;
		if (framerate && GST_VALUE_HOLDS_FRACTION(framerate)){
			fps_n = gst_value_get_fraction_numerator(framerate);
			fps_d = gst_value_get_fraction_denominator(framerate);
			nFrames = (float)(durationNanos / GST_SECOND) * (float)fps_n / (float)fps_d;
			gLogManager.log("ofGstVideoPlayer - allocate(): framerate: " + core::StringConverter::toString(fps_n) + "/" + core::StringConverter::toString(fps_d),ELL_INFO);
		}
		else{
			gLogManager.log("ofGstVideoPlayer - allocate(): cannot get framerate, frame seek won't work",ELL_WARNING);
		}
		bIsAllocated = true;
#else
		if (GstCaps *caps = gst_pad_get_current_caps(GST_PAD(pad))){
			GstVideoInfo info;
			gst_video_info_init(&info);
			if (gst_video_info_from_caps(&info, caps)){
				if (!videoUtils.allocate(info.width, info.height, bpp)) return false;
			}
			else{
				ofLogError("ofGstVideoPlayer") << "allocate(): couldn't query width and height";
				return false;
			}

			fps_n = info.fps_n;
			fps_d = info.fps_d;
			nFrames = (float)(durationNanos / GST_SECOND) * (float)fps_n / (float)fps_d;
			gst_caps_unref(caps);
			bIsAllocated = true;
		}
		else{
			ofLogError("ofGstVideoPlayer") << "allocate(): cannot get pipeline caps";
			bIsAllocated = false;
		}
#endif
		gst_object_unref(GST_OBJECT(pad));
	}
	else{
		gLogManager.log("ofGstVideoPlayer - allocate(): cannot get sink pad",ELL_WARNING);
		bIsAllocated = false;
	}

	return bIsAllocated;
}

void ofGstVideoPlayer::on_stream_prepared(){
	if (!bIsAllocated) allocate(24);
}

int	ofGstVideoPlayer::getCurrentFrame(){
	int frame = 0;

	// zach I think this may fail on variable length frames...
	float pos = getPosition();
	if (pos == -1) return -1;


	float  framePosInFloat = ((float)getTotalNumFrames() * pos);
	int    framePosInInt = (int)framePosInFloat;
	float  floatRemainder = (framePosInFloat - framePosInInt);
	if (floatRemainder > 0.5f) framePosInInt = framePosInInt + 1;
	//frame = (int)ceil((getTotalNumFrames() * getPosition()));
	frame = framePosInInt;

	return frame;
}

int	ofGstVideoPlayer::getTotalNumFrames(){
	return nFrames;
}

void ofGstVideoPlayer::firstFrame(){
	setFrame(0);
}

void ofGstVideoPlayer::nextFrame(){
	gint64 currentFrame = getCurrentFrame();
	if (currentFrame != -1) setFrame(currentFrame + 1);
}

void ofGstVideoPlayer::previousFrame(){
	gint64 currentFrame = getCurrentFrame();
	if (currentFrame != -1) setFrame(currentFrame - 1);
}

void ofGstVideoPlayer::setFrame(int frame){ // frame 0 = first frame...
	float pct = (float)frame / (float)nFrames;
	setPosition(pct);
}

bool ofGstVideoPlayer::isStream(){
	return bIsStream;
}

void ofGstVideoPlayer::update(){
	videoUtils.update();
}

void ofGstVideoPlayer::play(){
	videoUtils.play();
}

void ofGstVideoPlayer::stop(){
	videoUtils.stop();
}

void ofGstVideoPlayer::setPaused(bool bPause){
	videoUtils.setPaused(bPause);
}

bool ofGstVideoPlayer::isPaused(){
	return videoUtils.isPaused();
}

bool ofGstVideoPlayer::isLoaded(){
	return videoUtils.isLoaded();
}

bool ofGstVideoPlayer::isPlaying(){
	return videoUtils.isPlaying();
}

float ofGstVideoPlayer::getPosition(){
	return videoUtils.getPosition();
}

float ofGstVideoPlayer::getSpeed(){
	return videoUtils.getSpeed();
}

float ofGstVideoPlayer::getDuration(){
	return videoUtils.getDuration();
}

bool ofGstVideoPlayer::getIsMovieDone(){
	return videoUtils.getIsMovieDone();
}

void ofGstVideoPlayer::setPosition(float pct){
	videoUtils.setPosition(pct);
}

void ofGstVideoPlayer::setVolume(float volume){
	videoUtils.setVolume(volume);
}

void ofGstVideoPlayer::setLoopState(int state){
	videoUtils.setLoopState(state);
}

int ofGstVideoPlayer::getLoopState(){
	return videoUtils.getLoopState();
}

void ofGstVideoPlayer::setSpeed(float speed){
	videoUtils.setSpeed(speed);
}

void ofGstVideoPlayer::close(){
	bIsAllocated = false;
	videoUtils.close();
}

bool ofGstVideoPlayer::isFrameNew(){
	return videoUtils.isFrameNew();
}

unsigned char * ofGstVideoPlayer::getPixels(){
	return videoUtils.getPixels();
}

video::ImageInfo* ofGstVideoPlayer::getPixelsRef(){
	return videoUtils.getPixelsRef();
}

float ofGstVideoPlayer::getHeight(){
	return videoUtils.getHeight();
}

float ofGstVideoPlayer::getWidth(){
	return videoUtils.getWidth();
}

ofGstVideoUtils * ofGstVideoPlayer::getGstVideoUtils(){
	return &videoUtils;
}

void ofGstVideoPlayer::setFrameByFrame(bool frameByFrame){
	videoUtils.setFrameByFrame(frameByFrame);
}