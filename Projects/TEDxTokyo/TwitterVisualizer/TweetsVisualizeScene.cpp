

#include "stdafx.h"
#include "TweetsVisualizeScene.h"

#include "ConnectionLineRenderer.h"
#include "BackgroundEmitter.h"
#include "ViewPort.h"
#include "ParsedShaderPP.h"
#include "TextDecorator.h"

#include "CPUParticleBillboardRenderer.h"
#include "GPUParticleBillboardRenderer.h"
#include "ICustomParticleEmitter.h"

#include "PAVelocityInterpolater.h"
#include "PAVelocityOverTimeInterpolater.h"
#include "SphereParticleEmitter.h"


#include "IGUIListBox.h"
#include "IGUIEditBox.h"
#include "GUIManager.h"

#include "TwitterProfileListItem.h"
#include "GUIScreenLayoutImplV1.h"

#include "GUIUserProfile.h"
#include "GUITweetItem.h"

#include "TweetsEmitter.h"
#include "TweetParticle.h"

#include "LeapDevice.h"

#include "StringUtil.h"

#include "TwitterUserProfile.h"
#include "TwitterTweet.h"

#include "TextureResourceManager.h"

#include "GUIOverlayManager.h"
#include "GUIOverlay.h"

#include "SceneManager.h"
#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"

#include "GUIThemeManager.h"
#include "AppData.h"
#include "LeapDevice.h"

namespace mray
{


namespace scene
{
	class ParticleGridRegion
	{
		ParticleGridRegion* m_neighboors[6];
		std::list<IParticle*> m_particles;
	public:
		ParticleGridRegion()
		{}

		ParticleGridRegion* Up(){ return m_neighboors[0]; }
		ParticleGridRegion* Bot(){ return m_neighboors[1]; }
		ParticleGridRegion* Left(){ return m_neighboors[2]; }
		ParticleGridRegion* Right(){ return m_neighboors[3]; }
		ParticleGridRegion* Front(){ return m_neighboors[4]; }
		ParticleGridRegion* Back(){ return m_neighboors[5]; }

		void clear(){
			m_particles.clear();
		}
		void AddParticle(IParticle* p)
		{
			m_particles.push_back(p);
		}
		const std::list<IParticle*>& GetParticles(){ return m_particles; }
	};
	class ParticleGrid
	{
		math::vector3d m_size;
		math::vector3di m_resolution;

		ParticleGridRegion* m_cells;
	public:
		ParticleGrid()
		{
			m_cells = 0;
			SetResolution(1);
		}
		void SetSize(const math::vector3d &size)
		{
			m_size = size;
		}
		void SetResolution(const math::vector3di& res)
		{
			m_resolution = res;
			int count = m_resolution.x*m_resolution.y*m_resolution.z;
			delete[]m_cells;
			m_cells = new ParticleGridRegion[count];
		}
		int getIndex(const math::vector3d& cell)
		{
			return cell.x + (cell.z + cell.y*m_resolution.z)*m_resolution.x;
		}
		void InsertParticle(IParticle* p)
		{
			math::vector3di cell = (p->position + m_size / 2) / m_size;
			if (cell.x >= m_resolution.x || cell.y >= m_resolution.y || cell.z >= m_resolution.z
				|| cell.x < 0 || cell.y < 0 || cell.z < 0)
			{
				return;
			}
			int idx = getIndex(cell);
			m_cells[idx].AddParticle(p);
		}
		ParticleGridRegion* GetCells(){ return m_cells; }
		int GetCellsCount(){ return m_resolution.x*m_resolution.y*m_resolution.z; }

		void Reset()
		{
			int count = GetCellsCount();
			for (int i = 0; i < count; ++i)
			{
				m_cells[i].clear();
			}
		}
	};
	class  PACenterForce :public IParticleAffector
	{
	public:

		float maxRadius;

		PACenterForce() :maxRadius(50)
		{
		}
		virtual~PACenterForce(){}

		virtual bool affectsTransformations(){ return true; }

		virtual void applyOnParticle(IParticle*p, float dt)
		{
			math::vector3d dir = p->position;
			float len = dir.Normalize();
			if (len > maxRadius)
			{
				p->velocity += -dir*dt;
			}

		}

	};

	class CenterParticleLineRenderer :public ConnectionLineRenderer
	{
		scene::ParticleEmitter* m_emitter;
	public:
		CenterParticleLineRenderer()
		{

		}

