


#ifndef ISceneEffect_h__
#define ISceneEffect_h__

#include "IRenderTarget.h"

namespace mray
{
namespace AugTel
{
	
class ISceneEffect
{
protected:
public:
	ISceneEffect(){}
	virtual ~ISceneEffect(){}

	virtual void Init() = 0;
	virtual void Begin() = 0;
	virtual video::ITexture* Render(video::ITexture* scene, const math::rectf& vp) = 0;

	virtual bool IsDone() = 0;

	virtual bool Update(float dt) = 0;//return true in case the effect is done
};

MakeSharedPtrType(ISceneEffect)

}
}

#endif // ISceneEffect_h__
