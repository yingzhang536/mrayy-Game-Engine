

#include "stdafx.h"
#include "GUITweetItem.h"
#include "TweetDB.h"

#include "IGUIManager.h"
#include "GUIElementRegion.h"
#include "TextureResourceManager.h"
#include "FontResourceManager.h"
#include "IGUIRenderer.h"

namespace mray
{
namespace GUI
{

	const core::string GUITweetItem::ElementType("TweetItem");

	GUITweetItem::GUITweetItem(IGUIManager* m) :IGUIElement(ElementType, m)
	{
		m_tweet = 0;
	}
	GUITweetItem::~GUITweetItem()
	{

	}

	void GUITweetItem::SetTweet(ted::TweetDB* t)
	{
		m_tweet = t;
	}


	void GUITweetItem::Update(float dt)
	{
		IGUIElement::Update(dt);
	}
	void GUITweetItem::Draw(const math::rectf*vp)
	{
		if (!IsVisible())
			return;
		video::IVideoDevice *dev = Engine::getInstance().getDevice();

		GUI::IGUIManager* creator = GetCreator();

		const math::rectf& rect = GetDefaultRegion()->GetRect();
		const math::rectf& clip = GetDefaultRegion()->GetClippedRect();

		math::rectf bgR(rect);

		video::TextureUnit tex1;

		IFont* font = gFontResourceManager.getFontByName("Default");
		if (!font)
			font = gFontResourceManager.getDefaultFont();

		math::rectf oldScissor = GetCreator()->GetDevice()->getScissorRect();
		dev->setScissorRect(clip);
		tex1.SetTexture(gTextureResourceManager.loadTexture2D("TweetBG.png"));
		dev->useTexture(0, &tex1);
		dev->draw2DImage(bgR, 1);

		if (font && m_tweet)
		{
			math::rectf tmpRc;

			tex1.SetTexture(gTextureResourceManager.loadTexture2D("twitter.png"));
			tmpRc.ULPoint= bgR.ULPoint+20;
			tmpRc.BRPoint = tmpRc.ULPoint + 30;
			dev->useTexture(0, &tex1);
			dev->draw2DImage(tmpRc, 1);
			tmpRc.ULPoint = tmpRc.BRPoint;
			tmpRc.BRPoint = bgR.BRPoint - 60;

			m_fontAttrs.fontAligment = GUI::EFA_TopLeft;
			m_fontAttrs.fontColor.Set(1, 1, 1, 1);
			m_fontAttrs.hasShadow = false;
			m_fontAttrs.spacing = 0;
			m_fontAttrs.lineSpacing = 10;
			m_fontAttrs.wrap = true;
			m_fontAttrs.fontSize = 18;
			font->print(tmpRc, &m_fontAttrs, &clip, m_tweet->text, creator->GetRenderQueue());

		}
		GetCreator()->GetRenderQueue()->Flush();


		dev->setScissorRect(oldScissor);
		IGUIElement::Draw(vp);
	}

}
}
