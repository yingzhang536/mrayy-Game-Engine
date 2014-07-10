

#include "stdafx.h"
#include "GenericRenderLayer.h"
#include "GUIManager.h"



namespace mray
{
namespace kmd
{


GenericRenderLayer::GenericRenderLayer()
{
	m_guiPanel = 0;
}


GenericRenderLayer::~GenericRenderLayer()
{
}



void GenericRenderLayer::InitLayer(GUI::IGUIPanelElement* panel)
{
	m_guiPanel = panel;
}


void GenericRenderLayer::UpdateLayer(float dt)
{
	IRenderingLayer::UpdateLayer(dt);
	if (m_guiPanel)
		m_guiPanel->SetAlpha(GetAlpha());
}


}
}