		void SetEmitter(scene::ParticleEmitter* emitter){ m_emitter = emitter; }


		virtual void update(float dt)
		{
			if (!m_emitter)
				return;
			const scene::ParticleEmitter::ParticleList& particles = m_emitter->getParticles();
			math::vector3d pos = m_emitter->GetSystem()->getOwner()->getAbsolutePosition();
			Reset();
			scene::ParticleEmitter::ParticleList::const_iterator it = particles.begin();
			for (ushort i = 0; it != particles.end(); ++it, i += 2)
			{
				AddConnection(pos, (*it)->position, (*it)->color, video::SColor(1, 1, 1, 0));
			}
			ConnectionLineRenderer::update(dt);
		}
	};
	class DistanceParticleLineRenderer :public ConnectionLineRenderer
	{
		scene::ParticleEmitter* m_emitter;
		ParticleGrid m_grid;
	public:
		DistanceParticleLineRenderer()
		{
			m_grid.SetSize(math::vector3d(40));
			m_grid.SetResolution(10);

		}

		void SetEmitter(scene::ParticleEmitter* emitter){ m_emitter = emitter; }


		virtual void update(float dt)
		{
			if (!m_emitter)
				return;
			const scene::ParticleEmitter::ParticleList& particles = m_emitter->getParticles();
			math::vector3d pos = m_emitter->GetSystem()->getOwner()->getAbsolutePosition();

			m_grid.Reset();
			//m_grid.SetSize(m_emitter->GetBoundingBox().getExtent() * 2);

			Reset();
			scene::ParticleEmitter::ParticleList::const_iterator it = particles.begin();
			for (ushort i = 0; it != particles.end(); ++it, i += 2)
			{
				m_grid.InsertParticle(*it);
			}
			int c = m_grid.GetCellsCount();
			for (int i = 0; i < c; ++i)
			{
				const std::list<IParticle*> &r = m_grid.GetCells()[i].GetParticles();
				std::list<IParticle*>::const_iterator it = r.begin();
				for (int j = 0; j < ((int)r.size()) - 1 /*&& j<40*/; j += 2)
				{
					IParticle* a = dynamic_cast<IParticle*>(*it);
					IParticle* b = 0;
					std::list<IParticle*>::const_iterator itBest;
					std::list<IParticle*>::const_iterator it2 = it;
					float bestDist = 9999999;
					++it2;
					for (int j2 = 0; it2 != r.end() /*&& j2 < 20*/; ++it2, ++j2)
					{
						float d = a->position.getDistSQ((*it2)->position);
						if (d < bestDist)
						{
							itBest = it2;
							bestDist = d;
							b = dynamic_cast<IParticle*>(*it2);
						}
					}
					if (!b)
						break;
					++it;

					if (bestDist > 4)
						continue;

					bestDist = sqrtf(bestDist);
					//	if (a->GetLevelID() == b->GetLevelID())
					{
						video::SColor clr(1);
						clr.A = bestDist / 5.0f;
						clr.A = 0.5f + math::clamp<float>(clr.A, 0, 1)*0.5f;
						AddConnection(a->position, b->position, clr, clr);
					}
				}
			}

			ConnectionLineRenderer::update(dt);
		}

	};
}

namespace ted
{

class EditboxFocusListener :public GUI::IGUIManagerListener
{
public:
	GUI::IGUIEditBox* editbox;
	EditboxFocusListener() :editbox(0)
	{}
	virtual void OnChangeGUIFocusElement(GUI::IGUIManager* m, GUI::IGUIElement* e)
	{
		if (editbox == 0)
			return;
		if (e == editbox)
			return;
	//	m->SetFocus(editbox);
	}
	virtual void onEvent(Event* e)
	{}

} g_editboxFocuslistener;

//////////////////////////////////////////////////////////////////////////

class TweetsVisualizeSceneImpl:public nui::ILeapListener
{
public:

	GCPtr<scene::SParticleSystem> m_particleSystem;

	scene::CameraNode* m_camera;
	math::vector3d m_orbitAngles;

	GUI::IGUIPanelElement* m_guiroot;
	GUI::GUIScreenLayoutImplV1* m_screenLayout;

	std::vector<GUI::GUITweetItem*> m_tweetItems;

	scene::TweetsEmitter* m_tweetEmitter;
	std::vector<scene::TweetParticle*> m_loadedParticles;

