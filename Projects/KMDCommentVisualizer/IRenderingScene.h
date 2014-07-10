

#ifndef IRenderingScene_h__
#define IRenderingScene_h__

#include "IRenderTarget.h"

namespace mray
{

namespace kmd
{


class IRenderingScene
{
protected:
	core::string m_name;
	uint m_exitCode;

	video::ITexturePtr m_rtTexture;
	video::IRenderTargetPtr m_renderTarget;
public:
	IRenderingScene(){}
	virtual~IRenderingScene(){}

	void SetName(const core::string& n){ m_name = n; }
	const core::string& GetName(){ return m_name; }

	video::IRenderTargetPtr GetRenderTarget(){
		return m_renderTarget;
	}

	uint GetExitCode(){ return m_exitCode; }

	virtual void Init() ;
	virtual void OnEnter() = 0;
	virtual void OnExit() = 0;

	virtual bool OnEvent(Event* e, const math::rectf& rc) = 0;
	virtual void Update(float dt) = 0;
	virtual video::IRenderTarget* Draw(const math::rectf& rc);

};

}
}
#endif // IRenderingScene_h__
