
#include "stdafx.h"
#include "Application.h"


#include "GUIBatchRenderer.h"
#include "CPUParticleBillboardRenderer.h"
#include "GPUParticleBillboardRenderer.h"
#include "ICustomParticleEmitter.h"

#include "PAVelocityInterpolater.h"
#include "PAVelocityOverTimeInterpolater.h"
#include "SphereParticleEmitter.h"


#include "win32NetInterface.h"
#include "IWebRequest.h"

#include "TextureResourceManager.h"
#include "mySqlManager.h"

#include "ConnectionLineRenderer.h"
#include "BackgroundEmitter.h"
#include "ViewPort.h"
#include "ParsedShaderPP.h"
#include "TextDecorator.h"

#include "TweetDB.h"
#include "UserDB.h"

#include "GUIThemeManager.h"
#include "IGUIPanelElement.h"
#include "IGUIListBox.h"
#include "IGUIEditBox.h"
#include "GUIManager.h"

#include "InternetCacheManager.h"
#include "TwitterProfileListItem.h"
#include "FontResourceManager.h"
#include "DBHandler.h"
#include "TweetsEmitter.h"
#include "TweetParticle.h"
#include "StringUtil.h"
#include "GUIUserProfile.h"
#include "GUITweetItem.h"
#include "IGUIStackPanel.h"
#include "LeapDevice.h"
#include "GUIScreenLayoutImpl.h"
#include "GUIOverlayManager.h"
#include "GUIElementFactory.h"
#include "GUIOverlay.h"
#include "RenderTechnique.h"
#include "RenderPass.h"

#include "GUISessionSidePanel.h"
#include "GUISessionDetailsTopPanel.h"
#include <SQLAPI.h> // main SQLAPI++ header
// #include <cppdb/frontend.h>

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
			for (int i = 0; i < count;++i)
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
				AddConnection(pos, (*it)->position, (*it)->color, video::SColor(1,1,1,0));
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
			const scene::ParticleEmitter::ParticleList& particles= m_emitter->getParticles();
			math::vector3d pos = m_emitter->GetSystem()->getOwner()->getAbsolutePosition();

			m_grid.Reset();
			//m_grid.SetSize(m_emitter->GetBoundingBox().getExtent() * 2);

			Reset();
			scene::ParticleEmitter::ParticleList::const_iterator it = particles.begin();
			for (ushort i = 0; it != particles.end(); ++it,i+=2)
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
					for (int j2 = 0; it2 != r.end() /*&& j2 < 20*/; ++it2,++j2)
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

  					if (bestDist>4)
  						continue;

					bestDist = sqrtf(bestDist);
				//	if (a->GetLevelID() == b->GetLevelID())
					{
						video::SColor clr(1);
						clr.A = bestDist/5.0f;
						clr.A =  0.5f+math::clamp<float>(clr.A, 0, 1)*0.5f;
						AddConnection(a->position, b->position, clr,clr);
					}
				}
			}

			ConnectionLineRenderer::update(dt);
		}

	};
}

class EditboxFocusListener:public GUI::IGUIManagerListener
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
		m->SetFocus(editbox);
	}
	virtual void onEvent(Event* e) 
	{}

} g_editboxFocuslistener;

Application::Application()
{
	m_limitFps = true;
	m_limitFpsCount = 30;
	m_debugging = false;
}
Application::~Application()
{
}

core::UTFString Text;

GUI::ITextDecorateNode* TextD;

