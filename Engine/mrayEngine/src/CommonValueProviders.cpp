

#include "stdafx.h"
#include "CommonValueProviders.h"
#include "ITimer.h"
#include "CFPS.h"


namespace mray
{
	void TimeValueProvier::SetValue(const float &v)
	{
	}
	float TimeValueProvier::GetValue()
	{
		return gTimer.getActualTimeAccurate();
	}


	void FPSValueProvier::SetValue(const float &v)
	{
	}
	float FPSValueProvier::GetValue()
	{
		return gFPS.dt();
	}
}