

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



namespace mray
{

	namespace xml
	{
		class XMLElement;
	}
namespace VT
{
	class Application;

#define STATE_EXIT_CODE 1

class IRenderingState
{
protected:

	core::string m_name;
	uint m_exitCode;

	video::ITexturePtr m_rtTexture[2];
	video::IRenderTargetPtr m_renderTarget[2];

	bool m_stereo;

public:
	IRenderingState():m_exitCode(0)
	{}
	virtual~IRenderingState(){}

	void SetName(const core::string& n){m_name=n;}
	const core::string& GetName(){return m_name;}

	virtual void InitState(Application* app);
	
	virtual void OnEvent(Event* e){};
	virtual void OnEnter(IRenderingState*prev){};
	virtual void OnExit(){m_exitCode=0;}
	virtual video::IRenderTarget* Render(bool left,const math::rectf& rc);
	virtual video::IRenderTarget* GetLastFrame(bool left);
	virtual void Update(float dt){};

	virtual void LoadFromXML(xml::XMLElement* e);

	virtual void ForceExit(){m_exitCode=STATE_EXIT_CODE;}
	
	uint GetExitCode(){return m_exitCode;}

	virtual bool CanSleep(){return true;}

};

}
}

#endif
