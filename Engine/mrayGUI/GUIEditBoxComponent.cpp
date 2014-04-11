
#include "stdafx.h"
#include "GUIEditBoxComponent.h"
#include "IGUIElement.h"
#include "IGUITheme.h"
#include "IGUIManager.h"
#include "KeyboardEvent.h"
#include "MouseEvent.h"
#include "IOSClipboard.h"
#include <Regex/Pattern.h>


namespace mray
{
namespace GUI
{

GUIEditBoxComponent::GUIEditBoxComponent():
	m_CursorPos(0),m_startPos(0),m_MouseMark(0),m_regexExpression(0),
	m_MarkEnd(0),m_MarkBegin(0),m_selectionColor(128,128,128,128),listener(0),
	acceptsNumbers(true),acceptsChars(true),m_currentTint(1)
{
}

GUIEditBoxComponent::~GUIEditBoxComponent()
{
	delete m_regexExpression;
}

bool GUIEditBoxComponent::SetText(const core::UTFString& cap)
{
	if(!CheckIsValideText(cap))
		return false;
	text=cap;
	//m_CursorPos=text.Length();
	if(listener)
		listener->OnTextChangeCallback(this);
	return true;
}
bool GUIEditBoxComponent::CheckIsValideText(const core::UTFString&t)
{
	if(m_regexExpression)
	{
		core::stringc expStr;
		core::string_to_char(t.GetAsString().c_str(),expStr);
		return m_regexExpression->matches(expStr.c_str());
	}
	return true;
}
core::string GUIEditBoxComponent::getSelectedText()
{
	if(m_MarkEnd==m_MarkBegin)
		return mT("");
	return text.substr(m_MarkBegin,m_MarkEnd-m_MarkBegin).GetAsString();
}
void GUIEditBoxComponent::_RemoveSelectedText()
{
	if(m_MarkEnd!=m_MarkBegin)
	{
		core::UTFString str=text;

		str.Erase(m_MarkBegin,m_MarkEnd-1);
		SetText(str);
		m_CursorPos=m_MarkBegin;
		m_MarkBegin=m_MarkEnd=0;
	}
}
void GUIEditBoxComponent::Update(float dt)
{
	if(owner->GetCreator()->HasFocus(owner))
	{
		if(m_currentTint<1)m_currentTint+=dt;
		else m_currentTint=1;
	}else
	{
		if(m_currentTint>0.7f)m_currentTint-=dt;
		else m_currentTint=0.7f;

	}
}

void GUIEditBoxComponent::Draw()
{

	IGUIManager* creator=owner->GetCreator();
	IGUITheme* skin=creator->GetActiveTheme();
	math::rectf clipRect= rc;
	clipRect.clipRect(owner->GetDefaultRegion()->GetClippedRect());

	FontAttributes* fontAttrs=owner->GetFontAttributes();

	GUI::IFont* font=gFontResourceManager.getFontByName(fontAttrs->fontName);

	int cPos=m_CursorPos-m_startPos;

	if(m_background && skin)
	{
		video::SColor tint = owner->GetColor()*m_currentTint;
		tint.A=owner->GetDerivedAlpha();
		skin->drawSizableBox(creator->GetRenderQueue(),clipRect,0,mT("EditBox"),tint);
	}

	math::rectf rc;
	if(skin)
		rc=skin->getSizableRect(4,clipRect,0,mT("EditBox"));
	else 
		rc=clipRect;


	core::UTFString str;
	if(m_HideText)
	{
		core::string t;
		for(int i=0;i<text.Length()-m_startPos;++i)
		{
			t+=m_HideChar;
		}
		str=t;
	}else
		str=text.substr(m_startPos,text.Length()-m_startPos);
	fontAttrs->fontColor.A=owner->GetDerivedAlpha();
	if(font)
		font->print(rc,fontAttrs,&clipRect,str,creator->GetRenderQueue());
	if(creator->HasFocus(owner) && enabled && active)
	{
		str=str.substr(0,max(0,m_CursorPos-m_startPos));
		//draw cursor
		math::vector2d p=font->getCharPos(cPos,fontAttrs,rc,str);
		if(font)
			font->print(math::rectf(p,p+300),fontAttrs,&clipRect,core::UTFString((utf8*)"_"),creator->GetRenderQueue());
	}

	if(creator->HasFocus(owner) && m_MarkBegin!=m_MarkEnd)
	{
		m_selectionColor.A=owner->GetDerivedAlpha();
		//draw selection
		math::vector2d startP=font->getCharPos(m_MarkBegin-m_startPos,fontAttrs,rc,text);
		math::vector2d endP=font->getCharPos(m_MarkEnd-m_startPos,fontAttrs,rc,text);

		math::vector2d cDim=font->getCharDimension('A',fontAttrs->fontSize);
		if(startP.y==endP.y)
		{
			math::rectf r(startP,math::vector2d(endP.x,endP.y+cDim.y));
			creator->GetDevice()->draw2DRectangle(r,m_selectionColor,1,&clipRect);
		}
		else
		{
			float y=startP.y;
			math::rectf r(startP,math::vector2d(endP.x,endP.y+cDim.y));
			creator->GetDevice()->draw2DRectangle(r,m_selectionColor,1,&clipRect);

			while(y<=endP.x)
			{
				creator->GetDevice()->draw2DRectangle(r,m_selectionColor,1,&clipRect);
				y+=cDim.y;
				if(y<endP.y)
				{
					r.ULPoint.x=rc.ULPoint.x;
					r.ULPoint.y=y;
					r.BRPoint.y=y+cDim.y;
				}else
				{
					r.ULPoint.x=rc.ULPoint.x;
					r.ULPoint.y=y;
					r.BRPoint.x=endP.x;
					r.BRPoint.y=y+cDim.y;
				}
			}
		}
	}

}


bool GUIEditBoxComponent::OnKeyboardEvent(KeyboardEvent* event)
{

	IGUIManager* creator=owner->GetCreator();
	math::rectf clipRect= rc;
	clipRect.clipRect(owner->GetDefaultRegion()->GetClippedRect());
	FontAttributes* fontAttrs=owner->GetFontAttributes();
	IFont*font=gFontResourceManager.getFontByName(fontAttrs->fontName);
	if(!font)
		return false;

	IGUITheme* skin=creator->GetActiveTheme(); 	

	if(skin)
		clipRect=skin->getSizableRect(4,clipRect,0,mT("EditBox"));
	

	GUI_EVENT_TYPE EventType=GET_NONE;
	bool ok=0;
	EKEY_CODE key=event->key;
	utf8 ch=event->Char;
	core::UTFString text=this->text;


	int cp=m_CursorPos;
	int mb=m_MarkBegin;
	int me=m_MarkEnd;
	bool mm=m_MouseMark;
	int sp=m_startPos;
	bool textChanged=false;

	if(event->press)
	{
		//OnKeyDown(this,event);
		switch(key)
		{
		case KEY_DELETE:
			if(m_MarkEnd!=m_MarkBegin)
			{
				_RemoveSelectedText();
				text=text;
				textChanged=true;
			}else{
				if(m_CursorPos<text.Length())
				{
					text.Erase(m_CursorPos,m_CursorPos);
					textChanged=true;
				}
			}
			break;
		case KEY_BACK:
			if(m_MarkEnd!=m_MarkBegin)
			{
				_RemoveSelectedText();
				text=text;
				textChanged=true;
			}else
				if(m_CursorPos>0)
				{
					--m_CursorPos;
					text.Erase(m_CursorPos,m_CursorPos);
					textChanged=true;
				}
				break;
		case KEY_LEFT:
			if(event->shift)
			{
				if(m_MarkBegin==m_MarkEnd)
					m_MarkEnd=m_CursorPos;
				m_MarkBegin=(m_CursorPos>0)? m_CursorPos-1:m_CursorPos;
			}
			else
			{
				m_MarkBegin=m_MarkEnd=0;
			}
			m_CursorPos=(m_CursorPos>0)? m_CursorPos-1:m_CursorPos;
			break;
		case KEY_RIGHT:
			if(event->shift)
			{
				if(m_MarkBegin==m_MarkEnd)
					m_MarkBegin=m_CursorPos;
				m_MarkEnd=(m_CursorPos<text.Length())? m_CursorPos+1:m_CursorPos;
			}
			else
			{
				m_MarkBegin=m_MarkEnd=0;
			}
			m_CursorPos=(m_CursorPos<text.Length())? m_CursorPos+1:m_CursorPos;
			break;
		case KEY_HOME:
			if(event->shift)
			{
				if(m_MarkBegin==m_MarkEnd)
					m_MarkEnd=m_CursorPos;
				m_MarkBegin=0;
			}
			else
			{
				m_MarkBegin=m_MarkEnd=0;
			}
			m_CursorPos=0;
			m_startPos=0;
			break;
		case KEY_END:
			{
				if(event->shift)
				{
					if(m_MarkBegin==m_MarkEnd)
						m_MarkBegin=m_CursorPos;
					m_MarkEnd=text.Length();
				}
				else
				{
					m_MarkBegin=m_MarkEnd=0;
				}
				m_CursorPos=text.Length();
			}
			break;
		default:

			if(event->ctrl)
			{
				switch(event->key)
				{
				case KEY_C://copy
					{
						core::string str;
						str=text.substr(m_MarkBegin,m_MarkEnd-m_MarkBegin).GetAsString();
						gOSClipboard.copyToClipboard(str.c_str());
						textChanged=true;
					}
					break;
				case KEY_X://cut
					{
						core::string str;
						str=text.substr(m_MarkBegin,m_MarkEnd-m_MarkBegin).GetAsString();
						_RemoveSelectedText();
						text.Erase(m_MarkBegin,m_MarkEnd);
						gOSClipboard.copyToClipboard(str.c_str());
						m_MarkBegin=m_MarkEnd=0;
						textChanged=true;
					}
					break;
				case KEY_V://paste
					{
						int a=m_MarkBegin,b=m_MarkEnd;
						if(m_MarkBegin==m_MarkEnd)
							a=b=m_CursorPos;
						core::string str=text.substr(0,a).GetAsString();
						str+=gOSClipboard.getClipboardText();
						str+=text.substr(b,text.Length()-b).GetAsString();
						text.Set(str);
						textChanged=true;
					}
					break;
				}
			}else if(ch!=0)
			{
				bool accept=true;
				if(!acceptsNumbers && isdigit(ch))
				{
					accept=false;
				}
				if(!acceptsChars && isalpha(ch))
				{
					accept=false;
				}
				if(accept)
				{
					if(m_CursorPos==text.Length()){
						text.Append(ch);
					}
					else{
						text.Insert(ch,m_CursorPos);
					}
					textChanged=true;
					m_CursorPos++;
					if(event->shift)
					{
						if(m_MarkBegin!=m_MarkEnd)
							m_MarkEnd++;
					}
				}
			}
		};
		if(m_startPos>m_CursorPos)
			m_startPos=m_CursorPos;
		else
		{
			float dim=0;
			int ci=0;
			float w=clipRect.getWidth();
			for(ci=text.Length()-1 ;ci>=0&& dim<w;ci--){
				dim+=font->getCharDimension(text[ci],fontAttrs->fontSize).x;
			}
			if(dim>clipRect.getWidth())
			{
				m_startPos=ci+1+2;
			}else
				m_startPos=0;
		}
		if(m_startPos>m_CursorPos)
			m_startPos=m_CursorPos;

		ok=true;

		if(event->press)
			EventType=GET_KeyDown;
		else EventType=GET_CHANGED;

		if(!SetText(text)  && textChanged)
		{
			m_CursorPos=cp;
			m_MarkBegin=mb;
			m_MarkEnd=me;
			m_MouseMark=mm;
			m_startPos=sp;
		}
	}

	if(ok)
	{
		event->setRecived();
		GCPtr<GUIEvent> event1=EventMemoryManager::getInstance().createEvent(ET_GUIEvent);
		event1->Caller=owner;
		event1->event=EventType;
		event1->key=key;
		creator->OnEvent(event1);
	}

	return ok;
}

bool GUIEditBoxComponent::OnMouseEvent(MouseEvent* e)
{
	IGUIManager* creator=owner->GetCreator();
	math::rectf clipRect= rc;
	clipRect.clipRect(owner->GetDefaultRegion()->GetClippedRect());
	bool res=false;

	switch(e->event)
	{
	case MET_LEFTDOWN:
		if(clipRect.IsPointInside(math::vector2d(e->pos.x,e->pos.y)))
		{
			creator->SetFocus(owner);
			res=true;
		}
		break;
	};
	if(res)
		e->setRecived();
	return res;
}



}
}