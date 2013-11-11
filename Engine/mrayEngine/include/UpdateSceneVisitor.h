

/********************************************************************
	created:	2010/10/29
	created:	29:10:2010   15:28
	filename: 	i:\Programing\GameEngine\mrayEngine\mrayEngine\include\UpdateSceneVisitor.h
	file path:	i:\Programing\GameEngine\mrayEngine\mrayEngine\include
	file base:	UpdateSceneVisitor
	file ext:	h
	author:		Mohamad Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef UpdateSceneVisitor_h__
#define UpdateSceneVisitor_h__

#include "ISceneVisitor.h"

namespace mray
{
	class ThreadJobManager;
namespace scene
{

class UpdateSceneVisitor:public ISceneVisitor
{
private:
protected:
	float m_dt;
	ThreadJobManager* m_jobsManager;
public:
	UpdateSceneVisitor(float dt,ThreadJobManager* jobsManager);
	virtual ~UpdateSceneVisitor();

	virtual void Visit(ISceneManager*mngr);
	virtual void Visit(ISceneNode*node);
	virtual void Visit(CameraNode*c);
	virtual void Visit(LightNode*l);

	virtual void Visit(IRenderable*r);
	virtual void Visit(IMovable*m);
};

}
}

#endif // UpdateSceneVisitor_h__
