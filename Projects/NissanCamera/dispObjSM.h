#pragma once
#ifndef INCLUDE_DISPOBJSM_H
#define INCLUDE_DISPOBJSM_H

#ifdef _WIN32
	#ifndef _INT32_DEFINED_
	#define _INT32_DEFINED_
	typedef int int32_t;
	typedef unsigned int uint32_t;
	#endif
#else
#include <stdint.h>
#endif

// set structure alignment (to packed)
#pragma pack(push, 1) // valid both on VC++ & gcc

// common header for all object types (including Shared Mem header)
typedef struct {        
	uint32_t type;		// type of object (DISPOBJ_TYPE_*)
	uint32_t bytes;		// bytes for this object
							// includes type & bytes field
	uint32_t objId;		// object Id  (0 for Shared Mem header)
} DispSMObjHdr;

// values objHdr.type 
#define DISPOBJ_TYPE_HEADER	0   // Shared Mem header, includes # of objects

#define DISPOBJ_TYPE_GL_OBJECT	1		// OpenGL object (e.g. Triangle strip)
#define DISPOBJ_TYPE_PREDEFINED		2		// Predefined objects
#define DISPOBJ_TYPE_STRING		3		// string objects
#define DISPOBJ_TYPE_VEHICLEDATA		4		// Vehicle data

// values DispSMHdr.signature below
#define DISPSMHDR_SIGN		0x70736944 // L-endian "Disp"

typedef struct {
	DispSMObjHdr oh;		// common header,
							// oh.type == DISPOBJ_TYPE_HEADER
	uint32_t signature;		// value is DISPSMHDR_SIGN
	uint32_t version;		// version Number, for now 1
	uint32_t groupID;		// group ID for this object group
	uint32_t seq;			// sequence #. incremented on each change
	uint32_t sec;			// group commit time, sec (from epoch)
	uint32_t nsec;			// group  ommit time, nano sec
	uint32_t numObjects;	// # of objects in this group

	struct _DispObjHdr *toFirstDispObj()
	{
		return (struct _DispObjHdr *) (((char *) this) + this->oh.bytes);
	}

	void init()
	{
		oh.type = DISPOBJ_TYPE_HEADER;
		oh.bytes = sizeof (*this);
		oh.objId = 0;	// any
		signature = DISPSMHDR_SIGN;
		version = 1;
		// N.B. groupID & seq are not set by init()
	}
} DispSMHdr ;

typedef struct  {
	float x, y, z;
	void init(double _x, double _y, double _z) {
		x = (float) _x; y = (float) _y; z = (float) _z;
	}
	void init() {
		init(0.0, 0.0, 0.0);
	}
} DispObjXYZ;

typedef struct {
	float r, g, b, a;
	void init(double _r, double _g, double _b, double _a) {
		r = (float) _r; g = (float) _g; b = (float) _b; a = (float) _a;
	}
} DispObjRGBA;

typedef struct {
	DispObjXYZ pos; // position
	float heading; // radian. ccw from topview. 0 is -Y
	float pitch;   // radian. nosedive is +.
	float roll;    // radian. ccw looking front.
	void init() {
		pos.init();
		heading = pitch = roll = 0;
	}
} DispObjPosDir;	// position and direction(orientation)

// common attributes for displayable objects
typedef struct _DispObjCommonAttribute {
	int32_t bytes;   // bytes of structure. set to sizeof DispObjCommonAttribute
	int32_t version; // version #. is 1

	//type of origin, see ORIGIN_* below
	int32_t origin;

	// Window ID (only for origin == ORIGIN_WINDOW_*)
	int32_t windowID;

	// object position and orientation
	DispObjPosDir posDir;

	int32_t reserved[20];	// reserved. set to 0

	void init()
	{
		this->bytes = sizeof (*this);
		this->version = 1;
		this->windowID = 0;
		posDir.init();
		for (size_t i = 0; i < sizeof reserved / sizeof reserved[0]; i++) {
			reserved[i] = 0;
		}
	}

	void assignFrom(const struct _DispObjCommonAttribute *other)
	{
		this->origin = other->origin;
		if (other->version > 0) {
			this->windowID = other->windowID;
			this->posDir = other->posDir;
		}
		// future expansion
		// if (other->version > 1) {
		//    assignment for extended portions in version 2 and above
		// }
	}
} DispObjCommonAttribute;

