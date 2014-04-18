#include "stdafx.h"

#include "ViewPort2D.h"
#include "IVideoDevice.h"
#include "IGUIElement.h"

namespace mray{
namespace GUI{


ViewPort2D::ViewPort2D(IGUIElement*elem,const video::IRenderTargetPtr&rt){
	m_elem=elem;
	m_rt=rt;
}
ViewPort2D::~ViewPort2D(){
}

void ViewPort2D::draw(){
	/*
	if(!m_elem || !m_rt)return;
	video::IVideoDevice*dev=m_elem->getDevice();
	if(!dev)return;

	math::vector2di oldPos=m_elem->getPos();
	m_elem->MainPos=0;

	dev->set2DMode();
	dev->setRenderTarget(m_rt);
	m_elem->draw(0);
	dev->setRenderTarget(0);
	m_elem->MainPos=oldPos;*/
}

IGUIElement* ViewPort2D::getElement(){
	return m_elem;
}
const video::IRenderTargetPtr& ViewPort2D::getRenderTarget(){
	return m_rt;
}

void ViewPort2D::setElement(IGUIElement* elem){
	m_elem=elem;
}
void ViewPort2D::setRenderTarget(const video::IRenderTargetPtr& rt){
	m_rt=rt;
}


}
}