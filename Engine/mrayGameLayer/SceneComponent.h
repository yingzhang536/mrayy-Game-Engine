


/********************************************************************
	created:	2012/02/12
	created:	12:2:2012   22:24
	filename: 	d:\Development\mrayEngine\Engine\mrayGameLayer\SceneComponent.h
	file path:	d:\Development\mrayEngine\Engine\mrayGameLayer
	file base:	SceneComponent
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __SceneComponent__
#define __SceneComponent__

#include "IMovableComponent.h"
#include "ISceneObjectsDesc.h"
//#include "IMovable.h"

namespace mray
{
namespace scene
{
	class ISceneNode;
	class ISceneManager;
}
namespace game
{

class MRAY_GAME_DLL SceneComponent:public IMovableComponent
{
	DECLARE_RTTI;
private:
protected:
	scene::ISceneManager* m_sceneMngr;
	scene::ISceneNode* m_node;
	bool m_removeNodeOnDestroy;
	scene::ISceneNodeDesc m_nodeDesc;

public:
public:
	SceneComponent(GameEntityManager*);
	SceneComponent();
	virtual~SceneComponent();

	virtual bool InitComponent();

	virtual bool SetEnabled(bool e);

	bool AddComponent(IObjectComponent* comp);
	void RemoveComponent(IObjectComponent* comp);

	void SetRemoveNodeOnDestroy(bool r){m_removeNodeOnDestroy=r;}
	bool IsRemoveNodeOnDestroy()const{return m_removeNodeOnDestroy;}

	virtual bool IsUnique()const{return false;}

	scene::ISceneNode* GetSceneNode(){return m_node;}
	void SetSceneNode(scene::ISceneNode* node);

	virtual IObject* GetAttachedObject();
	virtual scene::IMovable* GetMovableObject();

	const scene::ISceneNodeDesc& GetNodeDesc(){return m_nodeDesc;}

	virtual xml::XMLElement*  loadXMLSettings(xml::XMLElement* elem);
	virtual xml::XMLElement*  exportXMLSettings(xml::XMLElement* elem);
};
DECLARE_GAMECOMPONENT_FACTORY(SceneComponent);


}
}

#endif
