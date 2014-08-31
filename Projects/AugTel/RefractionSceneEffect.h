

#ifndef RefractionSceneEffect_h__
#define RefractionSceneEffect_h__

#include "ISceneEffect.h"
#include "ParsedShaderPP.h"


namespace mray
{
namespace AugTel
{

class RefractionSceneEffect :public ISceneEffect
{
protected:
	float m_amount;
	math::vector2d m_scale;
	math::vector2d m_offset;

	video::ITexturePtr m_refraction;

	GCPtr<video::ParsedShaderPP> m_effect;

public:
	RefractionSceneEffect();
	virtual ~RefractionSceneEffect();

	void SetAmount(float amount);
	void SetScale(const math::vector2d& v);
	void SetOffset(const math::vector2d& v);

	virtual bool IsDone(){ return false; }

	virtual void Init();
	virtual void Begin();
	virtual video::ITexture* Render(video::ITexture* scene, const math::rectf& vp);

	virtual bool Update(float dt);
};


}
}

#endif // RefractionSceneEffect_h__

