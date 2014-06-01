
#include "stdafx.h"
#include "PointerNode.h"
#include "TextureResourceManager.h"


namespace mray
{
namespace scene
{



PointerNode::PointerNode()
{
	m_alpha = 0;
	m_isVisible = false;
	m_angle = 0;
}
PointerNode::~PointerNode()
{
}

void PointerNode::SetPosition(const math::vector2d&pos)
{
	m_pos = pos;
	m_targetPos = pos;
}
void PointerNode::MoveBy(const math::vector2d&dv)
{
	m_targetPos = m_pos + dv;
}
const math::vector2d& PointerNode::GetPosition()
{
	return m_pos;
}

void PointerNode::Hide()
{
	m_isVisible = false;
}
void PointerNode::Show()
{
	m_isVisible = true;
}

void PointerNode::Draw()
{
	if (m_alpha == 0)
		return;
	math::rectf rc;

	rc.ULPoint = m_pos - 10;
	rc.BRPoint = m_pos + 10;
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	video::TextureUnit tex;
	tex.SetTexture(gTextureResourceManager.loadTexture2D("Pointer.png"));
	dev->useTexture(0, 0);
	dev->setLineWidth(3);
	//dev->draw2DLine(m_pos, m_targetPos, 1);
	dev->useTexture(0, &tex);
	dev->draw2DImageRot(rc, rc.getSize()/2, video::SColor(1,1,1,m_alpha), m_angle);
}
void PointerNode::Update(float dt)
{
	if (m_isVisible)
	{
		m_alpha = math::Min(1.0f, m_alpha + dt);
	}
	else
	{
		m_alpha = math::Max(0.0f, m_alpha - dt);
	}

	float len = m_targetPos.getDist(m_pos);
	if (len == 0)len = 1;
	float l = math::Min<float>(len, 400 * dt)/len;

	m_pos += (m_targetPos - m_pos)*l;
	m_angle += 400 * dt;
}

}
}

