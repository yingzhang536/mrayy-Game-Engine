


#ifndef PhManager_h__
#define PhManager_h__



namespace mray
{
namespace physics
{
	class PhNode;
	class PhSpring;

class PhManager
{
protected:
	std::list<PhNode*> m_nodes;
	std::list<PhSpring*> m_springs;

	void _CheckCollision(PhNode*a, PhNode*b);

public:
	PhManager();
	virtual ~PhManager();


	PhNode* CreateNode();
	PhSpring* CreateSpring(PhNode* a, PhNode* b, float s,float l);

	void Clear();

	void Update(float dt);

	
};

}
}

#endif // PhManager_h__
