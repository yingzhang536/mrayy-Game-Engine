

#include "stdafx.h"
#include "GUIConnectingScreenImpl.h"
#include "GUIListBoxComponent.h"


namespace mray
{
namespace GUI
{


GUIConnectingScreenImpl::GUIConnectingScreenImpl()
{

}
GUIConnectingScreenImpl::~GUIConnectingScreenImpl()
{
}

void GUIConnectingScreenImpl::SetStatus(const core::UTFString& st, bool connecting)
{
	core::UTFString str;
		
	if (m_status != core::UTFString::Empty)
	{
		str = m_status;
		str += core::UTFString(" [Connected]");
		GUI::StringListItem* item = new GUI::StringListItem(str);
		StatusLst->AddItem(item);
	}
	m_status = st;

	if (connecting)
		str = core::UTFString("Connecting: ");
	else
		str = core::UTFString::Empty;

	str += m_status;
	CurrentTxt->SetText(str);
}

void GUIConnectingScreenImpl::ClearStatus()
{
	StatusLst->ClearItems();
}

void GUIConnectingScreenImpl::DoneLoadingElements()
{
	StatusLst->GetComponent()->SetBackground(false);
}


}
}

