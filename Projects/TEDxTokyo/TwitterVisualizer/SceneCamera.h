

#ifndef SceneCamera_h__
#define SceneCamera_h__

namespace mray
{
namespace scene
{

class SceneCamera
{
protected:

	math::vector2d m_targetTranslation;
	float m_targetScale;
	float m_targetSpeed;

	math::vector2d m_translation;
	float m_scale;

	math::matrix4x4 m_transformation;

	math::rectf m_vp;
	void _UpdateTransformation();
public:
	SceneCamera();
	virtual~SceneCamera();

	void SetViewPort(const math::rectf& vp){ m_vp = vp; }

	void ResetView();
	void FrameBox(const math::rectf& rc);
	void MoveTo(const math::rectf& rc, float speed);

	void ApplyTransformation();
	void Update(float dt);

	void SetTransformation(const math::vector2d& pos, float angle, float scale);

	math::vector2d ConvertToWorldSpace(const math::vector2d& pos);
	math::vector2d ConvertToScreenSpace(const math::vector2d& pos);
};

}
}
#endif // SceneCamera_h__
