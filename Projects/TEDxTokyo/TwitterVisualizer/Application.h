

/********************************************************************
	created:	2014/03/24
	created:	24:3:2014   1:38
	filename: 	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer\Application.h
	file path:	C:\Development\mrayEngine\Projects\TEDxTokyo\TwitterVisualizer
	file base:	Application
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __Application__
#define __Application__


#include <cmrayapplication.h>
#include "DynamicFontGenerator.h"
#include "ISQLConnection.h"
#include "ISQLManager.h"
#include "IGUIListBox.h"
#include "LeapDevice.h"

namespace mray
{
	namespace scene
	{
		class TweetsEmitter;
		class TweetParticle;
	}

	namespace GUI
	{
		class GUIUserProfile;
		class IGUIEditBox;
		class GUITweetItem;
		class IGUIStackPanel;
	}
class Application :public CMRayApplication,public nui::ILeapListener
{
protected:
	GCPtr<GUI::DynamicFontGenerator> m_fontGen;

	GCPtr<scene::SParticleSystem> m_particleSystem;

	GCPtr<db::ISQLManager> m_dbManager;
	GCPtr<db::ISQLConnection> m_dbConnection;

	scene::CameraNode* m_camera;
	math::vector3d m_orbitAngles;

	video::ITexture* m_loadedTexture;

	std::vector < video::ITexturePtr> m_images;

	GUI::IGUIPanelElement* m_guiroot;
	GUI::IGUIEditBox* m_userNameTxt;
	GUI::IGUIListBox* m_usersList;
	scene::TweetsEmitter* m_tweetEmitter;
	GUI::GUIUserProfile* m_userProfile;

	GUI::IGUIStackPanel* m_tweetsStack;
	std::vector<GUI::GUITweetItem*> m_tweetItems;

	std::vector<scene::TweetParticle*> m_loadedParticles;

	GCPtr<nui::LeapDevice> m_leap;

	math::vector3d m_speedVec;
	math::vector3d m_leapVec;
	float m_cameraRadius;

	void _OnUserNameChange(IObject* sender, PVOID param);
	void _OnSelectUserChange(IObject* sender, PVOID param);
public:
	Application();
	virtual~Application();

	virtual void init(const OptionContainer &extraOptions);

	virtual void update(float dt);

	virtual void WindowPostRender(video::RenderWindow* wnd);

	virtual void OnGesture(nui::LeapDevice* d, nui::ELeapGestureType type, Leap::Gesture g);

};

}


#endif