// element values for DispObjCommonAttribute.origin (masks)
#define ORIGMASK_WINDOW 0x2 		//  Window coordinate (origin BottomLeft)
//#define ORIGMASK_WORLD  0x0 	//  World  (east:+X, north :+Y, up :+Z)
									// i.e. right-handed system 
#define ORIGMASK_OWNCAR 0x1 		// Car (left:+X, rear:+Y, up:+Z)
								// origin is x: middle of car, y: front of car, z: ground
#define ORIGMASK_2D_NORMALIZE 0x1 // Window, unit is not pixel but normalized to window size. (i.e. 1 is window width/height)
#define ORIGMASK_3D_TO_2D	0x4		// coordinate is 3D, but use 2D coordinate after transformation for rendering

// values for DispObjCommonAttribute.origin
#define ORIGIN_3DSPACE_3D_WORLD    0
			// 3D world coord -> display in 3D
#define ORIGIN_3DSPACE_3D_OWNCAR   (ORIGMASK_OWNCAR)
			// 3D car relative coord -> display in 3D
#define ORIGIN_WINDOW_3D_WORLD     (ORIGMASK_3D_TO_2D)
			// 3D world coord -> get 2D coord after transformation and display
#define ORIGIN_WINDOW_3D_OWNCAR    (ORIGMASK_3D_TO_2D | ORIGMASK_OWNCAR)
			// 3D car relative coord -> get 2D coord ...
#define ORIGIN_WINDOW_2D_PIXEL     (ORIGMASK_WINDOW)
			// 2D Pixel coord  -> display in window
#define ORIGIN_WINDOW_2D_NORMALIZE (ORIGMASK_WINDOW | ORIGMASK_2D_NORMALIZE)
			// 2D "normalized" coord -> display in window

// N.B. for handling origin values:
// 1. &'ing with  ORIGMASK_WINDOW gives 2D or 3D
// 2-a. for 2D, & ORIGMASK_2D_NORMALIZE tells pixel or "normalized" coord
// 2-b. for 3D 
//   & ORIGMASK_OWNCAR tells world or car relative
//   & ORIGMASK_3D_TO 2D tells whether to use 3D coord directly or use 2D coord after transformation

// displayable object common header

typedef struct _DispObjHdr {
	DispSMObjHdr oh;			// object common header
	DispObjCommonAttribute attr;   // displayable object common attributes

	template <typename T> T *getData() {
		return (T *) ((char *)(&(this->attr)) + this->attr.bytes);
	}

	template <typename T> const T *getConstData() const {
		return (T *) ((char *)(&(this->attr)) + this->attr.bytes);
	}

	struct _DispObjHdr *toNext()
	{
		return (struct _DispObjHdr *) (((char *) this) + this->oh.bytes);
	}
} DispObjHdr;

// struct definition for displayable object (data following DispObjHdr)
// use DispObjHdr.getData() to get pointer for data
// is one of DispObjGLObject, DispObjPredefined or DispObjString

//  GL Object types (DispObjGLObject.glObjType) (same as OpenGL values)
#define DISPOBJ_GL_LINES            0x0001 //GL_LINES
#define DISPOBJ_GL_LINE_LOOP        0x0002 //GL_LINE_LOOP
#define DISPOBJ_GL_LINE_STRIP       0x0003 //GL_LINE_STRIP
#define DISPOBJ_GL_TRIANGLES        0x0004 //GL_TRIANGLES
#define DISPOBJ_GL_TRIANGLE_STRIP   0x0005 //GL_TRIANGLE_STRIP
#define DISPOBJ_GL_TRIANGLE_FAN     0x0006 //GL_TRIANGLE_FAN

