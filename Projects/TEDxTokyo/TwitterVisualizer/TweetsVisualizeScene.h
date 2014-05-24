


#ifndef TweetsVisualizeScene_h__
#define TweetsVisualizeScene_h__


#include "IRenderingScene.h"

namespace mray
{
namespace ted
{

class TweetsVisualizeSceneImpl;

class TweetsVisualizeScene :public IRenderingScene
{
protected:
	TweetsVisualizeSceneImpl* m_impl;
public:
	TweetsVisualizeScene();
	virtual~TweetsVisualizeScene();


	virtual void Init();
	virtual void OnEnter();
	virtual void OnExit();

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void Update(float dt);
	virtual video::IRenderTarget* Draw(const math::rectf& rc);
};

}
}

#endif // TweetsVisualizeScene_h__

