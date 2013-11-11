

/********************************************************************
	created:	2010/09/02
	created:	2:9:2010   17:36
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\TextDecorateNode.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	TextDecorateNode
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef TextDecorateNode_h__
#define TextDecorateNode_h__

#include "ITextDecorateNode.h"
#include "UTFString.h"

namespace mray
{
namespace GUI
{

class TextDecorateNode:public ITextDecorateNode
{
private:
protected:
	core::string m_text;
	core::UTFString m_renderText;
public:
	TextDecorateNode(const core::string&text);
	virtual ~TextDecorateNode();

	virtual void Draw(TextContextAttributes*context,IGUIRenderer*renderer,const math::rectf*clip);
	virtual void CalculateSize(TextContextAttributes*context);
};

}
}
#endif // TextDecorateNode_h__
