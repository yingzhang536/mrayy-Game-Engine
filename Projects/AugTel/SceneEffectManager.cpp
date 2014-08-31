

#include "stdafx.h"
#include "SceneEffectManager.h"



namespace mray
{
namespace AugTel
{

SceneEffectManager::SceneEffectManager()
{

}
SceneEffectManager::~SceneEffectManager()
{

}


void SceneEffectManager::AddEffect(const core::string& name, ISceneEffectPtr effect, bool active, bool autoStopWhenDone)
{
	if (m_effects.find(name) != m_effects.end())
		return;
	EffectInfo ifo;
	ifo.name = name;
	ifo.effect = effect;
	ifo.active = active;
	ifo.autoStopWhenDone = autoStopWhenDone;
	m_effects[name] = ifo;;

}
void SceneEffectManager::SetAcive(const core::string& name, bool active)
{
	EffectMap::iterator it = m_effects.find(name);
	if (it == m_effects.end())
		return;
	it->second.active = active;
}
ISceneEffectPtr SceneEffectManager::GetEffect(const core::string& name)
{
	EffectMap::iterator it = m_effects.find(name);
	if (it == m_effects.end())
		return 0;
	return it->second.effect;
}

bool SceneEffectManager::IsDone()
{
	bool done = true;
	EffectMap::iterator it = m_effects.begin();
	for (; it != m_effects.end(); ++it)
	{
		if (it->second.active)
		{
			done &= it->second.effect->IsDone();
		}
	}
	return done;

}

void SceneEffectManager::Init()
{

	EffectMap::iterator it = m_effects.begin();
	for (; it != m_effects.end();++it)
	{
		it->second.effect->Init();
	}
}
void SceneEffectManager::Begin()
{
	EffectMap::iterator it = m_effects.begin();
	for (; it != m_effects.end(); ++it)
	{
		it->second.effect->Begin();
	}
}
video::ITexture* SceneEffectManager::Render(video::ITexture* scene, const math::rectf& vp)
{
	video::ITexture* last = scene;

	EffectMap::iterator it = m_effects.begin();
	for (; it != m_effects.end(); ++it)
	{
		if (it->second.active)
		{
			last=it->second.effect->Render(last, vp);
		}
	}
	return last;
}

bool SceneEffectManager::Update(float dt)
{
	bool done = true;
	EffectMap::iterator it = m_effects.begin();
	for (; it != m_effects.end(); ++it)
	{
		if (it->second.active)
		{
			bool ret=it->second.effect->Update(dt);
			done &= ret;
			if (ret && it->second.autoStopWhenDone)
				it->second.active = false;
		}
	}
	return done;
}

}
}


