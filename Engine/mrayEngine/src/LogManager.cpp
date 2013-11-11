#include "stdafx.h"

#include "logManager.h"
#include "ILogDevice.h"

namespace mray{


LogManager::LogManager():m_verboseLevel(EVL_Normal)
{
}

LogManager::~LogManager(){
	/*
	LogDeviceList::iterator it=m_loggers.begin();
	for(;it!=m_loggers.end();++it){
		(*it)=0;
	}*/
	m_loggers.clear();
	m_logHistory.clear();
}

void LogManager::setVerbosLevel(EVerbosLevel vl)
{
	m_verboseLevel=vl;
}
EVerbosLevel LogManager::GetVerbosLevel()
{
	return m_verboseLevel;
}

void LogManager::log(const core::string &msg,const core::string &info,ELogLevel level,EVerbosLevel vl){
// 	LogDeviceList::iterator it=m_loggers.begin();
// 	for(;it!=m_loggers.end();++it){
// 		(*it)->log(msg,info,level);
// 	}

	log(msg+info,level,vl);
}

void LogManager::log(const core::string &msg,ELogLevel level,EVerbosLevel vl){
	if(vl>m_verboseLevel)
		return;

	LogMsg tmp;
	tmp.level=level;
	tmp.msg=msg;
	m_logHistory.push_back(tmp);

	LogDeviceList::iterator it=m_loggers.begin();
	for(;it!=m_loggers.end();++it){
		(*it)->log(msg,level);
	}
	flush();
}


void LogManager::startSection(const core::string &name){
	LogMsg tmp;
	tmp.level=ELL_INFO;
	tmp.msg=mT(" ---------- ")+name+mT(" ---------- ");
	m_logHistory.push_back(tmp);
	LogDeviceList::iterator it=m_loggers.begin();
	for(;it!=m_loggers.end();++it){
		(*it)->startSection(name);
	}
}

void LogManager::endSection(bool Success){
	LogMsg tmp;
	tmp.level=Success ? ELL_SUCCESS:ELL_WARNING;
	tmp.msg=mT(" ---------- [Done] ---------- ");
	m_logHistory.push_back(tmp);
	LogDeviceList::iterator it=m_loggers.begin();
	for(;it!=m_loggers.end();++it){
		(*it)->endSection(Success);
	}
}


void LogManager::close(){
	LogDeviceList::iterator it=m_loggers.begin();
	for(;it!=m_loggers.end();++it){
		(*it)->close();
	}
}


void LogManager::addLogDevice(const ILogDevicePtr&logger){
	m_loggers.push_back(logger);

	for (int i=0;i<m_logHistory.size();++i)
	{
		logger->log(m_logHistory[i].msg,m_logHistory[i].level);
	}
}

void LogManager::removeLogDevice(const ILogDevicePtr&logger){
	LogDeviceList::iterator it=m_loggers.begin();
	for(;it!=m_loggers.end();++it){
		if((*it)==logger){
			m_loggers.erase(it);
			return;
		}
	}
}

void LogManager::flush(){
	LogDeviceList::iterator it=m_loggers.begin();
	for(;it!=m_loggers.end();++it){
		(*it)->flush();
	}

}

}