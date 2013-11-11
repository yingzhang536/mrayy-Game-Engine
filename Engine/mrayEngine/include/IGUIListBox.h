

/********************************************************************
	created:	2011/11/20
	created:	20:11:2011   12:57
	filename: 	d:\Development\mrayEngine\Engine\mrayEngine\include\IGUIListBox.h
	file path:	d:\Development\mrayEngine\Engine\mrayEngine\include
	file base:	IGUIListBox
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __IGUIListBox__
#define __IGUIListBox__

#include "IGUIElement.h"
#include "IListItem.h"

namespace mray
{
namespace GUI
{
	class IListItem;

class MRAY_DLL IGUIListBox:public IGUIElement
{
private:
protected:
public:
	static const GUID ElementType;

	ObjectEvent OnKeyDown;//args=KeyboardEvent
	ObjectEvent OnSelectChange;//args=Index
public:
	IGUIListBox(IGUIManager* manager);
	virtual~IGUIListBox();

	virtual int GetItemsCount()const=0;
	virtual IListItem* GetItem(int index)const=0;
	virtual int FindItem(const core::UTFString& txt)const=0;
	virtual int AddItem(IListItem* item)=0;
	virtual int InsertItem(int index,IListItem* item)=0;
	virtual void DeleteItem(int index)=0;
	virtual const ItemList& GetItems()const=0;
	virtual void SortItems()=0;
	virtual void ClearItems()=0;

	virtual int GetSelectedItem()=0;
	virtual void SetSelectedItem(int i)=0;

	virtual int GetItemFromPos(const math::vector2d& y)=0;
};

}
}

#endif