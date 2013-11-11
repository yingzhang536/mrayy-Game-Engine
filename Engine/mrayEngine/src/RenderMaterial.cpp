#include "stdafx.h"


#include "RenderMaterial.h"
#include "IVideoDevice.h"
#include "RenderTechnique.h"

namespace mray{
namespace video{


RenderMaterial::RenderMaterial()
	
{
	//matRenderType=MRT_SOLID;

	m_activeTechnique=0;
	CPropertieDictionary *dic=0;
	if(CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeName::instance);
	}
}
uint RenderMaterial::calcSizeInternal(){
	return sizeof(RenderMaterial);
}
RenderMaterial::~RenderMaterial(){
	ClearTechniques();
}


bool RenderMaterial::SetName(const core::string&name)
{
	setResourceName(name);
	return true;
}
core::string RenderMaterial::GetName()const
{
	return getResourceName();
}

bool RenderMaterial::SetActiveTechniqueByIndex(int idx)
{
	RenderTechnique* t=GetTechniqueAt(idx);
	if(!t)
		return false;
	m_activeTechnique=t;
	return true;
}
bool RenderMaterial::SetActiveTechniqueByName(const core::string& name)
{
	RenderTechnique* t=GetTechniqueByName(name);
	if(!t)
		return false;
	m_activeTechnique=t;
	return true;
}
RenderTechnique* RenderMaterial::GetActiveTechnique()
{
	return m_activeTechnique;
}

RenderTechnique* RenderMaterial::CreateTechnique(const core::string& name)
{
	RenderTechnique* t=new RenderTechnique(this);
	t->SetName(name);
	AddTechnique(t);
	return t;
}
const RenderTechniqueList& RenderMaterial::GetTechniques()const
{
	return m_techniques;
}
RenderTechnique* RenderMaterial::GetTechniqueAt(int i)
{
	if(i>=m_techniques.size())
		return 0;
	return m_techniques[i];
}
RenderTechnique* RenderMaterial::GetTechniqueByName(const core::string& name)
{
	for(int i=0;i<m_techniques.size();++i)
	{
		if(name.equals_ignore_case(m_techniques[i]->GetName()))
			return m_techniques[i];
	}
	return 0;
}
void RenderMaterial::AddTechnique(RenderTechnique*p)
{
	if(!p)
		return;
	m_techniques.push_back(p);
	p->SetOwnerMaterial(this);
	if(!m_activeTechnique)
		m_activeTechnique=p;
}
void RenderMaterial::RemoveTechnique(RenderTechnique*p)
{
	for(RenderTechniqueList::iterator it=m_techniques.begin();it!=m_techniques.end();++it)
	{
		if(*it==p)
		{
			m_techniques.erase(it);
			break;
		}
	}
}
void RenderMaterial::RemoveTechniqueAt(int index)
{
	if(index>=m_techniques.size())
		return;
	RenderTechniqueList::iterator it=m_techniques.begin();
	it+=index;
	delete *it;
	m_techniques.erase(it);
}
void RenderMaterial::ClearTechniques()
{
	for(int i=0;i<m_techniques.size();++i)
	{
		delete m_techniques[i];
	}
	m_techniques.clear();
}

RenderMaterial* RenderMaterial::Duplicate(){
	RenderMaterial* m=new RenderMaterial();
	m->setResourceName(getResourceName());

	for(int i=0;i<m_techniques.size();++i)
	{
		RenderTechnique* t;
		m->AddTechnique( t=m_techniques[i]->Duplicate());
		t->SetOwnerMaterial(this);
		if(m_techniques[i]==m_activeTechnique)
		{
			m->m_activeTechnique=t;
		}
	}
	return m;
}

xml::XMLElement* RenderMaterial::loadXMLSettings(xml::XMLElement* elem){

	IResource::loadFromXML(elem);

	xml::XMLElement*e=elem->getSubElement(mT("Technique"));
	while(e)
	{
		core::string name=e->getValueString("Name");
		video::RenderTechnique* t= GetTechniqueByName(name);
		bool loaded=false;
		if(!t)
			t=new RenderTechnique(this);
		else loaded=true;
		t->loadXMLSettings(e);
		if(!loaded)
			AddTechnique(t);
		e=e->nextSiblingElement(mT("Technique"));
	}
	m_activeTechnique=0;
	for(int i=0;i<m_techniques.size();++i)
	{
		if(m_techniques[i]->IsSupported())
		{
			m_activeTechnique=m_techniques[i];
			break;
		}
	}
	return elem;
}

xml::XMLElement*  RenderMaterial::exportXMLSettings(xml::XMLElement* elem){

	xml::XMLElement* node=0;

	node=new xml::XMLElement(mT("Material"));
	IResource::exportToXML(node);

	elem->addSubElement(node);

	for(int i=0;i<m_techniques.size();++i)
	{
		m_techniques[i]->exportXMLSettings(node);
	}
	return node;
	
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Name,RenderMaterial,string,mT("Name"),EPBT_Basic,mT("Material Name"),mT(""));
IMPLEMENT_PROPERTY_TYPE_GENERIC(Name,RenderMaterial,core::string,SetName,GetName,,,false)



}
}


