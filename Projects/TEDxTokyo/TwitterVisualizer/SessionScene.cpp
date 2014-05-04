

#include "stdafx.h"
#include "SessionScene.h"



namespace mray
{
namespace ted
{



SessionScene::SessionScene()
{
}

SessionScene::~SessionScene()
{
}



void SessionScene::Init()
{
}

void SessionScene::OnEnter()
{
}

void SessionScene::OnExit()
{
}


bool SessionScene::OnEvent(Event* e, const math::rectf& rc)
{
	return false;
}

void SessionScene::Update(float dt)
{
}

video::IRenderTarget* SessionScene::Draw(const math::rectf& rc)
{
	IRenderingScene::Draw(rc);

	return GetRenderTarget();
}


}
}