	math::vector3d m_speedVec;
	math::vector3d m_leapVec;
	float m_cameraRadius;

	GUI::GUIManager* m_guiMngr;

	scene::ISceneManager* m_sceneManager;

	TweetsVisualizeScene* m_owner;
	scene::ViewPort* m_vp;

public:
	TweetsVisualizeSceneImpl(TweetsVisualizeScene* o);
	~TweetsVisualizeSceneImpl();


	void Init();

	virtual void OnEnter();
	virtual void OnExit();

	virtual bool OnEvent(Event* e, const math::rectf& rc);
	virtual void Update(float dt);
	virtual video::IRenderTarget* Draw(const math::rectf& rc);


	void _OnUserNameChange(IObject* sender, PVOID param);
	void _OnSelectUserChange(IObject* sender, PVOID param);

	void OnGesture(nui::LeapDevice* d, nui::ELeapGestureType type, Leap::Gesture g);
};
TweetsVisualizeSceneImpl::TweetsVisualizeSceneImpl(TweetsVisualizeScene* o)
{
	m_owner = o;
}
TweetsVisualizeSceneImpl::~TweetsVisualizeSceneImpl()
{

}

void TweetsVisualizeSceneImpl::Init()
{
	video::IVideoDevice* dev = Engine::getInstance().getDevice();
	m_guiMngr = new GUI::GUIManager(dev);
	m_sceneManager = new scene::SceneManager(dev);


	m_camera = m_sceneManager->createCamera();
	m_camera->setAutoUpdateAspect(true);
	m_camera->setUseTarget(true);
	m_camera->setTarget(0);
	m_camera->setPosition(math::vector3d(10));
	{
		video::IRenderTargetPtr rt = dev->createRenderTarget("", dev->createTexture2D(1, video::EPixel_R8G8B8A8, false), 0, 0, 0);
		m_vp = new scene::ViewPort("", m_camera, rt,0, math::rectf(0, 0, 1, 1), 0);
		m_vp->enablePostProcessing(true);

		video::ParsedShaderPP* pp = new video::ParsedShaderPP(dev);

		pp->LoadXML(gFileSystem.openFile("blur.peff"));
		m_vp->setPostProcessing(pp);
	}
	m_particleSystem = new scene::SParticleSystem();

	scene::ISceneNode* psNode = m_sceneManager->createSceneNode("Particle System");

	video::RenderMaterialPtr partMaterial = gMaterialResourceManager.getMaterial("CPUParticlesMaterial")->Duplicate();
	partMaterial->GetTechniqueAt(0)->GetPassAt(0)->setTexture(gTextureResourceManager.loadTexture2D("LightParticle.png"), 0);
	if (false)
	{
		for (int i = 0; i < 2; ++i)
		{
			scene::BackgroundEmitter* be = new scene::BackgroundEmitter();
			m_particleSystem->AddEmitter(be);

			be->SetRenderer(new scene::CPUParticleBillboardRenderer(1000));
			be->setMaterial(partMaterial);

			scene::PAVelocityOverTimeInterpolater* vel = new scene::PAVelocityOverTimeInterpolater();
			be->addAffector(vel);
			//vel->addKey(0, -0.1, 0.1);
			vel->SetMinMaxVel(-1, 1);

			scene::PACenterForce* centerForce = new scene::PACenterForce();
			be->addAffector(centerForce);

			scene::DistanceParticleLineRenderer* lrend = new scene::DistanceParticleLineRenderer();
			lrend->SetEmitter(be);
			lrend->setMaterial(gMaterialResourceManager.getMaterial("ParticleLinesMaterial"), 0);
			psNode->AttachNode(lrend);
		}
	}
	{
		scene::TweetsEmitter* e = new scene::TweetsEmitter();
		m_tweetEmitter = e;

		m_particleSystem->AddEmitter(e);

		e->SetRenderer(new scene::CPUParticleBillboardRenderer(10000));
		e->setMaterial(gMaterialResourceManager.getMaterial("CPUParticlesMaterial"));


		scene::CenterParticleLineRenderer* lrend = new scene::CenterParticleLineRenderer();
		lrend->SetEmitter(e);
		lrend->setMaterial(gMaterialResourceManager.getMaterial("ParticleLinesMaterial"), 0);


		e->particlesCount.addKey(0, -1);
		e->particlesLifeSpawn.addKey(0, -1);
		e->particlesRate.addKey(0, 300);

		psNode->AttachNode(lrend);
	}
	psNode->AttachNode(m_particleSystem);



	m_guiMngr->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());

