

/********************************************************************
	created:	2011/11/20
	created:	20:11:2011   11:56
	filename: 	d:\Development\mrayEngine\Engine\mrayGUI\IListItem.h
	file path:	d:\Development\mrayEngine\Engine\mrayGUI
	file base:	IListItem
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __IListItem__
#define __IListItem__


namespace mray
{
namespace GUI
{


class IListItem
{
private:
protected:
public:
	void* userData;
	IListItem(void*ud=0):userData(ud){}
	virtual~IListItem(){}

	virtual core::UTFString toString()const=0;
};

class StringListItem:public IListItem
{
	core::UTFString m_text;
public:
	StringListItem(const core::string& txt,void* ud=0):IListItem(ud),m_text(txt)
	{}
	virtual~StringListItem()
	{}

	virtual core::UTFString toString()const
	{
		return m_text;
	}
};

typedef std::vector<IListItem*> ItemList;


}
}

#endif
