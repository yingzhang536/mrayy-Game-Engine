
#ifndef CommentNode_h__
#define CommentNode_h__

#include "DataTypes.h"
#include "IKMDNode.h"

namespace mray
{
	namespace kmd
	{
		class KMDComment;
		class SessionDetails;
	}

namespace scene
{

class CommentNode:public IKMDNode
{
protected:
	kmd::SessionDetails* m_targetProject;
	kmd::KMDComment* m_comment;
	video::TextureUnit m_texture;

	float m_sizeScaler;
	float m_hoverValue;
public:
	CommentNode(kmd::SessionDetails* s, kmd::KMDComment* t);
	virtual~CommentNode();

	kmd::IDType GetCommentID();

	kmd::SessionDetails* GetProject(){ return m_targetProject; }
	kmd::KMDComment* GetComment(){ return m_comment; }

	video::TextureUnit& GetImage(){ return m_texture; }

	float GetHoverValue(){ return m_hoverValue; }

	virtual void Update(float dt);

	void Draw(NodeRenderer *r, const math::rectf& rc);
	virtual IKMDNode* GetNodeFromPoint(const math::vector2d& pos);
	virtual math::rectf GetBoundingBox(bool includeChildren);
	virtual void OnHoverOn(){ m_hovered = true; }
	virtual void OnHoverOff(){ m_hovered = false; }
};

}
}

#endif // CommentNode_h__
