

#ifndef ProjectNode_h__
#define ProjectNode_h__

#include "DataTypes.h"
#include "IKMDNode.h"

namespace mray
{
	namespace kmd
	{
		class SessionDetails;
	}
namespace scene
{
	class CommentNode;

	class ProjectNode :public IKMDNode
{
protected:
	kmd::SessionDetails* m_project;
	std::vector<CommentNode*> m_subComments;
	float m_glowFactor;
public:
	ProjectNode(kmd::SessionDetails* s);
	virtual~ProjectNode();

	kmd::SessionDetails* GetProject(){
		return m_project;
	}

	core::string GetProjectName();

	void AddComment(CommentNode* t);
	const std::vector<CommentNode*>& GetSubComments(){ return m_subComments; }
	virtual void Update(float dt);


	void Draw(NodeRenderer *r, const math::rectf& rc);
	void DrawSubComments();
	virtual IKMDNode* GetNodeFromPoint(const math::vector2d& pos);
	virtual math::rectf GetBoundingBox(bool includeChildren);
};

}
}

#endif // ProjectNode_h__