// Normal types (DispObjGLObject.normalType) 
#define DISPOBJ_NORMAL_NONE         0    // no normals (no shading)
#define DISPOBJ_NORMAL_VERTEX       1    // normal on each vertex
#define DISPOBJ_NORMAL_OVERALL      3    // 1 normal on whole object
#define DISPOBJ_NORMAL_AUTO_SMOOTH  4    // auto (smooth shading)
#define DISPOBJ_NORMAL_AUTO_FLAT    5    // auto (flat shading) (only for GL_TRIANGLES)

// color types (DispObjGLObject.colorType)
#define DISPOBJ_COLOR_NONE      0    // no colors (fixed color, defined by the viewer)
#define DISPOBJ_COLOR_VERTEX    1    // color on each vertex
#define DISPOBJ_COLOR_OVERALL   2    // 1 color on  whole object

// GL object
typedef struct {
	int32_t glObjType;	// openGL object Type (e.g. GL_TRIANGLE_STRIP) 

	float lineWidth;	// width of line (pixels)
	int32_t backCulling;	// back Culling ON/OFF (0: off, non-0 : on)

	int32_t normalType; // type of Normals (one of DISPOBJ_NORMAL_*)
	int32_t colorType;  // type of Colors (one of DISPOBJ_COLOR_*)

	int32_t nPoints;    // number of Points (XYZ)
	int32_t nNormals;   // number of Normals (XYZ)
	int32_t nColors;    // number of colors  (RGBA)
	float data[1];  // data, points/normals/colors

	void init() {
		lineWidth = 1.0;
		nPoints = nNormals = nColors = 0;
		normalType = DISPOBJ_NORMAL_NONE;
		colorType = DISPOBJ_COLOR_NONE;
		backCulling = 0;
	}
} DispObjGLObject;

// "predefined" object
typedef struct {
	float scaleX, scaleY, scaleZ;
	int32_t reserved[1];
		// reserved. set to 0
	char name[1];			// predefined name, '\0' terminated
							// usage is TBD
	void init() {
		scaleX = scaleY = scaleZ = 1.0;
		for (size_t i = 0; i < sizeof reserved / sizeof reserved[0]; i++) {
			reserved[i] = 0;
		}
	}
} DispObjPredefined;

#define DISPOBJ_FONTSIZETYPE_PIXEL 0	// font size is in pixels
#define DISPOBJ_FONTSIZETYPE_NORMALIZE	1 // fontsize is in "normalized" size, i.e. relative to window size

// string object
typedef struct {
	// position for the string is in DispObjCommonAttribute.posDir
	// posDir.pos.z, heading, pitch are ignored.
	// posDir.roll is also ignored (for now)
	int32_t fontSizeType;			// one of DISPOBJ_FONTSIZETYPE_*
	float fontSize;			// font size
	int32_t numColor;			// either 0 or 1 for now.
								// use defalut color (TBD) when 0
	DispObjRGBA color[1];	// is array (for future expansion)
	int32_t reserved[12 + 4];	// reserved. in the future, may be used as:
							// 1. additional RGBA x 3
							// 2. alignment type (left = 0/center/right x bottom = 0/top)
							// 3. bool useOutline
							// 4. string encoding type
							// 5. reserved
	char font_data[1];		// font data & string (both are '\0' terminated)

	void init() {
		fontSizeType = DISPOBJ_FONTSIZETYPE_PIXEL;
		numColor = 0;
		for (size_t i = 0; i < sizeof reserved / sizeof reserved[0]; i++) {
			reserved[i] = 0;
		}
	}
} DispObjString;

typedef struct {
	// Vehicle position is stored in DispObjHdr.origin, posDir
	// (origin is always ORIGIN_3DSPACE_3D_WORLD
	// no other info is defined at this time,.
	int32_t reserved[8];

	void init() {
		for (size_t i = 0; i < sizeof reserved / sizeof reserved[0]; i++) {
			reserved[i] = 0;
		}
	}
} DispObjVehicleData;
// restore structure alignment
#pragma pack(pop) // valid both on VC++ & gcc

#endif	// !defined(INCLUDE_DISPOBJSM_H)
