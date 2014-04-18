

/********************************************************************
	created:	2012/10/09
	created:	9:10:2012   22:06
	filename: 	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5\IRenderingState.h
	file path:	d:\Development\mrayEngine\Projects\VirtualTelesar\VTelesar5
	file base:	IRenderingState
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___IRenderingState___
#define ___IRenderingState___


#include "TBeeCommon.h"

namespace mray
{

	namespace xml
	{
		class XMLElement;
	}
namespace TBee
{

#define STATE_EXIT_CODE 1

class IRenderingState
{
protected:

	core::string m_name;
	uint m_exitCode;

	video::ITexturePtr m_rtTexture[2];
	video::IRenderTargetPtr m_renderTarget[2];
public:
	IRenderingState():m_exitCode(0)
	{}
	virtual~IRenderingState(){}

	void SetName(const core::string& n){m_name=n;}
	const core::string& GetName(){return m_name;}

	virtual void InitState();

	int GetEyeIndex(ETargetEye e){return (e==Eye_Left?0:1);}
	
	virtual bool OnEvent(Event* e, const math::rectf& rc){ return false; };
	virtual void OnEnter(IRenderingState*prev){};
	virtual void OnExit(){m_exitCode=0;}
	virtual video::IRenderTarget* Render(const math::rectf& rc,ETargetEye eye);
	virtual video::IRenderTarget* GetLastFrame(ETargetEye eye){return m_renderTarget[GetEyeIndex(eye)];}
	virtual void Update(float dt){};

	virtual void LoadFromXML(xml::XMLElement* e);
	virtual xml::XMLElement* WriteToXML(xml::XMLElement* e);

	virtual void ForceExit(){m_exitCode=STATE_EXIT_CODE;}
	
	uint GetExitCode(){return m_exitCode;}

	virtual bool CanSleep();

};

}
}

#endif
