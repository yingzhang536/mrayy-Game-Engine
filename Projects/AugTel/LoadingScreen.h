


#ifndef LoadingScreen_h__
#define LoadingScreen_h__

#include "IGUIManager.h"
#include "GUILoadingScreen.h"

namespace mray
{
namespace AugTel
{
	class VideoRenderElement;
	
class LoadingScreen
{
protected:
	VideoRenderElement* m_video;
	GUI::IGUIManager* m_guiManager;
	GUI::IGUIPanelElement* m_guiroot;
	GUI::GUILoadingScreen* m_screenLayout;
	bool m_done;
public:
	LoadingScreen();
	virtual ~LoadingScreen();

	void Init();

	void Start();

	void Draw(const math::rectf& rc);

	void Update(float dt);

	void End();

	bool IsDone();

	
};

}
}


#endif // LoadingScreen_h__
