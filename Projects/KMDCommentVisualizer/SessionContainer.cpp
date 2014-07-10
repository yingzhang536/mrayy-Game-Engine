

#include "stdafx.h"
#include "SessionContainer.h"
#include "SessionDetails.h"
#include "XMLTree.h"

namespace mray
{
namespace kmd
{

SessionContainer::SessionContainer()
{
	m_currentSession = 0;
}

SessionContainer::~SessionContainer()
{
	Clear();
}


SessionDetails* SessionContainer::GetCurrentSession()
{
	if (m_currentSession >= m_sessions.size())
		return 0;
	return m_sessions[m_currentSession];
}


void SessionContainer::SetCurrentSession(int s)
{
	if (s < 0 || s >= m_sessions.size())
		return;
	m_currentSession = s;

	OnSessionChanged(this, m_sessions[m_currentSession]);
}

void SessionContainer::Clear()
{
	for (int i = 0; i < m_sessions.size(); ++i)
		delete m_sessions[i];
	m_currentSession = 0;
	OnSessionChanged(this, 0);
}

void SessionContainer::LoadFromXML(const core::string& path)
{
	xml::XMLTree t;
	if (!t.load(path))
		return;

	Clear();

	xml::XMLElement* e = t.getSubElement("Sessions");
	if (!e)
		return;



	//first load professors
	xml::XMLElement* elem = e->getSubElement("Professors");
	if (elem)
	{
		elem = elem->getSubElement("Professor");
		while (elem)
		{
			CProfessor::LoadProfessor(elem);
			elem = elem->nextSiblingElement("Professor");
		}
	}

	e=e->getSubElement("Session");
	while (e)
	{
		SessionDetails* s = new SessionDetails();
		s->LoadFromXML(e);
		m_sessions.push_back(s);
		SessionDetails::AddSession(s);
		e = e->nextSiblingElement("Session");
	}
	SetCurrentSession(0);
}

}
}
