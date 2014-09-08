

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
		ARSceneGroup* group;
		IARObject* obj;
		scene::ISceneNode* sceneNode;
	};
	class ARSceneGroup
	{
	public:
		~ARSceneGroup()
		{
			for (int i = 0; i < objects.size();++i)
			{
				delete objects[i];
			}
			delete group;
		}
		ARGroup* group;
		std::vector<ARSceneObject*> objects;
	};
	typedef std::map<uint, ARSceneGroup*> GroupMap;

class ARGroupManager
{
protected:
	GroupMap m_groups;
	scene::IRenderable* GenerateMeshObject(ARMesh* mesh);
	scene::IRenderable* LoadMeshObject(ARPredef* mesh);
 
	scene::ISceneManager* m_sceneManager;
public:
	ARGroupManager();
	virtual ~ARGroupManager();

	void SetSceneManager(scene::ISceneManager* mgr){ m_sceneManager = mgr; }

	ARSceneGroup* AddGroup(ARGroup* group);
	ARSceneGroup* GetGroupListByID(uint id);
	void RemoveGroup(uint id);
	void HideGroup(uint id);
	void ShowGroup(uint id);
	void ClearGroups();

};

}
}


#endif // ARGroupManager_h__
