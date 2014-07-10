

#ifndef LightComponent_h__
#define LightComponent_h__


#include "IMovableComponent.h"
#include "ISceneObjectsDesc.h"
//#include "IMovable.h"

namespace mray
{
	namespace scene
	{
		class LightNode;
		class ISceneManager;
	}
	
namespace game
{

class MRAY_GAME_DLL LightComponent :public IMovableComponent
{
	DECLARE_RTTI;
private:
protected:
	scene::ISceneManager* m_sceneMngr;
	scene::LightNode* m_node;
	bool m_removeNodeOnDestroy;
	scene::LightNodeDesc m_nodeDesc;

public:
public:
	LightComponent(GameEntityManager*);
	virtual~LightComponent();

	virtual bool InitComponent();

	bool AddComponent(IObjectComponent* comp);
	void RemoveComponent(IObjectComponent* comp);

	void SetRemoveNodeOnDestroy(bool r){ m_removeNodeOnDestroy = r; }
	bool IsRemoveNodeOnDestroy()const{ return m_removeNodeOnDestroy; }

	virtual bool IsUnique()const{ return false; }

	scene::LightNode* GetLightNode(){ return m_node; }
	void SetLightNode(scene::LightNode* node);

	virtual IObject* GetAttachedObject();
	virtual scene::IMovable* GetMovableObject();

	const scene::LightNodeDesc& GetNodeDesc(){ return m_nodeDesc; }

	virtual xml::XMLElement*  loadXMLSettings(xml::XMLElement* elem);
	virtual xml::XMLElement*  exportXMLSettings(xml::XMLElement* elem);
};
DECLARE_GAMECOMPONENT_FACTORY(LightComponent);


}
}
#endif // LightComponent_h__
