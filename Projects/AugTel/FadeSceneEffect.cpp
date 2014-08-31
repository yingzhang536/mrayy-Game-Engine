

#include "stdafx.h"
#include "FadeSceneEffect.h"
#include "TextureRTWrap.h"


namespace mray
{
namespace AugTel
{

FadeSceneEffect::FadeSceneEffect()
{
	m_targetColor = video::DefaultColors::Black;
	m_time = 0;
	m_targetTime = 1000.0f;
	m_type = EFadeEffectType::FadeIn;
	m_effect = new video::ParsedShaderPP(gEngine.getDevice());
}

FadeSceneEffect::~FadeSceneEffect()
{

}


void FadeSceneEffect::SetTime(float ms)
{
	m_targetTime = ms;
}

void FadeSceneEffect::SetTargetColor(const video::SColor& clr)
{
	m_targetColor = clr;
}

void FadeSceneEffect::SetType(EFadeEffectType type)
{
	m_type = type;
}


void FadeSceneEffect::Init()
{
	m_time = 0;
	m_effect->LoadXML(gFileSystem.openFile("AugEffects\\Fade.peff"));
}

void FadeSceneEffect::Begin()
{
	m_time = 0;

}

video::ITexture* FadeSceneEffect::Render(video::ITexture* scene, const math::rectf& vp)
{
	float t = m_time / m_targetTime;
	if (m_type == EFadeEffectType::FadeOut)
		t = 1 - t;
	math::vector3d clr(m_targetColor.R, m_targetColor.G, m_targetColor.B);
	
	m_effect->GetParam("t")->SetValue(t);
	m_effect->GetParam("targetColor")->SetValue(clr);

	m_effect->Setup(vp);		
	m_effect->render(&video::TextureRTWrap(scene));

	return m_effect->getOutput()->GetColorTexture();
}


bool FadeSceneEffect::Update(float dt)
{
	m_time += dt;
	if (m_time >= m_targetTime)
	{
		m_time = m_targetTime;
		return true;
	}
	return false;
}

}
}

