
#ifndef SessionScene_h__
#define SessionScene_h__

#include "IRenderingScene.h"

namespace mray
{
namespace ted
{


class SessionScene :public IRenderingScene
{
protected:
public:
	SessionScene();
	virtual~SessionScene();


	virtual void Init();
	virtual void OnEnter() ;
	virtual void OnExit() ;

	virtual bool OnEvent(Event* e, const math::rectf& rc) ;
	virtual void Update(float dt) ;
	virtual video::IRenderTarget* Draw(const math::rectf& rc);
};

}
}

#endif // SessionScene_h__
