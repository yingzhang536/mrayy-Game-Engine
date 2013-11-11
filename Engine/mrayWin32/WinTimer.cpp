#include "stdafx.h"


#include "WinTimer.h"
#include <stdio.h>
#include "mMath.h"

namespace mray{
	


namespace OS{


WinTimer::WinTimer(){
	highPerformanceTimerSupport=0;

	m_startTime=0;
	reminder=0;

	resolution				=1000;
	initTimer();
	m_startTime=getActualTime();
}

/**/
void WinTimer::initTimer()
{
	highPerformanceTimerSupport=QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);


	//	staticTime = getActualTime();
	//	startRealTimer = staticTime;
	if(highPerformanceTimerSupport){
		highPerformanceSupport=1;
		QueryPerformanceCounter(&performanceTimerStart);
		resolution=(double)(((double)1.0f)/(double)frequency.QuadPart);
	}
	else
	{
		highPerformanceSupport=0;
	}
}

ulong WinTimer::getActualTime(void)
{
	
	if(highPerformanceSupport)
	{
		static LARGE_INTEGER Time;
		QueryPerformanceCounter(&Time);
		double t1=(Time.QuadPart -performanceTimerStart.QuadPart)*resolution*1000.0f;
		t1+=reminder;
		reminder=t1-(double)(floor(t1));
		return t1;
	}else
		return (ulong)(GetTickCount())-m_startTime;
	
}

double WinTimer::getActualTimeAccurate(void)
{
	static LARGE_INTEGER Time;
	QueryPerformanceCounter(&Time);
	double t1=(Time.QuadPart -performanceTimerStart.QuadPart)*resolution*1000.0f;
	t1+=reminder;
	reminder=t1-(double)(floor(t1));
	return t1;

}





}
}
