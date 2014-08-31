

#ifndef SceneEffectManager_h__
#define SceneEffectManager_h__

#include "ISceneEffect.h"


namespace mray
{
namespace AugTel
{
	
class SceneEffectManager:public ISceneEffect
{
protected:
	struct EffectInfo
	{
		core::string name;
		ISceneEffectPtr effect;
		bool active;
		bool autoStopWhenDone;
	};
	typedef std::map<core::string, EffectInfo> EffectMap;
	EffectMap m_effects;
public:
	SceneEffectManager();
	virtual ~SceneEffectManager();


	void AddEffect(const core::string& name, ISceneEffectPtr effect, bool active = true,bool autoStopWhenDone=false);
	void SetAcive(const core::string& name, bool active);
	ISceneEffectPtr GetEffect(const core::string& name);


	virtual bool IsDone();

	virtual void Init() ;
	virtual void Begin() ;
	virtual video::ITexture* Render(video::ITexture* scene, const math::rectf& vp);

	virtual bool Update(float dt);

	
};

}
}

#endif // SceneEffectManager_h__
