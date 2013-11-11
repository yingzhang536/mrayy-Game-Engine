

/********************************************************************
	created:	2010/09/02
	created:	2:9:2010   17:53
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\FontDecorateFont.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	FontDecorateFont
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef FontDecorateFont_h__
#define FontDecorateFont_h__

#include "ITextDecorateNode.h"

namespace mray
{
namespace GUI
{
class FontDecorateFont:public ITextDecorateNode
{
private:
protected:
	IFont* m_Font;
	IFont* m_SavedFont;
public:
	FontDecorateFont();
	virtual ~FontDecorateFont();

	virtual void ParseParameter(const core::string &p);

	virtual void OnEnter(TextContextAttributes*context);
	virtual void OnExit(TextContextAttributes*context);
};

class FontDecorateFontCreator:public ITextDecorateCreator
{
public:
	FontDecorateFontCreator():ITextDecorateCreator(mT("Font"))
	{
	}
	virtual ITextDecorateNode* CreateNode()
	{
		return new FontDecorateFont();
	}

};


}
}

#endif // FontDecorateFont_h__
