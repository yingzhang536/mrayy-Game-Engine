

#ifndef IRenderingLayer_h__
#define IRenderingLayer_h__

#include "IGUIPanelElement.h"


namespace mray
{
namespace kmd
{


class IRenderingLayer
{
protected:
	float m_alpha;
	float m_targetAlpha;
public:
	IRenderingLayer() :m_alpha(1), m_targetAlpha(1)
	{}
	virtual ~IRenderingLayer()
	{}


	virtual void InitLayer(GUI::IGUIPanelElement* panel) = 0;
	virtual void UpdateLayer(float dt)
	{
		if (dt > 1)
			dt = 1;
		m_alpha += (m_targetAlpha - m_alpha)*dt;
	}

	virtual void SetVisible(bool v) = 0;
	virtual bool IsVisible() = 0;

	virtual void SetAlpha(float alpha) { m_targetAlpha = alpha; }
	float GetAlpha(){ return m_alpha; }

};

}
}

#endif // IRenderingLayer_h__
