// NissanCamera.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "NissanApplication.h"
#include "GCCollector.h"

#include "compileconfig.h"

#include <windows.h>

#include <nvapi.h>
#include <NvApiDriverSettings.h>


using namespace mray;
using namespace core;
using namespace math;

void PrintError(NvAPI_Status status)
{
	NvAPI_ShortString szDesc = {0};
	NvAPI_GetErrorMessage(status, szDesc);
	printf(" NVAPI error: %s\n", szDesc);
	exit(-1);
}
APPLICATION_ENTRY_POINT
{

	GCPtr<NissanApplication> app=new NissanApplication();

	core::string resFileName=mT("plugins.stg");

#ifdef UNICODE
	resFileName=mT("pluginsU.stg");
#endif

	gLogManager.setVerbosLevel(EVL_Heavy);

	NvAPI_Status status=NvAPI_Initialize();
	if(status!=NVAPI_OK)
	{
		PrintError(status);
	}

	NvDRSSessionHandle hSession=0;
	status=NvAPI_DRS_CreateSession(&hSession);
	if(status!=NVAPI_OK)
	{
		PrintError(status);
	}

	status=NvAPI_DRS_LoadSettings(hSession);
	if(status!=NVAPI_OK)
	{
		PrintError(status);
	}

	NvDRSProfileHandle hProfile=0;

	status=NvAPI_DRS_GetBaseProfile(hSession,&hProfile);
	if(status!=NVAPI_OK)
	{
		PrintError(status);
	}
	NVDRS_SETTING drsSetting;

	drsSetting.version=NVDRS_SETTING_VER;
	drsSetting.settingId=VSYNCMODE_ID;
	drsSetting.settingType=NVDRS_DWORD_TYPE;
	drsSetting.u32CurrentValue=VSYNCMODE_FORCEOFF;
	status=NvAPI_DRS_SetSetting(hSession,hProfile,&drsSetting);
	if(status!=NVAPI_OK)
	{
		PrintError(status);
	}
	status=NvAPI_DRS_SaveSettings(hSession);
	if(status!=NVAPI_OK)
	{
		PrintError(status);
	}
	status=NvAPI_DRS_DestroySession(hSession);
	if(status!=NVAPI_OK)
	{
		PrintError(status);
	}






	std::vector<SOptionElement> extraOptions;
	SOptionElement op;
	op.name="Stereoscopic";
	op.value="None";
	op.valueSet.insert("None");
	op.valueSet.insert("OverUnder");
	op.valueSet.insert("SideBySide");
	op.valueSet.insert("DVI");
	extraOptions.push_back(op);
	op.valueSet.clear();
	op.name="Debugging";
	op.value="No";
	op.valueSet.insert("Yes");
	op.valueSet.insert("No");
	extraOptions.push_back(op);
	op.valueSet.clear();
	op.name="HorizontalFlip";
	op.value="No";
	op.valueSet.insert("Yes");
	op.valueSet.insert("No");
	extraOptions.push_back(op);
	op.valueSet.clear();
	op.name="USBCamera";
	op.value="Yes";
	op.valueSet.insert("Yes");
	op.valueSet.insert("No");
	extraOptions.push_back(op);
	op.valueSet.clear();
	//VLDEnable();
	app->loadResourceFile(mT("vtdataPath.stg"));
	if(app->startup(mT("Nissan"),vector2di(800,600),false,extraOptions,resFileName,0,true,false,true))
	{
		app->run();
	}

	//	VLDDisable();
	app=0;

	GCCollector::shutdown();
	return 0;
}

