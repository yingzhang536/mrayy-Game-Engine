

/********************************************************************
	created:	2013/06/05
	created:	5:6:2013   16:45
	filename: 	C:\Development\mrayEngine\Engine\mrayGameLayer\IPhysicsComponent.h
	file path:	C:\Development\mrayEngine\Engine\mrayGameLayer
	file base:	IPhysicsComponent
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IPhysicsComponent__
#define __IPhysicsComponent__

#include "IGameComponent.h"

namespace mray
{
namespace scene
{
	class IMovable;
}
namespace game
{

class IPhysicsComponent;

#define SetGetFunction(dtype,Name,VarName,PostSet)\
	inline bool Set##Name(dtype v){VarName=v;PostSet;return true;}\
	inline dtype Get##Name()const{return VarName;}
	class SceneComponent;
class MRAY_GAME_DLL PhysicalComponentAttachment
{
protected:
	core::string m_targetName;
	core::string m_targetBone;

	SceneComponent* m_targetNode;
	scene::IMovable* m_targetObject;

	bool m_inheritPosition;
	bool m_inheritRotation;
public:
	SetGetFunction(core::string,TargetName,m_targetName,);
	SetGetFunction(core::string,TargetBone,m_targetBone,);
	SetGetFunction(bool,InheritPosition,m_inheritPosition,);
	SetGetFunction(bool,InheritRotation,m_inheritRotation,);

	PhysicalComponentAttachment();

	bool Init(IPhysicsComponent*o);

	void SetTargetNode(SceneComponent* c){m_targetNode=c;}
	void SetTargetObject(scene::IMovable* m){m_targetObject=m;}

	SceneComponent* GetTargetNode(){return m_targetNode;}
	scene::IMovable* GetTargetObject(){return m_targetObject;}

	void Update(const math::vector3d& pos,const math::quaternion& ori);

	void LoadFromXML(xml::XMLElement*e);
	void SaveToXML(xml::XMLElement*e);
};

class MRAY_GAME_DLL IPhysicsComponent:public IGameComponent
{
	DECLARE_RTTI;
protected:
	std::list<PhysicalComponentAttachment*> m_attachments;
public:
	IPhysicsComponent(){}
	virtual~IPhysicsComponent(){}

	virtual bool InitComponent();

	virtual void AttachNode(PhysicalComponentAttachment*a)
	{
		m_attachments.push_back(a);
	}

	virtual bool SetPosition(const math::vector3d& v)=0;
	virtual const math::vector3d& GetPosition()=0;

	virtual bool SetRotation(const math::vector3d& v)=0;
	virtual const math::vector3d& GetRotation()=0;

	const std::list<PhysicalComponentAttachment*> & GetAttachedNodes(){return m_attachments;}

	virtual xml::XMLElement*  loadXMLSettings(xml::XMLElement* elem);
	virtual xml::XMLElement*  exportXMLSettings(xml::XMLElement* elem);
};

}
}


#endif
