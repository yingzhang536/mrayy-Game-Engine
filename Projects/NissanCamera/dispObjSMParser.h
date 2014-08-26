#pragma once
#ifndef INCLUDE_DISPOBJSMPARSER_H
#define INCLUDE_DISPOBJSMPARSER_H

#include "dispObjSM.h"
#include <vector>

// classes for parsed result
class DOSM_GLObject {
public:
	DispObjHdr *pHdr;
	DispObjGLObject *pGL;
	std::vector<DispObjXYZ> points;
	std::vector<DispObjXYZ> normals;
	std::vector<DispObjRGBA> colors;
};

class DOSM_Predefined {
public:
	DispObjHdr *pHdr;
	DispObjPredefined *pPredefined;
};

class DOSM_String {
public:
	DispObjHdr *pHdr;
	DispObjString *pString;
	char *pFont;
	char *pData;	// string to display
};

class DOSM_VehicleData {
public:
	DispObjHdr *pHdr;
	DispObjVehicleData *pVD;
};

class DispObjSMParser {
public:
	// parse raw data into object header ptrs
	bool parseData(const char *data, std::vector<DispObjHdr *> *pObjVec);
			// data: must point to DispSMHdr

	// parse object header ptrs to appropriate types
	bool asGLObject(DispObjHdr *pHdr, DOSM_GLObject *pGLObject);
	bool asString(DispObjHdr *pHdr, DOSM_String *pString);
	bool asPredefined(DispObjHdr *pHdr, DOSM_Predefined *pPredefined);
	bool asVehicleData(DispObjHdr *pHdr, DOSM_VehicleData *pVehicleData);
};

#if 0	// sample usage
		DispObjSMParser parser;
		std::vector<DispObjHdr *> objects;
		parser.parseData(data);	// assume data points to DispSMHdr
		for (size_t i = 0; i < objects.size(); i++) {
			DOSM_GLObject glObj;
			DOSM_Predefined predefined;
			DOSM_String str;
			DOSM_VehicleData vd;
			if (parser.asGLObject(objects[i], &glObj)) {
				// code for GLObject
			} else if (parser.asPredefined(pHdr, &predefined)) {
				// code for Predefined object
			} else if ...
		}
#endif	// sample usage

#endif	// !defined(INCLUDE_DISPOBJSMPARSER_H)
