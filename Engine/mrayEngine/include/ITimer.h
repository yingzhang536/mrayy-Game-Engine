
/********************************************************************
	created:	2009/01/02
	created:	2:1:2009   16:21
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\ITimer.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	ITimer
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___ITimer___
#define ___ITimer___

#include "ISingleton.h"
#include "mTypes.h"

namespace mray{
namespace OS{

class MRAY_DLL ITimer:public ISingleton<ITimer>
{
public:
	ITimer(){}
	virtual~ITimer(){}

	virtual void initTimer()=0;

	//! get ms value
	virtual ulong getActualTime(void)=0;

	//! get time in the internal precision (use with GetDifference100() )
	virtual double getActualTimeAccurate()=0;
};

#define gTimer mray::OS::ITimer::getInstance()
}
}

//this macro calculates block execution time 
#define calcTime(dt)\
	for(float __aux=0, __t1=gTimer.getActualTime();__aux!=1;__aux=1,\
	dt=gTimer.getActualTime()-__t1)

#endif //___ITimer___