void Application::_OnSelectUserChange(IObject* sender, PVOID param)
{
	if (!sender)
		return;
	GUI::IGUIListBox* lstBox=dynamic_cast<GUI::IGUIListBox*>(sender);
	int i = lstBox->GetSelectedItem();
	if (i == -1)
		return;
	GUI::TwitterProfileListItem* item= dynamic_cast<GUI::TwitterProfileListItem*>(lstBox->GetItem(i));
	if (!item)
		return;
	ted::UserDB* u = item->GetUser();
	for (int i = 0; i < m_loadedParticles.size(); ++i)
	{
		m_loadedParticles[i]->scale = 0.1;
		m_loadedParticles[i]->color = 1;
	}
	m_loadedParticles.clear();
	m_screenLayout->UserProfile->SetUser(u);
	for (int i = 0; i < u->tweets.size(); ++i)
	{
		scene::TweetParticle* p= m_tweetEmitter->GetTweetParticle(u->tweets[i]->ID);
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
			item = new GUI::GUITweetItem(getGUIManager());
			m_screenLayout->TweetsList->AddElement(item);
			item->SetSize(math::vector2d(500, 150));
			m_tweetItems.push_back(item);
		}
		item->SetTweet(u->tweets[i]);

	}
	m_screenLayout->TweetsList->SetScrollOffset(0);
	printf("Tweets count: %d\n",item->GetUser()->tweets.size());
}

