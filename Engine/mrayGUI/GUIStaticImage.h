

/********************************************************************
	created:	2011/11/27
	created:	27:11:2011   18:01
	filename: 	d:\Development\mrayEngine\Engine\mrayGUI\GUIStaticImage.h
	file path:	d:\Development\mrayEngine\Engine\mrayGUI
	file base:	GUIStaticImage
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef ___GUIIMAGE___
#define ___GUIIMAGE___

#include "CompileConfig.h"
#include "IGUIStaticImage.h"
#include "IDelegate.h"
#include "FontAttributes.h"
#include "TextureUnit.h"

#include "GUIElementFactoryDef.h"


namespace mray{
namespace GUI{

class MRAYGUI_API GUIStaticImage:public IGUIStaticImage
{
	DECLARE_RTTI
protected:
	video::TextureUnitPtr m_textureUnit;
	bool m_autoStretch;
	math::rectf m_texCoords;
	core::string m_source;

	virtual void fillProperties();
	bool _OnMouseEvent(MouseEvent* e);
public:

	DECLARE_PROPERTY_TYPE(TexCoords,math::rectf,MRAYGUI_API);
	DECLARE_PROPERTY_TYPE(Source,core::string,MRAYGUI_API);
public:

	GUIStaticImage(IGUIManager* creator);
	~GUIStaticImage();

	void SetImage(video::ITextureCRef tex);
	video::ITextureCRef GetImage();

	video::TextureUnit* GetTextureUnit();

	bool SetTargetTexCoords(const math::rectf& rc);
	const math::rectf&  GetTargetTexCoords();

	bool SetSourceImage(const core::string&path);
	const core::string& GetSourceImage();

	void Draw(video::IRenderArea*vp);

	IGUIElement* Duplicate();
	FontAttributes* GetFontAttributes();
};
DECLARE_ELEMENT_FACTORY(GUIStaticImage);

}
}

#endif

