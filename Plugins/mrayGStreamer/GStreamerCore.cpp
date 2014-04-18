

#include "stdafx.h"
#include "GStreamerCore.h"

#include "ILogManager.h"
#include <IThreadManager.h>

#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

#include <glib-object.h>
#include <glib.h>
#include <algorithm>
#include "CMySrc.h"
#include "CMySink.h"

namespace mray
{
namespace video
{

GStreamerCore* GStreamerCore::m_instance=0;
uint GStreamerCore::m_refCount = 0;

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
		//			startThread();
	}
	void execute(OS::IThread*caller, void*arg){
		main_loop = g_main_loop_new(NULL, FALSE);
		g_main_loop_run(main_loop);
		printf("GST Thread shutdown\n");
	}
};





GStreamerCore::GStreamerCore()
{
	m_mainLoopThread = 0;
	_Init();
}

GStreamerCore::~GStreamerCore()
{
	_StopLoop();
	gst_deinit();
}


void GStreamerCore::_Init()
{

	GError *err = 0;
	if (!gst_init_check(NULL, NULL, &err))
	{
		gLogManager.log("GStreamerCore - Failed to init GStreamer!"+ core::StringConverter::toString(err->message), ELL_ERROR, EVL_Heavy);
	}
	else
	{
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
		gLogManager.log("GStreamerCore - GStreamer inited", ELL_INFO, EVL_Heavy);
	}

#if GLIB_MINOR_VERSION<32
	if (!g_thread_supported()){
		g_thread_init(NULL);
	}
#endif
	_StartLoop();
}
void GStreamerCore::_StartLoop()
{
	m_threadFunc = new GstMainLoopThread();
	m_mainLoopThread = OS::IThreadManager::getInstance().createThread(m_threadFunc);
	m_mainLoopThread->start(0);
}

void GStreamerCore::_StopLoop()
{
	if (!m_threadFunc)
		return;
	GstMainLoopThread* mainLoop = (GstMainLoopThread*)m_threadFunc;
	g_main_loop_quit(mainLoop->main_loop);
	bool running = g_main_loop_is_running(mainLoop->main_loop);
	g_main_loop_unref(mainLoop->main_loop);
	delete m_threadFunc;
	OS::IThreadManager::getInstance().killThread(m_mainLoopThread);
	delete m_mainLoopThread;
	m_threadFunc = 0;
	m_mainLoopThread = 0;
}

		
void GStreamerCore::Ref()
{
	m_refCount++;
	if (m_refCount==1)
	{
		m_instance = new GStreamerCore();
	}

}

void GStreamerCore::Unref()
{
	if (m_refCount == 0)
	{
		gLogManager.log("GStreamerCore::Unref() - unreferencing GStreamer with no reference! ", ELL_ERROR);
		return;
	}
	m_refCount--;
	if (m_refCount == 0)
	{
		delete m_instance;
		m_instance = 0;
	}
}

GStreamerCore* GStreamerCore::Instance()
{
	return m_instance;
}

}
}


