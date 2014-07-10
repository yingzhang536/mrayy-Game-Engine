



/********************************************************************
	created:	2014/05/11
	created:	11:5:2014   13:34
	filename: 	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\ConnectionRenderer.h
	file path:	F:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	ConnectionRenderer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __ConnectionRenderer__
#define __ConnectionRenderer__




namespace mray
{
namespace scene
{
	class ProjectNode;
	class CommentNode;
	class IKMDNode;
	class ConnectionLineRenderer;
	class SessionRenderer;


class NodeRenderer
{
protected:
	static const core::string ProjectNodeShader;
	static const core::string CommentNodeShader;
	static const core::string ConnectionLineShader;

	struct SpeakerConnectionInfo
	{
		ProjectNode *a;
		ProjectNode *b;
	};

	struct NodeConnectionInfo
	{
		IKMDNode *a;
		CommentNode *b;
	};
	struct ProjectNodeInfo
	{
		ProjectNode* node;
		float glow;
	};
	struct CommentNodeInfo
	{
		CommentNode* node;
		float scale;
	};
	ConnectionLineRenderer *m_connRenderer;
	std::vector<SpeakerConnectionInfo> m_speakerConn;
	std::vector<NodeConnectionInfo> m_CommentsConn;

	std::vector<ProjectNodeInfo> m_speakers;
	std::vector<CommentNodeInfo> m_Comments;
	math::rectf m_clipRect;

	void _renderConnections(SessionRenderer *r, float alpha);
	void _renderSpeakers(SessionRenderer *r, float alpha);
	void _renderComments(SessionRenderer *r, float alpha);

	video::IGPUShaderProgramPtr m_ProjectNodeShader;
	video::IGPUShaderProgramPtr m_CommentNodeShader;
	video::IGPUShaderProgramPtr m_connectionShader;
public:
	NodeRenderer();
	virtual ~NodeRenderer();

	void Clear();
	void RenderAll(SessionRenderer* r, float alpha);
	void SetClippingVP(const math::rectf& rc){ m_clipRect = rc; }

	void AddSpeakerSpeaker(ProjectNode*a, ProjectNode* b);
	void AddSpeakerCommentNode(ProjectNode*a, CommentNode* b);

	void AddComment(CommentNode* node,float scale);
	void AddSpeaker(ProjectNode* node,float glow);
};

}
}

#endif
