
#ifndef FadeSceneEffect_h__
#define FadeSceneEffect_h__

#include "ISceneEffect.h"
#include "ParsedShaderPP.h"

namespace mray
{
namespace AugTel
{

	enum class EFadeEffectType
	{
		FadeIn,
		FadeOut
	};
	
class FadeSceneEffect :public ISceneEffect
{
protected:
	EFadeEffectType m_type;
	float m_time;
	float m_targetTime;
	video::SColor m_targetColor;

	GCPtr<video::ParsedShaderPP> m_effect;

public:
	FadeSceneEffect();
	virtual ~FadeSceneEffect();

	void SetTime(float ms);
	void SetTargetColor(const video::SColor& clr);
	void SetType(EFadeEffectType type);

	virtual bool IsDone(){ return m_time == m_targetTime; }

	virtual void Init() ;
	virtual void Begin() ;
	virtual video::ITexture* Render(video::ITexture* scene, const math::rectf& vp);

	virtual bool Update(float dt) ;
};

}
}

#endif // FadeSceneEffect_h__

