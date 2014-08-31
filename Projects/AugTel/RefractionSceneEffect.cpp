

#include "stdafx.h"
#include "RefractionSceneEffect.h"
#include "TextureRTWrap.h"
#include "TextureResourceManager.h"

namespace mray
{
namespace AugTel
{


RefractionSceneEffect::RefractionSceneEffect()
{

	m_amount = 0;
	m_scale = 1;
	m_offset = 0;
	m_effect = new video::ParsedShaderPP(gEngine.getDevice());
}


RefractionSceneEffect::~RefractionSceneEffect()
{

}



void RefractionSceneEffect::SetAmount(float amount)
{
	m_amount = amount;
}


void RefractionSceneEffect::SetScale(const math::vector2d& v)
{
	m_scale = v;
}


void RefractionSceneEffect::SetOffset(const math::vector2d& v)
{
	m_offset = v;
}



void RefractionSceneEffect::Init()
{
	m_refraction = gTextureResourceManager.loadTexture2D("4480-normal.png");
	m_effect->LoadXML(gFileSystem.openFile("AugEffects\\Refraction.peff"));

}


void RefractionSceneEffect::Begin()
{

}


video::ITexture* RefractionSceneEffect::Render(video::ITexture* scene, const math::rectf& vp)
{

	m_effect->GetParam("normalMap")->SetTexture(m_refraction);
	m_effect->GetParam("scale")->SetValue(m_scale);
	m_effect->GetParam("offset")->SetValue(m_offset);
	m_effect->GetParam("amount")->SetValue(m_amount);

	video::TextureUnit tu;
	tu.SetTexture(m_refraction);
	gEngine.getDevice()->useTexture(1, &tu);
	m_effect->Setup(vp);
	m_effect->render(&video::TextureRTWrap(scene));

	return m_effect->getOutput()->GetColorTexture();
}



bool RefractionSceneEffect::Update(float dt)
{
	return false;
}


}
}

