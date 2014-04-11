

/********************************************************************
	created:	2014/04/11
	created:	11:4:2014   15:26
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\GUITweetItem.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	GUITweetItem
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GUITweetItem__
#define __GUITweetItem__


#include "IGUIElement.h"

namespace mray
{
	namespace ted
	{
		class TweetDB;
	}
namespace GUI
{

class GUITweetItem:public IGUIElement
{
public: 
	static const core::string ElementType;
protected:
	ted::TweetDB* m_tweet;
	GUI::FontAttributes m_fontAttrs;
public:
	GUITweetItem(IGUIManager* m);
	virtual~GUITweetItem();

	void SetTweet(ted::TweetDB* t);
	ted::TweetDB* GetTweet(){ return m_tweet; }


	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};

}
}


#endif
