

/********************************************************************
	created:	2014/04/11
	created:	11:4:2014   15:26
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\GUICommentItem.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	GUICommentItem
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __GUICommentItem__
#define __GUICommentItem__


#include "IGUIElement.h"
#include "GUIElementFactoryDef.h"

namespace mray
{
	namespace kmd
	{
		class KMDComment;
	}
namespace GUI
{

class GUICommentItem:public IGUIElement
{
	DECLARE_RTTI;
public: 
	static const GUID ElementType;
protected:
	kmd::KMDComment* m_Comment;
	GUI::FontAttributes m_fontAttrs;
public:
	GUICommentItem(IGUIManager* m);
	virtual~GUICommentItem();

	void SetComment(kmd::KMDComment* t);
	kmd::KMDComment* GetComment(){ return m_Comment; }


	virtual void Update(float dt);
	virtual void Draw(const math::rectf*vp);
};

DECLARE_ELEMENT_FACTORY(GUICommentItem);

}
}


#endif
