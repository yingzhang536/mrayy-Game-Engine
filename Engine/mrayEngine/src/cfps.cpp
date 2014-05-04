#include "stdafx.h"


#include "CFPS.h"

#include "ITimer.h"


namespace mray{

namespace core{

CFPS::CFPS():m_lastFps(60),m_fps(99999),
	m_startTime(0),m_frames(60),m_frameTime(0),m_lastTime(0),m_worstFPS(999999),m_bestFPS(0),m_allFrames(0),
	m_allFps(0),m_timeFactor(1),m_isFixedFPS(0)
{
	m_lastTime=gTimer.getActualTime();
}

void CFPS::setFixedFPS(float fps,bool set){
	m_isFixedFPS=set;
	if(m_isFixedFPS)
		m_fps=fps;
}

void CFPS::setTimeFactor(float t){
	m_timeFactor=t;
	if(m_timeFactor<0)
		m_timeFactor=0;
}

float CFPS::getFPS()
{
	return m_fps;
}

float CFPS::dt(){
	return m_frameTime*m_timeFactor;
}

void CFPS::resetTime(int t){
	m_startTime=m_lastTime=t;
	m_frames=0;
	m_frameTime=0;
} 
void CFPS::regFrame(float time)
{

	if(m_isFixedFPS){
		m_frameTime=m_fps*0.001f;
		m_allFps+=m_frameTime;
		return;
	}
	m_allFps++;
	m_frames++;
	float dtime = time - m_startTime;
	/*
	if(time>m_lastTime)
		m_frameTime=(time-m_lastTime)*0.001f;
	else
		m_frameTime=0;*/
	m_allFrames+=m_frameTime;

	m_lastTime=time;
//	if(dtime>1000)
	if(dtime>1000)//update fps each 1/8 sec (125 ms)
	{ 
		float f=(float)(m_frames*1000)/((float)dtime);
		m_fps=math::lerp<float>(m_lastFps,f,0.8);
		m_frameTime = 1.0f / m_fps;
		if(m_fps>m_bestFPS)
			m_bestFPS=m_fps;
		if(m_fps<m_worstFPS)
			m_worstFPS=m_fps;
		m_lastFps=f;
		m_startTime=time;
		m_frames=0;
	}

}



float CFPS::getWorstFPS(){
	return m_worstFPS;
}
float CFPS::getBestFPS(){
	return m_bestFPS;;
}
float CFPS::getAverageFPS(){
	return (m_allFps)/(float)m_allFrames;
}






}
}
