

#include "stdafx.h"
#include "GUIListBoxComponent.h"

#include "IGUIManager.h"
#include "IGUITheme.h"
#include "IListItem.h"
#include "IGUIElement.h"


namespace mray
{
namespace GUI
{
	const int GUIListBoxComponent::g_scrollSize=20;

GUIListBoxComponent::GUIListBoxComponent(IGUIElement* owner):
m_selectedItem(-1), m_startItem(0), m_upBotState(0), m_botBotState(0), m_drawBackground(true),
	m_scrollOn(false), m_currentPageSize(0), listener(0), m_itemHeight(18)
{
	this->owner=owner;
	m_sliderBar=new GUISliderbarComponent();
	m_sliderBar->owner=owner;
	m_hoveredItem=-1;
}
GUIListBoxComponent::~GUIListBoxComponent()
{
}

void GUIListBoxComponent::_decreaseStartItem(int c)
{
	m_startItem-=c;
	if(m_startItem<0)
		m_startItem=0;

}
void GUIListBoxComponent::_increaseStartItem(int c,int itmsCount)
{
	m_startItem+=c;
	int lastItem=math::Min<int>(m_startItem+m_currentPageSize,itmsCount);
	if(lastItem-m_startItem<m_currentPageSize)
		m_startItem=lastItem-m_currentPageSize;
}
int GUIListBoxComponent::_GetItemsCount(const math::rectf& rc,float& cDim)
{
	int h=GetLineHeight();
	return floor(rc.getHeight()/h);
}
float GUIListBoxComponent::GetLineHeight()
{
// 	GUI::IFont* font=gFontResourceManager.getFontByName(owner->GetFontAttributes()->fontName);
// 	float cDim=font->getCharDimension('A',owner->GetFontAttributes()->fontSize).y;
	return m_itemHeight;

}
void GUIListBoxComponent::GetScrollBarRects(const math::rectf& innerRect,int itemsCount,int PageSize,
											math::rectf &topArrow,math::rectf &botArrow,math::rectf &scrollerBG,math::rectf &scrollerBar)
{

	topArrow.ULPoint.x=innerRect.BRPoint.x;
	topArrow.ULPoint.y=innerRect.ULPoint.y;
	topArrow.BRPoint=topArrow.ULPoint+g_scrollSize;

	scrollerBG.ULPoint.x=innerRect.BRPoint.x;
	scrollerBG.ULPoint.y=topArrow.BRPoint.y;
	scrollerBG.BRPoint.x=topArrow.ULPoint.x+g_scrollSize;
	scrollerBG.BRPoint.y=innerRect.BRPoint.y-g_scrollSize;

	botArrow.ULPoint.x=scrollerBG.ULPoint.x;
	botArrow.ULPoint.y=scrollerBG.BRPoint.y;
	botArrow.BRPoint=botArrow.ULPoint+g_scrollSize;

	scrollerBar.ULPoint.x=innerRect.BRPoint.x;
	scrollerBar.ULPoint.y=scrollerBG.ULPoint.y + (scrollerBG.getHeight() * m_startItem)/(float)itemsCount;
	scrollerBar.BRPoint.x=innerRect.BRPoint.x + g_scrollSize;
	scrollerBar.BRPoint.y=scrollerBar.ULPoint.y + (scrollerBG.getHeight() * PageSize)/(float)itemsCount;

}

int GUIListBoxComponent::GetItemFromPos(const math::vector2d& pt,const math::rectf& clipRect)
{
	return _GetItemFromPos(pt,clipRect);
}
int GUIListBoxComponent::_GetItemFromPos(const math::vector2d& pt,const math::rectf& clipRect)
{
	IGUITheme* skin=owner->GetCreator()->GetActiveTheme();
	GUI::IFont* font=gFontResourceManager.getFontByName(owner->GetFontAttributes()->fontName);
	math::rectf innerRect;

	if(skin)
	{
		innerRect= skin->getSizableRect(4,clipRect,0,mT("Frame"));
	}else
		innerRect=clipRect;

	float cDim=GetLineHeight();
	int totalCount=_GetItemsCount(innerRect,cDim);
	if(m_startItem+totalCount>=items.size())
	{
		m_startItem=items.size()-totalCount;
		if(m_startItem<0)
			m_startItem=0;
	}
	if(m_startItem+totalCount>=items.size())
	{
		totalCount=items.size()-m_startItem;
	}

	float itemPos;
	itemPos=innerRect.ULPoint.y;
	int lastItem=m_startItem+totalCount;

	for(int i=m_startItem;i<lastItem;++i)
	{
		math::rectf itmRc(innerRect.ULPoint.x,itemPos,innerRect.BRPoint.x,itemPos+cDim);
		if(itmRc.IsPointInside(pt))
			return i;
		itemPos += m_itemHeight;
	}
	return -1;
}

void GUIListBoxComponent::OnValueChanged(GUISliderbarComponent*caller)
{
}

int GUIListBoxComponent::GetSelectedItem()
{
	return m_selectedItem;
}
void GUIListBoxComponent::SetSelectedItem(int itm,int itemsCount)
{
	m_selectedItem=itm;
	if(m_selectedItem<-1)m_selectedItem=-1;
	else if(m_selectedItem>=itemsCount)m_selectedItem=-1;
	if(listener)
		listener->OnChangeSelection(this);
}

void GUIListBoxComponent::SetSelectionColor(const video::SColor& clr)
{
	m_selectionColor=clr;
}
const video::SColor& GUIListBoxComponent::GetSelectionColor()const
{
	return m_selectionColor;
}

GUIListBoxComponent::EResultEvent GUIListBoxComponent::LBOnKeyboardEvent(KeyboardEvent* e,const math::rectf& rc)
{
	IGUITheme* skin=owner->GetCreator()->GetActiveTheme();
	GUI::IFont* font=gFontResourceManager.getFontByName(owner->GetFontAttributes()->fontName);
	EResultEvent res=ENone;
	if(e->press )
	{
		switch(e->key)
		{
		case KEY_UP:
			if(m_selectedItem<=0)
				break;
			SetSelectedItem(m_selectedItem-1,items.size());
			if(m_selectedItem>=0 && m_selectedItem<m_startItem)
			{
				m_startItem=m_selectedItem;
			}
			res=ESelectionChange;
			break;
		case KEY_DOWN:
			{
				if(m_selectedItem<(int)items.size()-1)
				{
					SetSelectedItem(m_selectedItem+1,items.size());
					res=ESelectionChange;
				}

				math::rectf innerRect;

				if(skin)
				{
					innerRect= skin->getSizableRect(4,rc,0,mT("Frame"));
				}else
					innerRect=rc;
				float cDim;
				int itemsCount=_GetItemsCount(innerRect,cDim);
				itemsCount=math::Min<int>(m_startItem+itemsCount,items.size())-m_startItem;
				if(m_selectedItem>=m_startItem+itemsCount)
				{
					m_startItem=m_selectedItem-itemsCount+1;
				}
			}
			break;
		}
		e->setRecived();
	}
	if(res==ESelectionChange)
	{
		m_sliderBar->SetPercentValue((m_startItem)/max(1,items.size()-m_currentPageSize));
	}
	return res;
}
GUIListBoxComponent::EResultEvent GUIListBoxComponent::LBOnMouseEvent(MouseEvent* e,const math::rectf& rc)
{

	if(!rc.IsPointInside(e->pos) && !IsTracking())
	{
		return ENone;
	}
	EResultEvent res=EReceived;
	IGUITheme* skin=owner->GetCreator()->GetActiveTheme();
//	GUI::IFont* font=gFontResourceManager.getFontByName(owner->GetFontAttributes()->fontName);

//	m_sliderBar->itemsCount=
	math::rectf innerRect;
	if(skin)
	{
		innerRect= skin->getSizableRect(4,rc,0,mT("Frame"));
	}else
		innerRect=rc;

	bool displayScrollBar=true;

	if(m_currentPageSize>=items.size())//don't display scroll bar if items count is low
		displayScrollBar=false;

	if(displayScrollBar)
		innerRect.BRPoint.x=math::Max<int>(innerRect.BRPoint.x-g_scrollSize,0);

	//float cDim=GetLineHeight();
	//int itemsCount=_GetItemsCount(font,innerRect,cDim);

	math::rectf topArrow,botArrow;
	math::rectf scrollerBG,scrollerBar;
	switch(e->event)
	{
	case MET_LEFTDOWN:
		{

			if(innerRect.IsPointInside(e->pos))
			{
				SetSelectedItem(_GetItemFromPos(e->pos,innerRect),items.size());
				e->setRecived();
				res=ESelectionChange;
				m_scrollOn=false;
			}else// if(!m_scrollOn)
			{
				/*
				GetScrollBarRects(innerRect,items.size(),m_currentPageSize,topArrow,
					botArrow,scrollerBG,scrollerBar);

				if(topArrow.IsPointInside(e->pos))
				{
					_decreaseStartItem(1);
					m_scrollOn=false;
					m_upBotState=2;
				}else if(botArrow.IsPointInside(e->pos))
				{
					_increaseStartItem(1,items.size());
					m_scrollOn=false;
					m_botBotState=2;
				}else if(scrollerBar.IsPointInside(e->pos))
				{
					m_scrollOn=!m_scrollOn;
					m_lastMousePosY=e->pos.y;
					m_startY=scrollerBar.ULPoint.y;
				}
				e->setRecived();*/
				if(m_sliderBar->OnMouseEvent(e))
					res=EReceived;
			}
		}
		break;
	case MET_LEFTUP:
		{
			if(m_sliderBar->OnMouseEvent(e))
				res=EReceived;
			//m_scrollOn=false;
			//e->setRecived();
		}break;
	case MET_MOVED:
		if(innerRect.IsPointInside(e->pos))
		{
			m_hoveredItem=_GetItemFromPos(e->pos,innerRect);
		}
		if(e->MouseWheelRel>0)
		{
			_decreaseStartItem(1);
		}else if(e->MouseWheelRel<0)
		{
			_increaseStartItem(1,items.size());;
		}
		if(m_sliderBar->OnMouseEvent(e))
			res=EReceived;
		/*GetScrollBarRects(innerRect,items.size(),m_currentPageSize,topArrow,
			botArrow,scrollerBG,scrollerBar);

		if(m_scrollOn)
		{

			float dy=e->pos.y-m_lastMousePosY;
			//m_lastMousePosY=e->pos.y;
			scrollerBar.ULPoint.y=m_startY+dy;
			m_startItem=(scrollerBar.ULPoint.y-scrollerBG.ULPoint.y)*items.size()/scrollerBG.getHeight();
			if(m_startItem<0)m_startItem=0;
			if(m_startItem>items.size()-m_currentPageSize)
				m_startItem=items.size()-m_currentPageSize;

			
		}else
		{
			if(topArrow.IsPointInside(e->pos))
			{
				m_upBotState=1;
			}else
				m_upBotState=0;
			if(botArrow.IsPointInside(e->pos))
			{
				m_botBotState=1;
			}else
				m_botBotState=0;
		}
		e->setRecived();*/
	}

	if(res==EReceived)
	{
		e->setRecived();
		m_startItem=math::clamp<int>((int) (m_sliderBar->GetPercentValue()*(items.size()-m_currentPageSize)),0,items.size()-1);
	}
	return res;
}

bool GUIListBoxComponent::OnKeyboardEvent(KeyboardEvent* e)
{
	return LBOnKeyboardEvent(e,rc)!=ENone;
}

bool GUIListBoxComponent::OnMouseEvent(MouseEvent* e)
{
	return LBOnMouseEvent(e,rc)!=ENone;
}
void GUIListBoxComponent::LBDraw(const math::rectf& rc)
{
	IGUIManager* creator=owner->GetCreator();
	IGUITheme* skin=creator->GetActiveTheme();
	GUI::IFont* font=gFontResourceManager.getFontByName(owner->GetFontAttributes()->fontName);
	math::rectf innerRect;

	math::rectf oldScissor=creator->GetDevice()->getScissorRect();
	creator->GetDevice()->setScissorRect(rc);

	if(skin)
	{
		innerRect= skin->getSizableRect(4,rc,0,mT("Frame"));
	}else
		innerRect=rc;


	float cDim=GetLineHeight();


	m_sliderBar->itemsCount=items.size();
	m_currentPageSize=_GetItemsCount(innerRect,cDim);
	m_sliderBar->pageSize=m_currentPageSize;

	if(m_startItem+m_currentPageSize>=items.size())
	{
		m_startItem=items.size()-m_currentPageSize;
		if(m_startItem<0)
			m_startItem=0;
	}
	if(m_startItem+m_currentPageSize>=items.size())
	{
		m_currentPageSize=items.size()-m_startItem;
	}

	float itemPos;
	itemPos=innerRect.ULPoint.y;
	int lastItem=m_startItem+m_currentPageSize;

	bool displayScrollBar=true;

	if(m_currentPageSize>=items.size())//don't display scroll bar if items count is low
		displayScrollBar=false;

	if(displayScrollBar)
	{
		innerRect.BRPoint.x=math::Max<int>(innerRect.BRPoint.x-g_scrollSize,0);
	}



	if (skin && m_drawBackground)
	{
		skin->drawSizableBox(creator->GetRenderQueue(),rc,0,mT("Frame"));
	}
	for(int i=m_startItem;i<lastItem;++i)
	{
		math::rectf itmRc(innerRect.ULPoint.x,itemPos,innerRect.BRPoint.x,itemPos);
		itmRc.BRPoint.y += GetItemHeight();
		video::SColor clr;
		if(i%2==0)
			clr=video::SColor(0.7,0.7,0.7,0.2);
		else
			clr=video::SColor(1,1,1,0.2);
		if (m_drawBackground)
			creator->GetRenderQueue()->AddQuad(0,itmRc,math::rectf(0,0,1,1),clr);
		items[i]->Draw(itmRc,owner, font, creator->GetRenderQueue());
		if(m_selectedItem==i)
		{
			creator->GetRenderQueue()->AddQuad(0,itmRc,math::rectf(0,0,1,1),video::SColor(0.3,0.3,0.6,0.5));
		}else if(i==m_hoveredItem)
		{
			creator->GetRenderQueue()->AddQuad(0,itmRc,math::rectf(0,0,1,1),video::SColor(0.5,0.5,0.5,0.5));
		}

		itemPos += itmRc.getHeight();
	}

	if(displayScrollBar)
	{
		m_sliderBar->rc=innerRect;
		m_sliderBar->rc.ULPoint.x=m_sliderBar->rc.BRPoint.x;
		m_sliderBar->rc.BRPoint.x+=g_scrollSize;
		/*
		math::rectf topArrow,botArrow;
		math::rectf scrollerBG,scrollerBar;

		GetScrollBarRects(innerRect,items.size(),m_currentPageSize,topArrow,botArrow,scrollerBG,scrollerBar);
		skin->drawBox(creator->GetRenderQueue(),scrollerBG,mT("ScrollVBG"),0,video::DefaultColors::White);
		skin->drawBox(creator->GetRenderQueue(),topArrow,mT("ScrollVCursor"),0+2*m_upBotState,video::DefaultColors::White);
		skin->drawBox(creator->GetRenderQueue(),botArrow,mT("ScrollVCursor"),1+2*m_botBotState,video::DefaultColors::White);
		skin->drawSizableBox3PV(creator->GetRenderQueue(),scrollerBar,m_scrollOn?1:0,mT("ScrollV"));
		if(m_upBotState==2)
			m_upBotState=1;
		if(m_botBotState==2)
			m_botBotState=1;*/
		m_sliderBar->Draw();
	}
	creator->GetRenderQueue()->Flush();
	creator->GetDevice()->setScissorRect(oldScissor);
}

void GUIListBoxComponent::Draw()
{
	LBDraw(rc);
}

void GUIListBoxComponent::LostFocus()
{
	m_scrollOn=false;
	m_upBotState=0;
	m_botBotState=0;
}
bool GUIListBoxComponent::IsTracking()
{
	return m_scrollOn;
}

}
}

