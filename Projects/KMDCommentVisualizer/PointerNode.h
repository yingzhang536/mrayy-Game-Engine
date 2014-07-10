

#ifndef PointerNode_h__
#define PointerNode_h__

namespace mray
{
namespace scene
{



class PointerNode
{
protected:
	bool m_isVisible;
	math::vector2d m_pos;
	math::vector2d m_targetPos;
	float m_alpha;
	float m_angle;
	float m_lastMoveTime;
public:
	PointerNode();
	virtual~PointerNode();

	void SetPosition(const math::vector2d&pos);
	void MoveBy(const math::vector2d&dv);
	const math::vector2d& GetPosition();

	bool IsVisible(){ return m_isVisible; }

	void Hide();
	void Show();

	virtual void Draw();
	virtual void Update(float dt);

};

}
}

#endif // PointerNode_h__
