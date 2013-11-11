// VTelesar5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Application.h"
#include "GCCollector.h"
#include "DirectShowVideoGrabber.h"
#include "OFSerialPort.h"
#include <windows.h>
// #include <vld.h>
// #include <vldapi.h>


using namespace mray;
using namespace core;
using namespace math;

#ifdef _DEBUG
#define EntryPoint int main()
#else
#define EntryPoint int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)

#endif
EntryPoint
{

	GCPtr<TBee::Application> app=new TBee::Application();

	core::string resFileName=mT("plugins.stg");

#ifdef UNICODE
	resFileName=mT("pluginsU.stg");
#endif

	printf("%d\n",sizeof(std::string));


	gLogManager.setVerbosLevel(EVL_Heavy);

	std::vector<SOptionElement> extraOptions;
	SOptionElement op;
	op.valueSet.clear();
	op.name="Debugging";
	op.value="No";
	op.valueSet.insert("Yes");
	op.valueSet.insert("No");
	extraOptions.push_back(op);
	op.valueSet.clear();
	{
		op.name="Camera";
		video::DirectShowVideoGrabber ds;
		int camsCount=ds.ListDevices();
		for (int i=0;i<camsCount;++i)
		{
			op.valueSet.insert(ds.GetDeviceName(i));
		}
		if(op.valueSet.size()>0)
		{
			op.value=*op.valueSet.begin();
		}
		extraOptions.push_back(op);
		op.valueSet.clear();
	}
	{
		op.name="DVIPort";
		OS::OFSerialPortService spProvider;
		std::vector<OS::SerialPortInfo> ports= spProvider.EnumAvaliablePorts();
		for (int i=0;i<ports.size();++i)
		{
			op.valueSet.insert(ports[i].Name);
		}
		if(op.valueSet.size()>0)
		{
			op.value=*op.valueSet.begin();
		}
		extraOptions.push_back(op);
		op.valueSet.clear();
	}
	//VLDEnable();
	app->loadResourceFile(mT("tbdataPath.stg"));
	if(app->startup(mT("TELUBee 1.00"),vector2di(800,600),false,extraOptions,resFileName,0,true,true,false))
	{
		app->run();
	}

	//	VLDDisable();
	app=0;

	GCCollector::shutdown();
	return 0;
}

