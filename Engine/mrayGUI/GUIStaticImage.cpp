
#include "stdafx.h"
#include "GUIStaticImage.h"
#include "IGUITheme.h"
#include "IGUIManager.h"
#include "IVideoDevice.h"

#include "TextureResourceManager.h"

namespace mray{
namespace GUI{

	IMPLEMENT_ELEMENT_FACTORY(GUIStaticImage);

GUIStaticImage::GUIStaticImage(IGUIManager* creator):
IGUIStaticImage(creator),m_texCoords(0,0,1,1)
{
	m_stretchMode = EImage_Stretch;
	m_textureUnit=new video::TextureUnit();
	fillProperties();
}

GUIStaticImage::~GUIStaticImage()
{
}
void GUIStaticImage::fillProperties()
{
	CPropertieDictionary *dic=0;
	if(CreateDictionary(&dic))
	{

		dic->addPropertie(&PropertyTypeStretchMode::instance, mT("Design"));
		dic->addPropertie(&PropertyTypeSource::instance, mT("Design"));
		dic->addPropertie(&PropertyTypeTexCoords::instance,mT("Design"));

	}
}

void GUIStaticImage::SetImage(video::ITextureCRef tex)
{
	m_textureUnit->SetTexture(tex);
}

video::ITextureCRef GUIStaticImage::GetImage()
{
	return m_textureUnit->GetTexture();
}

video::TextureUnit* GUIStaticImage::GetTextureUnit()
{
	return m_textureUnit;
}


bool GUIStaticImage::SetTargetTexCoords(const math::rectf& rc)
{
	m_texCoords=rc;
	return true;
}
const math::rectf&  GUIStaticImage::GetTargetTexCoords()
{
	return m_texCoords;
}

bool GUIStaticImage::SetSourceImage(const core::string&path)
{
	m_source=path;
	video::ITexturePtr tex = TextureResourceManager::getInstance().loadTexture2D(path);
	SetImage(tex);
	return !tex.isNull();
}

void GUIStaticImage::SetStretchMode(EImageStretchMode m)
{ 
	m_stretchMode = m; 
	switch (m_stretchMode)
	{
	case mray::GUI::EImage_None:
	case mray::GUI::EImage_Stretch:
	case mray::GUI::EImage_Center:
	case mray::GUI::EImage_Zoom:
		m_textureUnit->setTextureClamp(video::ETW_WrapT, video::ETC_CLAMP_TO_EDGE);
		m_textureUnit->setTextureClamp(video::ETW_WrapS, video::ETC_CLAMP_TO_EDGE);
		m_textureUnit->setTextureClamp(video::ETW_WrapR, video::ETC_CLAMP_TO_EDGE);
		break;
		break;
	case mray::GUI::EImage_Tile:
		m_textureUnit->setTextureClamp(video::ETW_WrapT, video::ETC_REPEAT);
		m_textureUnit->setTextureClamp(video::ETW_WrapS, video::ETC_REPEAT);
		m_textureUnit->setTextureClamp(video::ETW_WrapR, video::ETC_REPEAT);
		break;
	default:
		break;
	}
}


const core::string& GUIStaticImage::GetSourceImage()
{
	return m_source;
}

void GUIStaticImage::Draw(const math::rectf*vp)
{
	if(!IsVisible())return;
	IGUIManager* creator=GetCreator();
	const math::rectf& rect=GetDefaultRegion()->GetRect();
	const math::rectf& clip=GetDefaultRegion()->GetClippedRect();
	video::IVideoDevice*device=creator->GetDevice();

	math::vector2d texSz;
	math::rectf tc;

	if (!m_textureUnit->GetTexture())
	{
		tc = m_texCoords;
	}
	else
	{
		texSz.x = m_textureUnit->GetTexture()->getSize().x;
		texSz.y = m_textureUnit->GetTexture()->getSize().y;
		tc = m_texCoords;

		switch (m_stretchMode)
		{
		case mray::GUI::EImage_None:
			tc.ULPoint = m_texCoords.ULPoint;
			tc.BRPoint.x = math::Min(m_texCoords.BRPoint.x, m_texCoords.ULPoint.x + texSz.x / clip.getWidth());
			tc.BRPoint.y = math::Min(m_texCoords.BRPoint.y, m_texCoords.ULPoint.y + texSz.y / clip.getHeight());
			break;
		case mray::GUI::EImage_Stretch:
			break;
		case mray::GUI::EImage_Center:
			break;
		case mray::GUI::EImage_Zoom:
		{
			float min = math::Min(clip.getWidth(), clip.getHeight());
			tc.ULPoint = m_texCoords.ULPoint;
			tc.BRPoint.x =  m_texCoords.ULPoint.x + m_texCoords.getWidth() * clip.getWidth()/min;
			tc.BRPoint.y =  m_texCoords.ULPoint.y + m_texCoords.getHeight() * clip.getHeight() / min;
		}
			break;
		case mray::GUI::EImage_Tile:
			break;
		default:
			break;
		}
	}

	creator->GetRenderQueue()->AddQuad(m_textureUnit,clip,tc,video::SColor(GetColor().R,GetColor().G,GetColor().B,GetDerivedAlpha()));
	creator->GetRenderQueue()->Flush();

	IGUIElement::Draw(vp);
}
bool GUIStaticImage::_OnMouseEvent(MouseEvent*  e)
{
	const math::rectf& clipped=GetDefaultRegion()->GetClippedRect();
	if(!clipped.IsPointInside(e->pos))
	{
		return false;
	}
	GetCreator()->SetMouseOn(this);
	if(e->event==MET_LEFTDOWN
		|| e->event==MET_RIGHTDOWN)
	{
		OnClick(this,&e->event);
	}else
	{
		OnMouseMove(this,&e->pos);
	}
	e->setRecived();
	return true;
}

IGUIElement* GUIStaticImage::Duplicate()
{
	return new GUIStaticImage(GetCreator());
}
FontAttributes* GUIStaticImage::GetFontAttributes()
{
	return 0;
}

/*

void GUIStaticImage::fillProperties()
{
	addPropertie(&AutoStretch,mT("Apperance"));
	addPropertie(&TexCoords,mT("Apperance"));
	addPropertie(&Source,mT("Apperance"));
}

const video::ITexturePtr& GUIStaticImage::getImage()
{
	return m_textureUnit->GetTexture();
}

void GUIStaticImage::setTexture(const video::ITexturePtr&   tex)
{
	m_textureUnit->SetTexture(tex);
}
const video::ITexturePtr&  GUIStaticImage::getTexture(){
	return m_textureUnit->GetTexture();
}

void GUIStaticImage::draw(float dt)
{
	if(!Visible)return;
	if(!guiMngr)return;
//	IGUITheme* skin=GUIThemeManager::getInstance().getActiveTheme(); 	
	if(AutoStretch && m_textureUnit->GetTexture()){
		this->MainSize=math::vector2d(m_textureUnit->GetTexture()->getSize().x,m_textureUnit->GetTexture()->getSize().y);
	}
	math::rectf rc=getRect();
	if(getParent() && getParent()->EnableClip)
		rc.clipRect(getParent()->getClipRect());
	//rc.clipRect(getClipRect());
//	if(m_textureUnit)
		guiMngr->getDevice()->draw2DImage(rc,m_textureUnit,video::SColor(Color().R,Color().G,Color().B,AlphaEffect ? Alpha:Color().A),&getClipRect(),&m_texCoords);
//	else skin->drawPanel(rc,video::SColor(Color().R,Color().G,Color().B,AlphaEffect ? Alpha:Color().A),&getClipRect());
	GUIElement::draw(dt);
	
}

void GUIStaticImage::onEvent(const EventPtr& e)
{
	if(!Visible)return;
	if(!Enable)return;
	if(!parentEnabled)return;
	bool ok=0;
	GUIElement::onEvent(e);
	GUI_EVENT_TYPE EventType=GET_NONE;
	math::rectf rc=getRect();
	switch(e->getType())
	{
	case ET_Mouse:
		{
		GCPtr<MouseEvent> event=e;
		if(rc.IsPointInside(math::vector2d(event->x,event->y))&&
			!event->isRecived())
		{
			if(event->Event==MET_MOVED)
			{
				if(!guiMngr->hasMouseOn(this))
				{
					EventType=GET_Mouse_MOVE;
					guiMngr->setMouseOn(this);
				}
				ok=1;
			}else
			if(event->Event==MET_LEFTDOWN)
			{
				if(Enable){
					EventType=GET_Mouse_DOWN;
					guiMngr->setFocus(this);
				}
				ok=1;
			}else if(event->Event==MET_LEFTUP)
			{
				if(Enable){
					EventType=GET_Mouse_UP;
				}
				ok=1;
				break;
			}
		}
		}break;
	};

	if(ok){
		e->setRecived();
		GCPtr<GUIEvent> event1=EventMemoryManager::getInstance().createEvent(ET_GuiEvent);
		event1->Caller=this;
		event1->Event=EventType;
		guiMngr->onEvent(event1);
		if(m_event)
			m_event->onEvent(event1);
	}
}


GCPtr<GUIElement> GUIStaticImage::getDuplicate()
{
	GUIStaticImage*dup=guiMngr->addImage(getPos(),getSize(),m_event,getParent(),getId());
	
	dup->AutoStretch=AutoStretch;
	dup->m_textureUnit=m_textureUnit;
	dup->TexCoords=TexCoords;

	return dup;
}


void GUIStaticImage::setSource(const core::string&s){
	m_textureUnit->SetTexture(gTextureResourceManager.loadTexture2D(s));
	m_source=s;
}
const core::string& GUIStaticImage::getSource(){
	return m_source;
}*/



IMPLEMENT_PROPERTY_TYPE_HEADER(StretchMode, GUIStaticImage, m_stretchMode, mT("StretchMode"), EPBT_Enum, mT(""), EImage_Stretch);

GUIStaticImage::PropertyTypeStretchMode GUIStaticImage::PropertyTypeStretchMode::instance;
EImageStretchMode GUIStaticImage::PropertyTypeStretchMode::GetValue(CPropertieSet*object)const
{
	GUIStaticImage* o = dynamic_cast<GUIStaticImage*>(object);
	if (!o)
		return m_default;
	return o->GetStretchMode();
}
bool GUIStaticImage::PropertyTypeStretchMode::SetValue(CPropertieSet*object, const EImageStretchMode& v)
{
	GUIStaticImage* o = dynamic_cast<GUIStaticImage*>(object);
	if (!o)return false;
	 o->SetStretchMode(v);
	 return true;
}
core::string GUIStaticImage::PropertyTypeStretchMode::toString(CPropertieSet*object)const
{
	switch (GetValue(object))
	{
	case EImage_None:
		return "None";
	case EImage_Stretch:
		return "Stretch";
	case EImage_Tile:
		return "Tile";
	case EImage_Center:
		return "Center";
	case EImage_Zoom:
		return "Zoom";
	default:
		break;
	}
	return "";
}
bool GUIStaticImage::PropertyTypeStretchMode::parse(CPropertieSet*reciver, const core::string&str)
{
	EImageStretchMode v;
	if (str.equals_ignore_case("None"))
		v = EImage_None;
	else if (str.equals_ignore_case("Stretch"))
		v = EImage_Stretch;
	else if (str.equals_ignore_case("Tile"))
		v = EImage_Tile;
	else if (str.equals_ignore_case("Center"))
		v = EImage_Center;
	else if (str.equals_ignore_case("Zoom"))
		v = EImage_Zoom;
	
	return SetValue(reciver, (EImageStretchMode)v);
}

bool GUIStaticImage::PropertyTypeStretchMode::isDefault(CPropertieSet*reciver)const
{
	if (GetValue(reciver) == GetDefaultValue())
		return true;
	return false;
}


IMPLEMENT_PROPERTY_TYPE_HEADER(TexCoords,GUIStaticImage,rect,mT("TexCoords"),EPBT_Struct,mT(""),math::rectf(0,0,1,1));
IMPLEMENT_PROPERTY_TYPE_GENERIC(TexCoords,GUIStaticImage,math::rectf,SetTargetTexCoords,GetTargetTexCoords,core::StringConverter::toString,core::StringConverter::toRect,false)

IMPLEMENT_PROPERTY_TYPE_HEADER(Source,GUIStaticImage,string,mT("Source"),EPBT_Basic,mT(""),mT(""));
IMPLEMENT_PROPERTY_TYPE_GENERIC(Source,GUIStaticImage,core::string,SetSourceImage,GetSourceImage,,,false)

}
}

