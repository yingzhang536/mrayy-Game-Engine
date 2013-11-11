


/********************************************************************
	created:	2011/11/22
	created:	22:11:2011   14:32
	filename: 	d:\Development\mrayEngine\Engine\mrayGUI\GUIListBoxComponent.h
	file path:	d:\Development\mrayEngine\Engine\mrayGUI
	file base:	GUIListBoxComponent
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __GUIListBoxComponent__
#define __GUIListBoxComponent__

#include "IGUIComponent.h"
#include "IListItem.h"
#include "GUISliderbarComponent.h"

namespace mray
{
namespace GUI
{

class GUIListBoxComponent:public IGUIComponent,public GUISliderbarComponent::IListener
{
public:
	class IListener
	{
	public:
		virtual void OnChangeSelection(GUIListBoxComponent*caller)=0;
	};
private:
protected:
	static const int g_scrollSize;
	int m_selectedItem;
	int m_startItem;
	GUISliderbarComponent* m_sliderBar;

	video::SColor m_selectionColor;
	bool m_scrollOn;
	float m_lastMousePosY;
	float m_startY;
	int m_upBotState;
	int m_botBotState;
	int m_hoveredItem;

	int m_currentPageSize;

	int _GetItemsCount(const math::rectf& rc,float& cDim);
	void GetScrollBarRects(const math::rectf& innerRect,int itemsCount,int PageSize,
		math::rectf &topArrow,math::rectf &botArrow,math::rectf &scrollerBG,math::rectf &scrollerBar);

	void _decreaseStartItem(int c);
	void _increaseStartItem(int c,int itmsCount);

public:

	enum EResultEvent
	{
		ENone,
		EReceived,
		ESelectionChange,
	};

	int _GetItemFromPos(const math::vector2d& pt,const math::rectf& clippedRect);
public:

	ItemList items;

	IListener* listener;

	GUIListBoxComponent(IGUIElement* owner);
	virtual~GUIListBoxComponent();

	void OnValueChanged(GUISliderbarComponent*caller);

	void SetStartItem(int i){m_startItem=i;}
	int GetSelectedItem();
	void SetSelectedItem(int itm,int itemsCount);

	void SetSelectionColor(const video::SColor& clr);
	const video::SColor& GetSelectionColor()const;

	float GetLineHeight();

	virtual int GetItemFromPos(const math::vector2d& pt,const math::rectf& clippedRect);

	virtual void LBDraw(const math::rectf& rc);
	virtual void Draw();

	virtual EResultEvent LBOnKeyboardEvent(KeyboardEvent* e,const math::rectf& rc);
	virtual EResultEvent LBOnMouseEvent(MouseEvent* e,const math::rectf& rc);

	virtual bool OnKeyboardEvent(KeyboardEvent* e);
	virtual bool OnMouseEvent(MouseEvent* e);


	virtual void LostFocus();

	bool IsTracking();
};


}
}

#endif
