



#include "stdafx.h"
/* FIXME 0.11: suppress warnings for deprecated API such as GValueArray
* with newer GLib versions (>= 2.31.0) */
#define GLIB_DISABLE_DEPRECATION_WARNINGS

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "CMyUDPSink.h"

#include <string.h>
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif

#ifndef G_OS_WIN32
#include <netinet/in.h>
#endif

#include <gst/gstelement.h>

#define UDP_DEFAULT_HOST        "localhost"
#define UDP_DEFAULT_PORT        5004

enum
{
	PROP_0,
	PROP_HOST,
	PROP_PORT,
};

GST_DEBUG_CATEGORY_STATIC(MyUDPSink_debug);
#define GST_CAT_DEFAULT (MyUDPSink_debug)

static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE("sink",
	GST_PAD_SINK,
	GST_PAD_ALWAYS,
	GST_STATIC_CAPS_ANY);


static void gst_MyUDPSink_finalize(GObject * object);

static GstFlowReturn gst_MyUDPSink_render(GstBaseSink * sink,
	GstBuffer * buffer);

static gboolean gst_MyUDPSink_start(GstBaseSink * bsink);
static gboolean gst_MyUDPSink_stop(GstBaseSink * bsink);
static gboolean gst_MyUDPSink_unlock(GstBaseSink * bsink);
static gboolean gst_MyUDPSink_unlock_stop(GstBaseSink * bsink);

static void gst_MyUDPSink_set_property(GObject * object, guint prop_id,
	const GValue * value, GParamSpec * pspec);
static void gst_MyUDPSink_get_property(GObject * object, guint prop_id,
	GValue * value, GParamSpec * pspec);


//static guint gst_MyUDPSink_signals[LAST_SIGNAL] = { 0 };

#define gst_MyUDPSink_parent_class parent_class
G_DEFINE_TYPE(GstMyUDPSink, gst_MyUDPSink, GST_TYPE_BASE_SINK);

static void
gst_MyUDPSink_class_init(GstMyUDPSinkClass * klass)
{
	GObjectClass *gobject_class;
	GstElementClass *gstelement_class;
	GstBaseSinkClass *gstbasesink_class;

	gobject_class = (GObjectClass *)klass;
	gstelement_class = (GstElementClass *)klass;
	gstbasesink_class = (GstBaseSinkClass *)klass;

	gobject_class->set_property = gst_MyUDPSink_set_property;
	gobject_class->get_property = gst_MyUDPSink_get_property;
	gobject_class->finalize = gst_MyUDPSink_finalize;


	g_object_class_install_property(G_OBJECT_CLASS(klass), PROP_HOST,
		g_param_spec_string("host", "host",
		"The host/IP/Multicast group to send the packets to",
		UDP_DEFAULT_HOST, G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));
	g_object_class_install_property(G_OBJECT_CLASS(klass), PROP_PORT,
		g_param_spec_int("port", "port", "The port to send the packets to",
		0, 65535, UDP_DEFAULT_PORT,
		G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

	gst_element_class_add_pad_template(gstelement_class,
		gst_static_pad_template_get(&sink_template));

	//gst_element_class_set_details()
	gst_element_class_set_details_simple(gstelement_class, "MyUDPSink class",
		"Sink",
		"Custom UDP Sink",
		"MHD Yamen Saraiji <mrayyamen@gmail.com>");
	gstbasesink_class->render = gst_MyUDPSink_render;
	gstbasesink_class->start = gst_MyUDPSink_start;
	gstbasesink_class->stop = gst_MyUDPSink_stop;
	gstbasesink_class->unlock = gst_MyUDPSink_unlock;
	gstbasesink_class->unlock_stop = gst_MyUDPSink_unlock_stop;


	GST_DEBUG_CATEGORY_INIT(MyUDPSink_debug, "MyUDPSink", 0, "My sink");
}


static void
gst_MyUDPSink_init(GstMyUDPSink * sink)
{
	sink->new_buffer = 0;
	sink->data = 0;
}

static void
gst_MyUDPSink_finalize(GObject * object)
{
	GstMyUDPSink *sink;

	sink = GST_MyUDPSink(object);

	G_OBJECT_CLASS(parent_class)->finalize(object);
}

static GstFlowReturn
gst_MyUDPSink_render(GstBaseSink * bsink, GstBuffer * buffer)
{
	GstMyUDPSink *sink;
	GError *err = NULL;

	sink = GST_MyUDPSink(bsink);
	if (sink->new_buffer)
	{
		return sink->new_buffer(sink, sink->data, buffer);
	}
	return GST_FLOW_OK;
}


static void
gst_MyUDPSink_set_property(GObject * object, guint prop_id,
const GValue * value, GParamSpec * pspec)
{
	GstMyUDPSink *sink = GST_MyUDPSink(object);

}

static void
gst_MyUDPSink_get_property(GObject * object, guint prop_id, GValue * value,
GParamSpec * pspec)
{
	GstMyUDPSink *sink = GST_MyUDPSink(object);

}


static gboolean
gst_MyUDPSink_start(GstBaseSink * bsink)
{
	GstMyUDPSink *sink;
	GError *err = NULL;

	sink = GST_MyUDPSink(bsink);

	return TRUE;
}

static gboolean
gst_MyUDPSink_stop(GstBaseSink * bsink)
{
	GstMyUDPSink *udpsink;

	udpsink = GST_MyUDPSink(bsink);


	return TRUE;
}

void
gst_MyUDPSink_clear(GstMyUDPSink * sink)
{
}

static gboolean
gst_MyUDPSink_unlock(GstBaseSink * bsink)
{
	GstMyUDPSink *sink;

	sink = GST_MyUDPSink(bsink);

	//g_cancellable_cancel(sink->cancellable);

	return TRUE;
}

static gboolean
gst_MyUDPSink_unlock_stop(GstBaseSink * bsink)
{
	GstMyUDPSink *sink;

	sink = GST_MyUDPSink(bsink);

	//g_cancellable_reset(sink->cancellable);

	return TRUE;
}

gboolean
_GstMyUDPSinkClass::plugin_init(GstPlugin * plugin)
{
	if (!gst_element_register(plugin, "MyUDPSink", GST_RANK_NONE,
		GST_TYPE_MyUDPSink))
		return FALSE;

	return TRUE;
}

/*#define PACKAGE ""
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
GST_VERSION_MINOR,
"MyUDPSink",
"Custom app sink",
_GstMyUDPSinkClass::plugin_init, "", "", "", "")
*/