bool _UsersSortFunc(const ted::UserDB* a, const ted::UserDB* b)
{
	if (a->displayName.compare_ignore_case(b->displayName)==-1)
		return false;
	else return true;
}
void Application::_OnUserNameChange(IObject* sender, PVOID param)
{
	if (!sender)
		return;
	core::stringw str = dynamic_cast<GUI::IGUIEditBox*>(sender)->GetText().GetAsStringW();
	m_screenLayout->UsersList ->ClearItems();
	printf("cleared\n");
	if (str == L"")
	{
		m_screenLayout->UserProfile->SetUser(0);
		for (int i = 0; i < m_tweetItems.size(); ++i)
			m_tweetItems[i]->SetVisible(false);
		gTextureResourceManager.unloadAll(true);
		return;
	}

	ted::UserDB* firstUser = 0;

	std::vector<ted::UserDB*> users;
	for (int i = 0; i < ted::UserDB::UserDBList.size(); ++i)
	{
		ted::UserDB* u = ted::UserDB::UserDBList[i];
		if (core::StringUtilW::BeginsWith(u->displayName, str))
		{
			users.push_back(u);
		}
	}
	if (!users.size())
		return;
	std::sort(users.begin(),users.end(), _UsersSortFunc);
	for (int i = 0; i < users.size();++i)
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

void Application::init(const OptionContainer &extraOptions)
{
	CMRayApplication::init(extraOptions);

	{
		m_debugging = extraOptions.GetOptionValue("Debugging")=="Yes";

	}
	CMRayApplication::loadResourceFile(mT("Resources.stg"));

	network::createWin32Network();

	m_fontGen = new GUI::DynamicFontGenerator();

	gFontResourceManager.setDefaultFont(m_fontGen);

	//gImageSetResourceManager.loadImageSet(mT("VistaCG_Dark.imageset"));
	GCPtr<OS::IStream> themeStream = gFileSystem.createBinaryFileReader(mT("VistaCG_Dark.xml"));
	GUI::GUIThemeManager::getInstance().loadTheme(themeStream);
	GUI::GUIThemeManager::getInstance().setActiveTheme(mT("VistaCG_Dark"));
	gFontResourceManager.loadFontsFromDir(gFileSystem.getAppPath()+ "..\\Data\\Fonts\\");
	m_leap = new nui::LeapDevice;
	m_leap->AddListener(this);

	m_fontGen->Init();
	
	m_fontGen->SetTextureSize(2048);
	m_fontGen->SetFontName(L"Arial");
	m_fontGen->SetFontResolution(24);

	m_camera = getSceneManager()->createCamera();
	m_camera->setAutoUpdateAspect(true);
	m_camera->setUseTarget(true);
	m_camera->setTarget(0);
	m_camera->setPosition(math::vector3d(10));
	{
		video::IRenderTargetPtr rt = getDevice()->createRenderTarget("", getDevice()->createTexture2D(1,video::EPixel_R8G8B8A8,false), 0, 0, 0);
		scene::ViewPort* vp= GetRenderWindow()->CreateViewport("", m_camera, rt, math::rectf(0, 0, 1, 1), 0);
		vp->enablePostProcessing(true);

		video::ParsedShaderPP* pp = new video::ParsedShaderPP(getDevice());

		pp->LoadXML(gFileSystem.openFile("blur.peff"));
		vp->setPostProcessing(pp);
	}
	m_particleSystem = new scene::SParticleSystem();

	scene::ISceneNode* psNode = getSceneManager()->createSceneNode("Particle System");
	 
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


	gmngr = new GUI::GUIManager(getDevice());

	getGUIManager()->SetActiveTheme(GUI::GUIThemeManager::getInstance().getActiveTheme());

	m_guiroot = (GUI::IGUIPanelElement*)new GUI::IGUIPanelElement(core::string(""), getGUIManager());
	m_guiroot->SetDocking(GUI::EED_Fill);
	getGUIManager()->SetRootElement(m_guiroot);

	{
		REGISTER_GUIElement_FACTORY(GUITweetItem);
		REGISTER_GUIElement_FACTORY(GUIUserProfile);
		REGISTER_GUIElement_FACTORY(GUISessionSidePanel);
		REGISTER_GUIElement_FACTORY(GUISessionDetailsTopPanel);
		GUI::GUIOverlay* screenOverlay= GUI::GUIOverlayManager::getInstance().LoadOverlay("GUIScreenLayout.gui");
		m_screenLayout = new GUI::GUIScreenLayoutImpl();
		screenOverlay->CreateElements(getGUIManager(), m_guiroot, 0, m_screenLayout);

		g_editboxFocuslistener.editbox = m_screenLayout->UserTxt;
		getGUIManager()->AddListener(&g_editboxFocuslistener);

		getGUIManager()->SetFocus(m_screenLayout->UserTxt);
		m_screenLayout->UserTxt ->OnTextChange += CreateObjectDelegate(Application, this, _OnUserNameChange);
		m_screenLayout->UsersList->OnSelectChange += CreateObjectDelegate(Application, this, _OnSelectUserChange);
		m_screenLayout->RootStack->SetUseScroll(false);
	}

	// 	m_userProfile->GetFontAttributes()->hasShadow = false;
//	m_userProfile->GetFontAttributes()->fontColor.Set(0, 0, 0, 1);
	if (false)
	{
		try
		{

			ted::DBHandler::Instance()->LoadDB();
			int i = 0;
			core::UTFString str;

			if (false)
			{
				for (int i = 0; i < ted::UserDB::UserDBList.size(); ++i)
				{
					ted::UserDB* u = ted::UserDB::UserDBList[i];
					OS::IStream* image = network::InternetCacheManager::getInstance().GetOrCreateItem(u->imageUrl);
					printf("[%d]- %s ", i, u->imageUrl.c_str());

					if (image)
					{
						printf("was loaded\n");
						//	video::ITexturePtr t = gTextureResourceManager.loadTexture2D(u->imageUrl, image);
						//	t->setMipmapsFilter(false);
						//	m_images.push_back(t);
					}
					else
					{
						printf("faile to load\n");

					}

				}
			}
			if (true)
			{
				int bytesCount = 0;
				for (int i = 0; i < ted::TweetDB::TweetDBList.size(); ++i)
				{
					ted::TweetDB* t = ted::TweetDB::TweetDBList[i];
					core::UTFString str = t->user->displayName;
					str += core::string(":");
					str += t->text;

					bytesCount += str.Length()*sizeof(utf32)+sizeof(GUI::StringListItem);;
					//GUI::StringListItem *item = new GUI::StringListItem(str);
					//	listBox->AddItem(item) ;
				}
				printf("%d\n", bytesCount);
				network::InternetCacheManager::getInstance().SaveCache();
			}
			Text = str;

			GUI::TextDecorator decorator;
			//TextD = decorator.ParseText(Text);


		}
		catch (SAException& e)
		{
			printf("%s\n", e.ErrText());
		}
	}
}

void Application::WindowPostRender(video::RenderWindow* wnd)
{
	video::TextureUnit tex;

	getDevice()->set3DMode();
	

	getDevice()->set2DMode();

	for (int i = 0; i < m_images.size(); ++i)
	{
		tex.SetTexture(m_images[i]);
		//getDevice()->useTexture(0, &tex);
		//getDevice()->draw2DImage(math::rectf(i*100, 0, 100+i*100, 100), 1);
	}

	GUI::GUIBatchRenderer m_guiRender;
	m_guiRender.SetDevice(getDevice());

	m_guiRender.Prepare();

	float yoffset = 50;


	if (m_debugging)
	{
		GUI::FontAttributes attr;
		attr.fontColor.Set(0.05, 1, 0.5, 1);
		attr.fontAligment = GUI::EFA_MiddleLeft;
		attr.fontSize = 24;
		attr.hasShadow = true;
		attr.shadowColor.Set(0, 0, 0, 1);
		attr.shadowOffset = math::vector2d(2);
		attr.spacing = 2;
		attr.wrap = 0;
		attr.RightToLeft = 0;
		core::UTFString msg;
		msg = L"FPS: " + core::string_to_wchar(core::StringConverter::toString(gFPS.getFPS()));
		m_fontGen->print(math::rectf(0, 0, 200, 100), &attr, &math::rectf(0, 0, 200, 100), msg, &m_guiRender);
		msg = Text;// core::stringw(L"※ご登録アドレスが、PCメール拒否、ドメイン指定等の設定がされている可能性");

		GUI::TextContextAttributes context;
		context.font = m_fontGen;
		context.device = getDevice();
		context.fontAttributes = attr;
		context.pos.set(0, 50);
		//TextD->CalculateSize(&context);
		//TextD->Draw(&context, &m_guiRender,&math::rectf(0,wnd->GetSize()));
		attr.wrap = true;
		//m_fontGen->print(math::rectf(24, 0, 500, 500), &attr, &math::rectf(0, 0, 500, 500), msg, &m_guiRender);

		int count = m_particleSystem->GetEmitter(0)->getParticles().size();
		msg = "Particles Count=" + core::StringConverter::toString(count);
		m_fontGen->print(math::rectf(50, 0, 150, 200), &attr, 0, msg, &m_guiRender);

		msg = "Draw Calls=" + core::StringConverter::toString(getDevice()->getBatchDrawnCount());
		m_fontGen->print(math::rectf(50, 30, 150, 200), &attr, 0, msg, &m_guiRender);

		msg = "Draw Primitives=" + core::StringConverter::toString(getDevice()->getPrimitiveDrawnCount());
		m_fontGen->print(math::rectf(50, 60, 150, 200), &attr, 0, msg, &m_guiRender);

		m_guiRender.Flush();/**/
	}
}
void Application::update(float dt)
{
	CMRayApplication::update(dt);

	getGUIManager()->Update(dt);

	m_orbitAngles.x += m_speedVec.x*dt;
	m_orbitAngles.y += m_speedVec.z*dt;

	m_cameraRadius += m_speedVec.y*dt*0.1f;

	m_orbitAngles.y = math::clamp<float>(m_orbitAngles.y, -70, 70);

	m_speedVec += m_leapVec*dt;
	m_speedVec -= m_speedVec*dt;
	
	float offset = m_screenLayout->TweetsList->GetScrollOffset();
	offset += m_speedVec.z*dt*2;
	m_screenLayout->TweetsList->SetScrollOffset(offset);

	m_cameraRadius = math::clamp<float>(m_cameraRadius, 40, 80);

	math::vector3d pos;
	pos.x = m_cameraRadius * math::cosd(m_orbitAngles.x)* math::cosd(m_orbitAngles.y);
	pos.y = m_cameraRadius * math::sind(m_orbitAngles.y);
	pos.z = m_cameraRadius * math::sind(m_orbitAngles.x)* math::cosd(m_orbitAngles.y);
	m_camera->setPosition(pos);
}
void Application::OnGesture(nui::LeapDevice* d, nui::ELeapGestureType type, Leap::Gesture g)
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

}



