// LeapMotionTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Application.h"


using namespace mray;
using namespace core;
using namespace math;

#define EntryPoint int main()

EntryPoint
{

	GCPtr<Application> app = new Application();

	core::string resFileName = mT("plugins.stg");
	std::vector<SOptionElement> extraOptions;


	//VLDEnable();
	app->loadResourceFile(mT("ncdataPath.stg"));
	if (app->startup(mT("Nissan Robot 1.00"), vector2di(800, 600), false, extraOptions, resFileName, 0, true, true, true))
	{
		app->run();
	}

	//	VLDDisable();
	app = 0;

	return 0;
}