	m_guiroot = (GUI::IGUIPanelElement*)new GUI::IGUIPanelElement(core::string(""), m_guiMngr);
	m_guiroot->SetDocking(GUI::EED_Fill);
	m_guiMngr->SetRootElement(m_guiroot);

	{
		GUI::GUIOverlay* screenOverlay = GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIScreenLayout_V1.gui");
		m_screenLayout = new GUI::GUIScreenLayoutImplV1();
		screenOverlay->CreateElements(m_guiMngr, m_guiroot, 0, m_screenLayout);

		g_editboxFocuslistener.editbox = m_screenLayout->UserTxt;
		m_guiMngr->AddListener(&g_editboxFocuslistener);

		m_guiMngr->SetFocus(m_screenLayout->UserTxt);
		m_screenLayout->UserTxt->OnTextChange += CreateObjectDelegate(TweetsVisualizeSceneImpl, this, _OnUserNameChange);
		m_screenLayout->UsersList->OnSelectChange += CreateObjectDelegate(TweetsVisualizeSceneImpl, this, _OnSelectUserChange);
		m_screenLayout->RootStack->SetUseScroll(false);
	}

}



bool _UsersSortFunc(const ted::TwitterUserProfile* a, const ted::TwitterUserProfile* b)
{
	if (a->displayName.compare_ignore_case(b->displayName) == -1)
		return false;
	else return true;
}
void TweetsVisualizeSceneImpl::_OnUserNameChange(IObject* sender, PVOID param)
{
	if (!sender)
		return;
	core::stringw str = dynamic_cast<GUI::IGUIEditBox*>(sender)->GetText().GetAsStringW();
	m_screenLayout->UsersList->ClearItems();
	printf("cleared\n");
	if (str == L"")
	{
		m_screenLayout->UserProfile->SetUser(0);
		for (int i = 0; i < m_tweetItems.size(); ++i)
			m_tweetItems[i]->SetVisible(false);
		gTextureResourceManager.unloadAll(true);
		return;
	}

	ted::TwitterUserProfile* firstUser = 0;

	std::vector<ted::TwitterUserProfile*> users;
	for (int i = 0; i < ted::TwitterUserProfile::TwitterUserProfileList.size(); ++i)
	{
		ted::TwitterUserProfile* u = ted::TwitterUserProfile::TwitterUserProfileList[i];
		if (core::StringUtilW::BeginsWith(u->displayName, str))
		{
			users.push_back(u);
		}
	}
	if (!users.size())
		return;
	std::sort(users.begin(), users.end(), _UsersSortFunc);
	for (int i = 0; i < users.size(); ++i)
	{
		if (!firstUser)
			firstUser = users[i];
		m_screenLayout->UsersList->AddItem(new GUI::TwitterProfileListItem(users[i]));
	}

	if (firstUser)
	{
	}
	m_screenLayout->UsersList->SetSelectedItem(0);
	printf("items added : %d\n", m_screenLayout->UsersList->GetItemsCount());

}
void TweetsVisualizeSceneImpl::_OnSelectUserChange(IObject* sender, PVOID param)
{
	if (!sender)
		return;
	GUI::IGUIListBox* lstBox = dynamic_cast<GUI::IGUIListBox*>(sender);
	int i = lstBox->GetSelectedItem();
	if (i == -1)
		return;
	GUI::TwitterProfileListItem* item = dynamic_cast<GUI::TwitterProfileListItem*>(lstBox->GetItem(i));
	if (!item)
		return;
	ted::TwitterUserProfile* u = item->GetUser();
	for (int i = 0; i < m_loadedParticles.size(); ++i)
	{
		m_loadedParticles[i]->scale = 0.1;
		m_loadedParticles[i]->color = 1;
	}
	m_loadedParticles.clear();
	m_screenLayout->UserProfile->SetUser(u);
	for (int i = 0; i < u->tweets.size(); ++i)
	{
		scene::TweetParticle* p = m_tweetEmitter->GetTweetParticle(u->tweets[i]->ID);
		if (!p)
			continue;;
		p->color.Set(1, 0, 0, 1);
		p->scale = 0.8;
		m_loadedParticles.push_back(p);
	}

	for (int i = 0; i < m_tweetItems.size(); ++i)
		m_tweetItems[i]->SetVisible(false);
	for (int i = 0; i < u->tweets.size(); ++i)
	{
		GUI::GUITweetItem *item = 0;
		if (m_tweetItems.size()>i)
		{
			item = m_tweetItems[i];
			item->SetVisible(true);
		}
		else
		{
			item = new GUI::GUITweetItem(m_guiMngr);
			m_screenLayout->TweetsList->AddElement(item);
			item->SetSize(math::vector2d(500, 150));
			m_tweetItems.push_back(item);
		}
		item->SetTweet(u->tweets[i]);

	}
	m_screenLayout->TweetsList->SetScrollOffset(0);
	printf("Tweets count: %d\n", item->GetUser()->tweets.size());
}

void TweetsVisualizeSceneImpl::OnEnter()
{
	gAppData.leapDevice->AddListener(this);
}
void TweetsVisualizeSceneImpl::OnExit()
{

	gAppData.leapDevice->RemoveListener(this);
}

bool TweetsVisualizeSceneImpl::OnEvent(Event* e, const math::rectf& rc)
{
	return m_guiMngr->OnEvent(e,&rc);
}
void TweetsVisualizeSceneImpl::Update(float dt)
{

	m_sceneManager->update(dt);
	m_guiMngr->Update(dt);

	m_orbitAngles.x += m_speedVec.x*dt;
	m_orbitAngles.y += m_speedVec.z*dt;

	m_cameraRadius += m_speedVec.y*dt*0.1f;

	m_orbitAngles.y = math::clamp<float>(m_orbitAngles.y, -70, 70);

	m_speedVec += m_leapVec*dt;
	m_speedVec -= m_speedVec*dt;

	float offset = m_screenLayout->TweetsList->GetScrollOffset();
	offset += m_speedVec.z*dt * 2;
	m_screenLayout->TweetsList->SetScrollOffset(offset);

	m_cameraRadius = math::clamp<float>(m_cameraRadius, 40, 80);

	math::vector3d pos;
	pos.x = m_cameraRadius * math::cosd(m_orbitAngles.x)* math::cosd(m_orbitAngles.y);
	pos.y = m_cameraRadius * math::sind(m_orbitAngles.y);
	pos.z = m_cameraRadius * math::sind(m_orbitAngles.x)* math::cosd(m_orbitAngles.y);
	m_camera->setPosition(pos);
}
video::IRenderTarget* TweetsVisualizeSceneImpl::Draw(const math::rectf& rc)
{
	Engine::getInstance().getDevice()->setRenderTarget(m_owner->GetRenderTarget());
	m_guiMngr->DrawAll(&rc);
	return m_owner->GetRenderTarget();
}

void TweetsVisualizeSceneImpl::OnGesture(nui::LeapDevice* d, nui::ELeapGestureType type, Leap::Gesture g)
{
	if (type == nui::ELeapGestureType::Swipe)
	{
		Leap::SwipeGesture swipe = g;
		if (swipe.state() == Leap::Gesture::STATE_UPDATE)
		{
			float speed = swipe.speed();
			math::vector3d dir(swipe.direction().x, swipe.direction().y, swipe.direction().z);
			m_leapVec = dir*speed;
		}
		else
			m_leapVec = 0;
	}
}
//////////////////////////////////////////////////////////////////////////

TweetsVisualizeScene::TweetsVisualizeScene()
{
	m_impl = new TweetsVisualizeSceneImpl(this);
}
TweetsVisualizeScene::~TweetsVisualizeScene()
{
	delete m_impl;
}

void TweetsVisualizeScene::Init()
{
	IRenderingScene::Init();
	m_impl->Init();
}
void TweetsVisualizeScene::OnEnter()
{
	m_impl->OnEnter();
}
void TweetsVisualizeScene::OnExit()
{
	m_impl->OnExit();
}

bool TweetsVisualizeScene::OnEvent(Event* e, const math::rectf& rc)
{
	return m_impl->OnEvent(e,rc);
}
void TweetsVisualizeScene::Update(float dt)
{
	 m_impl->Update(dt);
}
video::IRenderTarget* TweetsVisualizeScene::Draw(const math::rectf& rc)
{
	IRenderingScene::Draw(rc);
	return m_impl->Draw(rc);
}

}
}


