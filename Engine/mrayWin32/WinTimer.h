

#ifndef ___WinTIMER___
#define ___WinTIMER___


#include <mTypes.h>
#include "compileConfig.h"
#include <ITimer.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace mray{
namespace OS{

class MRAY_Win32_DLL WinTimer:public ITimer
{
	
	int highPerformanceTimerSupport;

	ulong m_startTime;

	double resolution;
	LARGE_INTEGER performanceTimerStart;
	LARGE_INTEGER frequency;

	double reminder;
	
	bool highPerformanceSupport;
public:
	WinTimer();
	virtual~WinTimer(){};

	void initTimer();

	//! get ms value
	ulong getActualTime(void);

	//! get time in the internal precision (use with GetDifference100() )
	double getActualTimeAccurate();

	
};


};//OS
};//mray



#endif






















