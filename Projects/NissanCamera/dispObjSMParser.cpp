/**
 * @file dispObjSMParser.cpp
 *  comment
 * @author taq@nemesys.co.jp
 * @since 2014.07
 */
#include "stdafx.h"
#include "dispObjSMParser.h"

//-----------------
// parser
//-----------------

bool DispObjSMParser::parseData(const char *data, std::vector<DispObjHdr *> *pObjVec)
{
	DispSMHdr *pSMH = (DispSMHdr *) data;
	if (pSMH->signature != DISPSMHDR_SIGN)
		return false;
	int numObjs = pSMH->numObjects;
	DispObjHdr *pOH = pSMH->toFirstDispObj();
	while (numObjs--) {
		pObjVec->push_back(pOH);
		pOH = pOH->toNext();
	}
	return true;
}

bool DispObjSMParser::asGLObject(DispObjHdr *pHdr, DOSM_GLObject *pRet)
{
	pRet->points.clear();
	pRet->normals.clear();
	pRet->colors.clear();

	if (pHdr->oh.type != DISPOBJ_TYPE_GL_OBJECT) {
		pRet->pHdr = NULL;
		pRet->pGL = NULL;
		return false;
	}
	pRet->pHdr = pHdr;
	DispObjGLObject *pGL = pRet->pGL = pHdr->getData<DispObjGLObject>();

	int n = 0;
	for (int i = 0; i < pGL->nPoints; i++) {
		DispObjXYZ xyz;
		xyz.init(pGL->data[n], pGL->data[n + 1], pGL->data[n + 2]);
		pRet->points.push_back(xyz);
		n += 3;
	}
	for (int i = 0; i < pGL->nNormals; i++) {
		DispObjXYZ xyz;
		xyz.init(pGL->data[n], pGL->data[n + 1], pGL->data[n + 2]);
		pRet->normals.push_back(xyz);
		n += 3;
	}
	for (int i = 0; i < pGL->nColors; i++) {
		DispObjRGBA rgba;
		rgba.init(pGL->data[n], pGL->data[n + 1], pGL->data[n + 2], pGL->data[n + 3]);
		pRet->colors.push_back(rgba);
		n += 4;
	}
	return true;
}

bool DispObjSMParser::asPredefined(DispObjHdr *pHdr, DOSM_Predefined *pRet)
{
	if (pHdr->oh.type != DISPOBJ_TYPE_PREDEFINED) {
		pRet->pHdr = NULL;
		pRet->pPredefined = NULL;
		return false;
	}
	pRet->pHdr = pHdr;
	pRet->pPredefined = pHdr->getData<DispObjPredefined>();
	return true;
}

bool DispObjSMParser::asString(DispObjHdr *pHdr, DOSM_String *pRet)
{
	if (pHdr->oh.type != DISPOBJ_TYPE_STRING) {
		pRet->pHdr = NULL;
		pRet->pString = NULL;
		return false;
	}
	pRet->pHdr = pHdr;
	pRet->pString = pHdr->getData<DispObjString>();
	char *p = pRet->pFont = pRet->pString->font_data;
	while (*p)
		p++;
	pRet->pData = ++p;
	return true;
}

bool DispObjSMParser::asVehicleData(DispObjHdr *pHdr, DOSM_VehicleData *pRet)
{
	if (pHdr->oh.type != DISPOBJ_TYPE_VEHICLEDATA) {
		pRet->pHdr = NULL;
		pRet->pVD = NULL;
		return false;
	}
	pRet->pHdr = pHdr;
	pRet->pVD = pHdr->getData<DispObjVehicleData>();
	return true;
}
