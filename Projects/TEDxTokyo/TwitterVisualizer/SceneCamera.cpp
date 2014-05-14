
#include "stdafx.h"
#include "SceneCamera.h"



namespace mray
{
namespace scene
{


SceneCamera::SceneCamera()
{
	m_scale = 1;
	m_targetScale = 1;
	m_targetSpeed = 1;
}

SceneCamera::~SceneCamera()
{
}


void SceneCamera::ResetView()
{
	m_targetTranslation = 0;
	m_targetScale = 1;
}

void SceneCamera::FrameBox(const math::rectf& rc)
{
	math::vector2d sz = m_vp.getSize()/rc.getSize();
	m_targetScale = math::Min(sz.x, sz.y);
	m_targetTranslation = (m_vp.ULPoint - rc.ULPoint) *m_targetScale;
}

void SceneCamera::MoveTo(const math::rectf& rc, float speed)
{
}


void SceneCamera::ApplyTransformation()
{
	_UpdateTransformation();
	Engine::getInstance().getDevice()->setTransformationState(video::TS_WORLD, m_transformation);
}
void SceneCamera::Update(float dt)
{
	math::vector3d dir (m_targetTranslation.x - m_translation.x, m_targetTranslation.y - m_translation.y, m_targetScale - m_scale);
	float len = dir.Normalize();
	m_translation.x += len*dir.x*m_targetSpeed*dt;
	m_translation.y += len*dir.y*m_targetSpeed*dt;
	m_scale += len*dir.z*m_targetSpeed*dt;

}
void SceneCamera::SetTransformation(const math::vector2d& pos, float angle, float scale)
{
	m_translation = pos;
	m_scale = scale;
}

void SceneCamera::_UpdateTransformation()
{
	m_transformation.makeIdentity();
	m_transformation.setTranslation(math::vector3d(m_translation.x, m_translation.y, 0));
	m_transformation.rotateZ(0);
	if (m_scale != 1)
	{
		math::matrix4x4 o;
		o.setScale(math::vector3d(m_scale, m_scale, 1));
		m_transformation = m_transformation*o;
	}
}

math::vector2d SceneCamera::ConvertToWorldSpace(const math::vector2d& pos)
{
	//math::vector3d p = m_transformation.inverseTransformVector(math::vector3d(pos.x, pos.y, 0));
	math::vector2d p;
	p = (pos - m_translation) / m_scale;
	return math::vector2d(p.x, p.y);
}
math::vector2d SceneCamera::ConvertToScreenSpace(const math::vector2d& pos)
{
	math::vector3d p = m_transformation*math::vector3d(pos.x, pos.y, 0);
	return math::vector2d(p.x, p.y);
}

}
}
