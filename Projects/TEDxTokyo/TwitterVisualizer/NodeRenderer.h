



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
	class SpeakerNode;
	class TweetNode;
	class ITedNode;
	class ConnectionLineRenderer;
	class SessionRenderer;
class NodeRenderer
{
protected:
	static const core::string SpeakerNodeShader;
	static const core::string TweetNodeShader;
	static const core::string ConnectionLineShader;

	struct SpeakerConnectionInfo
	{
		SpeakerNode *a;
		SpeakerNode *b;
	};

	struct NodeConnectionInfo
	{
		ITedNode *a;
		TweetNode *b;
	};
	struct SpeakerNodeInfo
	{
		SpeakerNode* node;
		float glow;
	};
	struct TweetNodeInfo
	{
		TweetNode* node;
		float scale;
	};
	ConnectionLineRenderer *m_connRenderer;
	std::vector<SpeakerConnectionInfo> m_speakerConn;
	std::vector<NodeConnectionInfo> m_TweetsConn;

	std::vector<SpeakerNodeInfo> m_speakers;
	std::vector<TweetNodeInfo> m_Tweets;
	math::rectf m_clipRect;

	void _renderConnections(SessionRenderer *r);
	void _renderSpeakers(SessionRenderer *r);
	void _renderTweets(SessionRenderer *r);

	video::IGPUShaderProgramPtr m_speakerNodeShader;
	video::IGPUShaderProgramPtr m_TweetNodeShader;
	video::IGPUShaderProgramPtr m_connectionShader;
public:
	NodeRenderer();
	virtual ~NodeRenderer();

	void Clear();
	void RenderAll(SessionRenderer* r);
	void SetClippingVP(const math::rectf& rc){ m_clipRect = rc; }

	void AddSpeakerSpeaker(SpeakerNode*a, SpeakerNode* b);
	void AddSpeakerTweetNode(SpeakerNode*a, TweetNode* b);
	void AddTweetTweet(TweetNode*a, TweetNode* b);

	void AddTweet(TweetNode* node,float scale);
	void AddSpeaker(SpeakerNode* node,float glow);
};

}
}

#endif
