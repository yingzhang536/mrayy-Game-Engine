
#include "stdafx.h"
#include "NodeRenderer.h"
#include "TweetNode.h"
#include "SpeakerNode.h"
#include "CSpeaker.h"
#include "ConnectionLineRenderer.h"

#include "ShaderResourceManager.h"
#include "SessionRenderer.h"

namespace mray
{
namespace scene
{

	const core::string NodeRenderer::SpeakerNodeShader =
		"float4 main_fp(float2 texCoord : TEXCOORD0, "
		"uniform sampler2D texA : register(s0)) : COLOR "
		"{"
		"vec4 clr=tex2D(texA,texCoord);"
		"float2 v=(texCoord-0.5);"
		"float dst=dot(v,v);"
		"dst=dst>0.25?0:1;"
		"return float4(clr.rgb,dst);"
		"}";
	const core::string NodeRenderer::TweetNodeShader =
		"half4 main_fp(float2 texCoord : TEXCOORD0, "
		"uniform sampler2D texA : register(s0),uniform float Gray,uniform float Alpha) : COLOR "
		"{"
		"half4 clr=tex2D(texA,texCoord);"
		"half2 v=(texCoord-0.5);"
		"half3 gclr=dot(clr.rgb,half3(0.5,0.3,0.2));"
		"clr.rgb=gclr*Gray+clr.rgb*(1-Gray);"
		"half dst=dot(v,v);"
		"dst=dst>0.25?0:Alpha;"
		"return half4(clr.rgb,dst);"
		"}";
	const core::string NodeRenderer::ConnectionLineShader =
		"float4 main_fp() : COLOR "
		"{"
		"return float4(1,1,1,1);"
		"}";
NodeRenderer::NodeRenderer()
{
	m_connRenderer = new ConnectionLineRenderer();
	m_speakerNodeShader = gShaderResourceManager.loadShader(SpeakerNodeShader, video::EShader_FragmentProgram, "main_fp", mT("cg"), false);
	gShaderResourceManager.addResource(m_speakerNodeShader, "SpeakerNodeShader");

	m_tweetNodeShader = gShaderResourceManager.loadShader(TweetNodeShader, video::EShader_FragmentProgram, "main_fp", mT("cg"), false);
	gShaderResourceManager.addResource(m_speakerNodeShader, "TweetNodeShader");

	m_connectionShader = gShaderResourceManager.loadShader(ConnectionLineShader, video::EShader_FragmentProgram, "main_fp", mT("cg"), false);
	gShaderResourceManager.addResource(m_speakerNodeShader, "ConnectionLineShader");
}

NodeRenderer::~NodeRenderer()
{
	delete m_connRenderer;
}


void NodeRenderer::Clear()
{
	m_speakerConn.clear();
	m_tweetsConn.clear();
	m_tweets.clear();
	m_speakers.clear();
	m_connRenderer->Reset();
}
void NodeRenderer::_renderConnections(SessionRenderer *r)
{
	m_connRenderer->update(0);
	Engine::getInstance().getDevice()->useTexture(0, 0);
	Engine::getInstance().getDevice()->unuseShader();
	Engine::getInstance().getDevice()->setFragmentShader(m_connectionShader);
	Engine::getInstance().getDevice()->setLineWidth(2);
	if(false)
	{
		Engine::getInstance().getDevice()->drawSingleMesh(m_connRenderer->getMesh()->getBuffer(0));
		return;
	}
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	math::vector2d points[2];
	for (int i = 0; i < m_tweetsConn.size(); ++i)
	{
		points[0]=(m_tweetsConn[i].a->GetPosition());
		points[1] = (m_tweetsConn[i].b->GetPosition());
		dev->draw2DLine(points, 2, video::SColor(1, 1, 1, 1));
	}
	Engine::getInstance().getDevice()->setLineWidth(5);
	for (int i = 0; i < m_speakerConn.size(); ++i)
	{
		points[0] = (m_speakerConn[i].a->GetPosition());
		points[1] = (m_speakerConn[i].b->GetPosition());
		dev->draw2DLine(points, 2, video::SColor(1, 0, 0, 0.5));
	}

}

void NodeRenderer::_renderSpeakers(SessionRenderer *r)
{
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	dev->setFragmentShader(0);
	video::TextureUnit tex;
	math::vector2d pos;
	tex.SetTexture(gTextureResourceManager.loadTexture2D("GlowBG.png"));
	dev->useTexture(0, &tex);

	for (int i = 0; i < m_speakers.size(); ++i)
	{
		scene::SpeakerNode* speaker = m_speakers[i].node;

		pos = speaker->GetPosition();

		float scale = speaker->GetSize()*m_speakers[i].glow;
		math::rectf rc(pos, pos);
		rc.ULPoint -= scale*0.8;
		rc.BRPoint += scale*0.8;


		dev->draw2DImage(rc, video::SColor(1,1,1,m_speakers[i].glow));
	}
	dev->setFragmentShader(m_speakerNodeShader);

	for (int i = 0; i < m_speakers.size(); ++i)
	{
		scene::SpeakerNode* speaker = m_speakers[i].node;


		tex.SetTexture(speaker->GetSpeaker()->GetTexture());
		dev->useTexture(0, &tex);
		dev->draw2DImage(speaker->GetBoundingBox(false), 1);
	}
}

void NodeRenderer::_renderTweets(SessionRenderer *r)
{
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	dev->setFragmentShader(m_tweetNodeShader);
	math::vector2d pos;

	for (int i = 0; i < m_tweets.size(); ++i)
	{
		scene::TweetNode* tweet = m_tweets[i].node;

		pos = tweet->GetPosition();

		float g = 1-tweet->GetHoverValue();
		float a = tweet->GetAlpha();
		m_tweetNodeShader->setConstant("Gray", &g, 1);
		m_tweetNodeShader->setConstant("Alpha", &a, 1);

		float scale = tweet->GetSize()*m_tweets[i].scale;
		math::rectf rc(tweet->GetPosition(), tweet->GetPosition());
		rc.ULPoint -= scale*0.5;
		rc.BRPoint += scale*0.5;

		dev->useTexture(0, &tweet->GetImage());
		dev->draw2DImage(rc, 1);
	}

}

void NodeRenderer::RenderAll(SessionRenderer *r)
{
	_renderConnections(r);
	_renderSpeakers(r);
	_renderTweets(r);
	Engine::getInstance().getDevice()->unuseShader();
}


void NodeRenderer::AddSpeakerSpeaker(SpeakerNode*a, SpeakerNode* b)
{/*
	math::rectf combRC = a->GetBoundingBox(false);
	combRC.addPoint(b->GetBoundingBox(false).ULPoint);
	combRC.addPoint(b->GetBoundingBox(false).BRPoint);
	if (!m_clipRect.IsRectCollide(combRC))
		return;*/
	SpeakerConnectionInfo c;
	c.a = a;
	c.b = b;
	m_speakerConn.push_back(c);
	math::vector2d pa = a->GetPosition();
	math::vector2d pb = b->GetPosition();
	m_connRenderer->AddConnection(math::vector3d(pa.x, pa.y, 0), math::vector3d(pb.x, pb.y, 0), video::SColor(1, 0, 0, 0.5), video::SColor(1, 0, 0, 1));
}

void NodeRenderer::AddSpeakerTweetNode(SpeakerNode*a, TweetNode* b)
{
	if (!m_clipRect.IsRectCollide(a->GetBoundingBox(false))
		&& !m_clipRect.IsRectCollide(b->GetBoundingBox(false)))
		return;
	NodeConnectionInfo c;
	c.a = a;
	c.b = b;
	m_tweetsConn.push_back(c);

	math::vector2d pa = a->GetPosition();
	math::vector2d pb = b->GetPosition();
	m_connRenderer->AddConnection(math::vector3d(pa.x, pa.y, 0), math::vector3d(pb.x, pb.y, 0), video::SColor(1, 0, 0, 1), video::SColor(1, 1, 1, 1));
}

void NodeRenderer::AddTweetTweet(TweetNode*a, TweetNode* b)
{
	if (!m_clipRect.IsRectCollide(a->GetBoundingBox(false))
		&& !m_clipRect.IsRectCollide(b->GetBoundingBox(false)))
		return;
	NodeConnectionInfo c;
	c.a = a;
	c.b = b;
	m_tweetsConn.push_back(c);

	math::vector2d pa = a->GetPosition();
	math::vector2d pb = b->GetPosition();
	m_connRenderer->AddConnection(math::vector3d(pa.x, pa.y, 0), math::vector3d(pb.x, pb.y, 0), video::SColor(1, 1, 1, 1), video::SColor(1, 1, 1, 1));
}


void NodeRenderer::AddTweet(TweetNode* node,float scale)
{
	TweetNodeInfo ifo;
	ifo.node = node;
	ifo.scale= scale;
	m_tweets.push_back(ifo);
}

void NodeRenderer::AddSpeaker(SpeakerNode* node,float glow)
{
	SpeakerNodeInfo ifo;
	ifo.node = node;
	ifo.glow = glow;
	m_speakers.push_back(ifo);

}

}
}



