

#include "stdafx.h"
#include "GUIProfilePicture.h"
#include "IGUIManager.h"
#include "IVideoDevice.h"
#include "IGUIRenderer.h"
#include "GUIElementRegion.h"


#include "ShaderResourceManager.h"


namespace mray
{

namespace GUI
{
	const GUID GUIProfilePicture::ElementType = "GUIProfilePicture";

	const core::string ProfileMaskingshader =
		"float4 main_fp(float2 texCoord : TEXCOORD0, \
		uniform sampler2D texA : register(s0),uniform float Alpha) : COLOR \
		{\
		vec4 clr=tex2D(texA,texCoord);\
		float2 v=(texCoord-0.5);\
		float dst=dot(v,v);\
		dst=dst>0.25?0:Alpha;\
		return float4(clr.rgb,dst);\
		}";

GUIProfilePicture::GUIProfilePicture(IGUIManager* m)
:GUIStaticImage(m)
{
	m_type = &ElementType;

	video::IGPUShaderProgramPtr shader = gShaderResourceManager.getResource("ProfileMasking");
	if (!shader)
	{
		shader = gShaderResourceManager.loadShader(ProfileMaskingshader, video::EShader_FragmentProgram, "main_fp", mT("cg"), false);
		shader->setResourceName("ProfileMasking");
		gShaderResourceManager.addResource(shader, "ProfileMasking");
	}
}
GUIProfilePicture::~GUIProfilePicture()
{

}

void GUIProfilePicture::Draw(const math::rectf*vp)
{

	if (!IsVisible())return;
	IGUIManager* creator = GetCreator();
	const math::rectf& rect = GetDefaultRegion()->GetRect();
	const math::rectf& clip = GetDefaultRegion()->GetClippedRect();
	video::IVideoDevice*device = creator->GetDevice();
	video::IGPUShaderProgram *shader = (video::IGPUShaderProgram*)gShaderResourceManager.getResource("ProfileMasking").pointer();
	if (shader)
	{
		device->setFragmentShader(shader);
		float a = GetDerivedAlpha();
		shader->setConstant("Alpha", &a,1);
	}
	creator->GetRenderQueue()->AddQuad(m_textureUnit, clip, m_texCoords, video::SColor(GetColor().R, GetColor().G, GetColor().B, GetDerivedAlpha()));
	creator->GetRenderQueue()->Flush();
	if (shader)
		device->setFragmentShader(0);

	IGUIElement::Draw(vp);
}


	IMPLEMENT_RTTI(GUIProfilePicture, GUIStaticImage);
	IMPLEMENT_ELEMENT_FACTORY(GUIProfilePicture);

}
}