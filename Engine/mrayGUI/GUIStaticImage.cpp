
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

		dic->addPropertie(&PropertyTypeSource::instance,mT("Design"));
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
	creator->GetRenderQueue()->AddQuad(m_textureUnit,clip,m_texCoords,video::SColor(GetColor().R,GetColor().G,GetColor().B,GetDerivedAlpha()));
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




IMPLEMENT_PROPERTY_TYPE_HEADER(TexCoords,GUIStaticImage,rect,mT("TexCoords"),EPBT_Struct,mT(""),math::rectf(0,0,1,1));
IMPLEMENT_PROPERTY_TYPE_GENERIC(TexCoords,GUIStaticImage,math::rectf,SetTargetTexCoords,GetTargetTexCoords,core::StringConverter::toString,core::StringConverter::toRect,false)

IMPLEMENT_PROPERTY_TYPE_HEADER(Source,GUIStaticImage,string,mT("Source"),EPBT_Basic,mT(""),mT(""));
IMPLEMENT_PROPERTY_TYPE_GENERIC(Source,GUIStaticImage,core::string,SetSourceImage,GetSourceImage,,,false)

}
}

