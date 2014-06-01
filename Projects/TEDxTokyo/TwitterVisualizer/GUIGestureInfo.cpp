

#include "stdafx.h"
#include "GUIGestureInfo.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"
#include "ImageSetResourceManager.h"

namespace mray
{
namespace GUI
{

	const GUID GUIGestureInfo::ElementType = "GUIGestureInfo";

	const core::string gestureImageInfo[] =
	{
		"",
		"Swipe",
		"Tap",
		"Circle",
		"Clapping",
		"InOut_DoubleSwipe",
		"Keytap",
		"OpenClose",
		"Push_Pull",
		"SteeringWheel",
		"FingerCount"
	};

	const core::string gestureImageTitle[] =
	{
		"",
		"Swipe",
		"Tap",
		"Circle",
		"Clapping",
		"In/Out Double Swipe",
		"Keytap",
		"Open/Close",
		"Push/Pull",
		"Steering Wheel",
		"Finger Count"
	};

GUIGestureInfo::GUIGestureInfo(IGUIManager* m)
	:IGUIPanelElement(ElementType,m)
{
	m_type = &ElementType;

	GUI::GUIOverlay* o = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIGestureElementLayout.GUI");
	if (o)
	{
		o->CreateElements(m, this, this, this);
	}

	m_imageset = ImageSetResourceManager::getInstance().loadImageSet("leapGestures.imageset");
	if (m_imageset)
		GestureImage->SetImage(m_imageset->GetTexture()->GetTexture());

	SetGesture(EGestureType::Swipe);
	GestureImage->SetStretchMode(GUI::EImage_Center);
}
GUIGestureInfo::~GUIGestureInfo()
{

}

void GUIGestureInfo::SetGesture(EGestureType type)
{
	if (m_gesture == type)
		return;
	m_alpha = 0;
	m_time = 0;
	m_gesture = type;

	video::ImageRegion* r= m_imageset->GetRegion(gestureImageInfo[(int)type]);
	if (r)
	{
		GestureImage->SetTargetTexCoords(r->GetTextureRect());
	}

	GestureName->SetText(gestureImageTitle[(int)type]);
}

void GUIGestureInfo::Update(float dt)
{
	IGUIPanelElement::Update(dt);

	float a = 0;
	if (m_alpha < 1)
	{
		m_alpha = math::Min(1.0f, m_alpha + dt);
	}
	a = m_alpha*abs(sin(m_time));
	GestureImage->SetAlpha(a);
	GestureName->SetAlpha(a);
	m_time += dt;
}



IMPLEMENT_RTTI(GUIGestureInfo, IGUIPanelElement);
IMPLEMENT_ELEMENT_FACTORY(GUIGestureInfo);

}
}


