

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
		"half4 main_fp(float2 texCoord : TEXCOORD0, "
		"uniform sampler2D texA : register(s0),uniform float alpha,uniform float gray) : COLOR "
		"{"
		"half4 clr=tex2D(texA,texCoord);"
		"half2 v=(texCoord-0.5);"
		"half dst=dot(v,v);"
		"dst=dst>0.25?0:1;"
		"return half4(clr.rgb,dst*alpha);"
		"}";

GUIProfilePicture::GUIProfilePicture(IGUIManager* m)
:GUIStaticImage(m)
{
	m_type = &ElementType;

	video::IGPUShaderProgramPtr shader = gShaderResourceManager.getResource("ProfileMasking");
	if (!shader)
	{
		shader = gShaderResourceManager.loadShaderFromProgram("ProfileMasking",ProfileMaskingshader, video::EShader_FragmentProgram, "main_fp", video::ShaderPredefList(), mT("cg"));
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
	video::IVideoDevice*device = creator->GetDevice();
	video::IGPUShaderProgram *shader = (video::IGPUShaderProgram*)gShaderResourceManager.getResource("ProfileMasking").pointer();
	if (shader)
	{
		device->setFragmentShader(shader);
		float a = GetDerivedAlpha();
		shader->setConstant("alpha", &a, 1);
		shader->setConstant("gray", &a, 0);
	}
	GUIStaticImage::Draw(vp);
	if (shader)
		device->setFragmentShader(0);

	IGUIElement::Draw(vp);
}


	IMPLEMENT_RTTI(GUIProfilePicture, GUIStaticImage);
	IMPLEMENT_ELEMENT_FACTORY(GUIProfilePicture);

}
}