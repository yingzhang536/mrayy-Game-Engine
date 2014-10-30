

#include "stdafx.h"
#include "GUIInterfaceScreenImpl.h"
#include "ImageSetResourceManager.h"
#include "GUIOverlayManager.h"
#include "GUIOverlay.h"


namespace mray
{
namespace GUI
{

IMPLEMENT_RTTI(GUIInterfaceScreenImpl, IGUIPanelElement)
 const GUID GUIInterfaceScreenImpl::ElementType="GUIInterfaceScreenImpl";

GUIInterfaceScreenImpl::GUIInterfaceScreenImpl(IGUIManager* m):
IGUIPanelElement(ElementType,m)
{
	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIInterfaceScreen.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}
	SetDocking(EED_Fill);
}

GUIInterfaceScreenImpl::~GUIInterfaceScreenImpl()
{

}


void GUIInterfaceScreenImpl::EnableHand(bool e)
{
	video::ImageRegion* r = m_imageset->GetRegion(e ? "hand":"no_hand");
	if (r && HandIcon)
	{
		HandIcon->SetTargetTexCoords(r->GetTextureRect());
	}
}


void GUIInterfaceScreenImpl::EnableVideo(bool e)
{
	video::ImageRegion* r = m_imageset->GetRegion(e ? "video" : "no_video");
	if (r&& VideoIcon)
	{
		VideoIcon->SetTargetTexCoords(r->GetTextureRect());
	}

}


void GUIInterfaceScreenImpl::EnableMic(bool e)
{
	video::ImageRegion* r = m_imageset->GetRegion(e ? "mic" : "no_mic");
	if (r&&MicIcon)
	{
		MicIcon->SetTargetTexCoords(r->GetTextureRect());
	}

}

void GUIInterfaceScreenImpl::DoneLoadingElements()
{

	m_imageset = ImageSetResourceManager::getInstance().getImageSet("VideoIcons");
	if (!m_imageset)
		return;
	HandIcon->SetImage(m_imageset->GetTexture()->GetTexture());
	VideoIcon->SetImage(m_imageset->GetTexture()->GetTexture());
	MicIcon->SetImage(m_imageset->GetTexture()->GetTexture());

	EnableHand(false);
	EnableVideo(false);
	EnableMic(false);
}


}
}