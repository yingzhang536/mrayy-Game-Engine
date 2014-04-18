#include "stdafx.h"

#include "IGUIManager.h"

#include "IGUIElement.h"


namespace mray{
namespace GUI{


IGUIManager::IGUIManager():m_device(0),m_rootElement(0),m_focusElement(0),m_MouseOnElement(0)
{

}
IGUIManager::~IGUIManager()
{
	m_rootElement=0;
}

void IGUIManager::SetRootElement(IGUIElement*  elem)
{
	m_rootElement=elem;
}
IGUIElement* IGUIManager::GetRootElement()
{
	return m_rootElement;
}
void IGUIManager::SetDevice(video::IVideoDevice* dev)
{
	m_device=dev;
}
video::IVideoDevice* IGUIManager::GetDevice()
{
	return m_device;
}

IGUIElement* IGUIManager::GetFocusElement()
{
	return m_focusElement;
}
bool IGUIManager::HasFocus(IGUIElement*  elem)
{
	return m_focusElement==elem;
}
void IGUIManager::RemoveFocus()
{
	SetFocus(0);
}
void IGUIManager::SetFocus(IGUIElement*  elem)
{
	if(m_focusElement==elem)return;
	IGUIElement*tmp=m_focusElement;
	m_focusElement=elem;
	if(tmp)
		tmp->_notifyLostFocus();
	if(m_focusElement)
		m_focusElement->_notifyReceiveFocus();

	OnChangeFocus(elem);
	FIRE_LISTENR_METHOD(OnChangeGUIFocusElement, (this, elem));
}

bool IGUIManager::HasMouseOn(IGUIElement* elem)
{
	return m_MouseOnElement==elem;
}
void IGUIManager::RemoveMouseOn()
{
	SetMouseOn(0);
}
void IGUIManager::SetMouseOn(IGUIElement*  elem)
{
	if(m_MouseOnElement==elem)return;
	IGUIElement*tmp=m_MouseOnElement;
	m_MouseOnElement=elem;
	if(tmp)
		tmp->_notifyMouseLeave();
	if(m_MouseOnElement)
		m_MouseOnElement->_notifyMouseEnter();

	OnChangeMouseOn(elem);
	FIRE_LISTENR_METHOD(OnChangeGUIHoverElement, (this, elem));
}
IGUIElement* IGUIManager::GetMouseOn()
{
	return m_MouseOnElement;
}




}
}