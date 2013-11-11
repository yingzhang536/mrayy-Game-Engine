

#ifndef ___CFPS___
#define ___CFPS___

#include "mTypes.h"
#include "CompileConfig.h"
#include "ISingleton.h"


namespace mray{
namespace core{

//! FPS(Fram per second)
class MRAY_DLL CFPS:public ISingleton<CFPS>
{
	float m_lastFps;
	float m_lastTime;
	float m_frameTime;
	float m_fps;
	float m_startTime;
	uint m_frames;
	float m_allFrames;
	float m_allFps;

	float m_worstFPS;
	float m_bestFPS;

	bool m_isFixedFPS;

	float m_timeFactor;
public:
	CFPS();
	float getFPS();
	float dt();

	void setTimeFactor(float t);

	void resetTime(int t);

	void setFixedFPS(float fps,bool set=true);

	//! register a new frame
	void regFrame(float time);

	float getWorstFPS();
	float getBestFPS();
	float getAverageFPS();
};

#define gFPS mray::core::CFPS::getInstance()

};//core
};//mray


#endif













