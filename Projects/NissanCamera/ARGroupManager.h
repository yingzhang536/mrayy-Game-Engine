

#ifndef ARGroupManager_h__
#define ARGroupManager_h__

#include "ARDataTypes.h"

namespace mray
{
namespace NCam
{
	class ARSceneGroup;
	class ARSceneObject
	{
	public:
		ARSceneObject() :id(0), alpha(1), group(0), obj(0), sceneNode(0)
		{}
		uint id;
		float alpha;
		ARSceneGroup* group;
		IARObject* obj;
		scene::ISceneNode* sceneNode;
	};
	class ARSceneGroup
	{
	public:
		~ARSceneGroup()
		{
			std::map<uint, ARSceneObject*>::iterator it = objects.begin();
			for (; it != objects.end();++it)
			{
				delete it->second;
			}
			objects.clear();
			delete group;
		}
		ARGroup* group;
		std::map<uint,ARSceneObject*> objects;
	};
	typedef std::map<uint, ARSceneGroup*> GroupMap;

class ARGroupManager
{
protected:
	GroupMap m_groups;
	scene::IRenderable* GenerateMeshObject(ARMesh* mesh);
	scene::IRenderable* LoadMeshObject(ARPredef* mesh);
	scene::IRenderable* LoadStringObject(ARString* mesh);
	scene::IRenderable* LoadVehicleObject(ARVehicle* mesh);
 

	void _UpdateGroup(ARSceneGroup* grp, ARGroup* src);
	scene::ISceneManager* m_sceneManager;
	scene::ISceneNode* m_arRoot;

	scene::ISceneNode* m_vehicle;
public:
	ARGroupManager();
	virtual ~ARGroupManager();

	void SetSceneManager(scene::ISceneManager* mgr, scene::ISceneNode* arRoot){ m_sceneManager = mgr; m_arRoot = arRoot; }

	scene::ISceneNode* GetVehicle(){ return m_vehicle; }

	ARSceneGroup* AddGroup(ARGroup* group);
	ARSceneGroup* GetGroupListByID(uint id);
	bool RemoveGroup(uint id);
	bool HideGroup(uint id);
	bool ShowGroup(uint id);
	void ClearGroups();

	bool SetAlpha(uint id, float alpha);
	bool SetAlpha( float alpha);

};

}
}


#endif // ARGroupManager_h__
