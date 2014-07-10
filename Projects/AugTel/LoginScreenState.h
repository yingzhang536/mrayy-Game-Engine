

/********************************************************************
	created:	2013/01/29
	created:	29:1:2013   0:16
	filename: 	C:\Development\mrayEngine\Projects\TELUBee\LoginScreenState.h
	file path:	C:\Development\mrayEngine\Projects\TELUBee
	file base:	LoginScreenState
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __LoginScreenState__
#define __LoginScreenState__

#include "IRenderingState.h"
#include "TBUserInfo.h"


namespace mray
{
	namespace GUI
	{
		class IGUIManager;
		class IGUIPanelElement;
	}
	class GUILoginMenu;
namespace AugTel
{
#define ToMap_CODE 0x100
#define ToLocalCamera_CODE 0x101
#define ToRemoteCamera_CODE 0x102
#define ToDepthView_CODE 0x103
#define ToTelesarSim_CODE 0x104

class LoginScreenState:public TBee::IRenderingState,public IDelegateContainer
{
protected:
	enum EStatus
	{
		EIdle,
		EDone
	};
	EStatus m_status;
	GUI::IGUIManager* m_guiManager;
	GUI::IGUIPanelElement* m_guiroot;

	GUILoginMenu* m_loginMenu;

	void OnExitPressed(IObject* caller,void* args);
	void OnConnectRemotePressed(IObject* caller, void* args);
	void OnConnectLocalPressed(IObject* caller, void* args);
	void OnDepthViewerPressed(IObject* caller, void* args);

public:
	LoginScreenState(const core::string& name);
	virtual~LoginScreenState();


	virtual void InitState();

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void OnEnter(IRenderingState*prev);
	virtual void OnExit();
	virtual video::IRenderTarget* Render(const math::rectf& rc,TBee::ETargetEye eye);
	virtual void Update(float dt);

	virtual void LoadFromXML(xml::XMLElement* e);
};

}
}


#endif
