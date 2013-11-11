
/********************************************************************
	created:	2009/02/02
	created:	2:2:2009   22:10
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\src\IGUIManager.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\src
	file base:	IGUIManager
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___IGUIManager___
#define ___IGUIManager___

#include "GUID.h"
#include "IGUIElement.h"
#include "IDelegate.h"

namespace mray{
	class IEventReciver;
	class Event;

namespace video
{
	class IRenderArea;
	class IVideoDevice;
}
namespace GUI{

	class IGUIRenderer;
	class LanguageFontSpecification;
	class IGUITheme;
	class IGUIElementFactory;

class MRAY_DLL IGUIManager
{
protected:
	video::IVideoDevice* m_device;

	GCPtr<IGUIElement>	m_rootElement;
	IGUIElement*	m_focusElement;
	IGUIElement*	m_MouseOnElement;
	IEventReciver*	m_listener;

	std::list<IGUIElement*> m_postDrawElements;
public:
	typedef DelegateEvent1<IGUIElement*> IGUIElementEvent;

	IGUIElementEvent OnChangeFocus;
	IGUIElementEvent OnChangeMouseOn;
public:
	IGUIManager();
	virtual~IGUIManager();

	virtual void SetDevice(video::IVideoDevice* dev);
	video::IVideoDevice* GetDevice();

	virtual void SetRootElement(IGUIElement*  elem);
	virtual IGUIElement* GetRootElement();

	virtual void AddPostDrawElement(IGUIElement* e){m_postDrawElements.push_back(e);}

	virtual void		SetActiveTheme(IGUITheme* theme)=0;
	virtual IGUITheme*	GetActiveTheme()=0;

	virtual LanguageFontSpecification* GetLanguageSpecification()=0;

	virtual void RegisterBenchmark()=0;
	virtual void UnregisterBenchmark()=0;

	virtual IGUIRenderer* GetRenderQueue()=0;

	//! get the current focus elem
	virtual IGUIElement* GetFocusElement();
	virtual bool		HasFocus(IGUIElement*  elem);
	virtual void		RemoveFocus();
	virtual void		SetFocus(IGUIElement*  elem);

	//! get the current element which has mouse on it
	virtual bool			HasMouseOn(IGUIElement* elem);
	virtual void			RemoveMouseOn();
	virtual void			SetMouseOn(IGUIElement*  elem);
	virtual IGUIElement*	GetMouseOn();

	virtual IGUIElement*	GetElementFromPoint(float x,float y)=0;

	virtual bool OnEvent(Event* event)=0;
	virtual void DrawAll(video::IRenderArea*vp)=0;

	virtual void Update(float dt)=0;

	virtual IGUIElement* CreateElement(const GUID& type)=0;

	virtual void SetEventListener(IEventReciver*l);
};

}
}


#endif //___IGUIManager___

