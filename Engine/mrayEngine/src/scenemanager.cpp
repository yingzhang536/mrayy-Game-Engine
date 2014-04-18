#include "stdafx.h"


#include "SceneManager.h"

#include "ITimer.h"
#include "ILogManager.h"

#include "LightNode.h"
#include "Engine.h"

#include "IRenderTarget.h"

#include "CameraNode.h"
#include "SSkyBoxNode.h"
 
#include "TraceManager.h"
#include "NormalRenderPass.h"

#include "common.h"
#include "IThreadManager.h"
#include "MathUtil.h"

#include "IMutex.h"
#include "XMLTree.h"

#include "ISceneVisitor.h"
#include "ThreadJobManager.h"

#include "RequestToRenderVisitor.h"
#include "RenderSceneVisitor.h"
#include "UpdateSceneVisitor.h"
#include "SceneLoggerSystem.h"
#include "MutexLocks.h"

#include "BenchmarkItem.h"
#include "ViewPort.h"

namespace mray{
namespace scene{


SceneManager::SceneManager(video::IVideoDevice*dev):
ISceneManager(dev),m_activeCamera(0)
{

	m_benchmarkItem=new BenchmarkItem(mT("SceneManager"));

	m_nodesMutex=OS::IThreadManager::getInstance().createMutex();

	m_nameIDGenerator=0;

	m_jobsThreadsManager=new ThreadJobManager();
	m_renderManager=new RenderManager(this);

	m_renderBI=m_benchmarkItem->addSubItem(mT("render"));
	m_updateBI=m_benchmarkItem->addSubItem(mT("update"));

	//by default,there is one normal render pass
	//m_normalRP=new NormalRenderPass();
	//addRenderPass(m_normalRP);

	m_activeViewnode=0;

	m_debugDrawManager=0;
}

SceneManager::~SceneManager()
{

	delete m_renderManager;
	delete m_jobsThreadsManager;

	unregisterBenchmark();

	clearSceneNodes();

	delete m_nodesMutex;
	delete m_benchmarkItem;
	
}


void SceneManager::_collectPasses(RenderPassList& plist)
{
	std::list<IRenderPassSubscriber*>::iterator pit= m_passSubscribers.begin();
	for(;pit!=m_passSubscribers.end();++pit)
	{
		(*pit)->GetRenderPasses(plist);
	}
}
void SceneManager::addLightToRender(LightNode*l)
{
	m_lightNodesToRender.push_back(l);
	/*std::list<IRenderPass*> p;
	l->GetRenderPasses(p);
	
	std::list<IRenderPass*>::iterator it= p.begin();
	for(;it!=p.end();++it)
		m_shadowPasses.push_back(*it);*/
}

void SceneManager::addNodeToRender(ISceneNode*node)
{
	m_nodesToRender.push_back(node);
}

void SceneManager::clearRenderList()
{
	m_lightNodesToRender.clear();
	//m_shadowPasses.clear();
	m_nodesToRender.clear();
}

scene::RenderManager* SceneManager::getRenderManager(){
	return m_renderManager;
}

void SceneManager::SetDebugDrawManager(IDebugDrawManager* mngr)
{
	m_debugDrawManager=mngr;
}
IDebugDrawManager* SceneManager::GetDebugDrawManager()
{
	return m_debugDrawManager;
}

void SceneManager::registerBenchmark(){
	Engine::getInstance().getRootBenchmarking()->addSubItem(m_benchmarkItem);
}
void SceneManager::unregisterBenchmark()
{
	Engine::getInstance().getRootBenchmarking()->removeSubItem(m_benchmarkItem);
}

void SceneManager::removeSceneNode(ISceneNode*node)
{
	if(!node)
		return;

	node->_OnRemovedFromSceneManager(this);
	m_objectsID.RemoveObject(node->getID());

	m_Children.erase(node->getNodeName());
}



void SceneManager::addSceneNode(const ISceneNodePtr& node)
{
	if(!node)
		return;

	SceneNodeMap::iterator it= m_Children.find(node->getNodeName());
	if(it!=m_Children.end())
	{
		gSceneLoggerSystem.log(mT("AddSceneNode: SceneNode with name:")+node->getNodeName()+mT(" is already exists in the scene manager"),ELL_WARNING,EVL_Heavy);
		node->setNodeName(node->getNodeName()+mT("#")+core::StringConverter::toString(++m_nameIDGenerator)+mT("_"));
		addSceneNode(node);
		return;
	}
	node->SetSceneManager(this);

	{
		OS::ScopedLock lock(m_nodesMutex);
		m_Children[node->getNodeName()]=node;
		uint id=m_objectsID.AddObject(node,node->getID());
		node->setID(id);
	}

	node->_OnAddedToSceneManager(this);
}


LightNode* SceneManager::createLightNode(const core::string&name)
{
	core::string strName=name;
	if(strName==mT(""))
	{
		strName=mT("LightNode#")+core::StringConverter::toString(++m_nameIDGenerator);
	}
	SceneNodeMap::iterator it= m_Children.find(strName);
	if(it!=m_Children.end())
	{
		gSceneLoggerSystem.log(mT("CreateLight: SceneNode with name:")+strName+mT(" is already exists in the scene manager"),ELL_WARNING,EVL_Heavy);
		//pick new name
		strName+=core::StringConverter::toString(++m_nameIDGenerator);
	}

	LightNode* node=new LightNode(strName,m_objectsID.GetNextID(),this);
	m_objectsID.AddObject(node,node->getID());
	m_Children[strName]=node;
	node->_OnAddedToSceneManager(this);
	return node;
}
ISceneNode* SceneManager::createSceneNode(const core::string&name)
{
	core::string strName=name;
	if(strName==mT(""))
	{
		strName=mT("SceneNode#")+core::StringConverter::toString(++m_nameIDGenerator);
	}
	SceneNodeMap::iterator it= m_Children.find(strName);
	if(it!=m_Children.end())
	{
		gSceneLoggerSystem.log(mT("CreateSceneNode: SceneNode with name:")+strName+mT(" is already exists in the scene manager"),ELL_WARNING,EVL_Heavy);
		//pick new name
		strName+=core::StringConverter::toString(++m_nameIDGenerator);
	}

	ISceneNode* node=new ISceneNode(strName,m_objectsID.GetNextID(),this);
	m_objectsID.AddObject(node,node->getID());
	m_Children[strName]=node;
	node->_OnAddedToSceneManager(this);
	return node;
}
CameraNode* SceneManager::createCamera(const core::string&name)
{
	core::string strName=name;
	if(strName==mT(""))
	{
		strName=mT("CameraNode#")+core::StringConverter::toString(++m_nameIDGenerator);
	}
	SceneNodeMap::iterator it= m_Children.find(strName);
	if(it!=m_Children.end())
	{
		gSceneLoggerSystem.log(mT("CreateCamera: SceneNode with name:")+strName+mT(" is already exists in the scene manager"),ELL_WARNING,EVL_Heavy);
		//pick new name
		strName+=core::StringConverter::toString(++m_nameIDGenerator);
	}

	CameraNode* node=new CameraNode(strName,m_objectsID.GetNextID(),this);
	m_objectsID.AddObject(node,node->getID());
	m_Children[strName]=node;
	node->_OnAddedToSceneManager(this);
	return node;
}



ISceneNode* SceneManager::getNodeByID(uint id)
{
	scene::ISceneNode*res=0;

	m_objectsID.GetObjectByID(id,res);

	return res;
}

void SceneManager::clearSceneNodes()
{
 	SceneNodeMap::iterator it=m_Children.begin();
 	for(;it!=m_Children.end();++it)
 		it->second->_OnRemovedFromSceneManager(this);
	m_Children.clear();
	m_objectsID.Clear();
}





void SceneManager::_setupLightsToRender(IRenderPass*pass)
{
	getDevice()->disableAllLights();
	std::list<LightNode*>::iterator it=m_lightNodesToRender.begin();
	for(;it!=m_lightNodesToRender.end();++it)
	{
		(*it)->render(pass);
	}
}


void SceneManager::doRender()
{
	/*
	ViewPortMultiList::iterator it=m_viewports.begin();
	for (;it!=m_viewports.end();++it)
	{
		ViewPortList&lst=(*it);
		ViewPortList::iterator vpIt=lst.begin();
		for (;vpIt!=lst.end();++vpIt)
		{
			if((*vpIt)->isActive())
				render(*vpIt);
		}

	}*/
}

void SceneManager::render(ViewPort*vp)
{


	_AUTO_BENCHMARK(m_renderBI);
	traceFunction(eScene);

	if(!getDevice() || !vp || !vp->getCamera())
		return;

	getDevice()->set3DMode();

	if(m_debugDrawManager)
	{
		//prepare debug draw manager primitives
		m_debugDrawManager->StartDraw(vp->getCamera());
	}
	video::IRenderTarget* renderTarget;
	if(vp ){
		if(vp->getCamera())
			m_activeCamera=vp->getCamera();
		if(vp->getRenderTarget() )
			renderTarget=vp->getRenderTarget();
	}

	if(vp){
		//math::rectf rc=vp->getAbsViewPort();
		//getDevice()->setViewportRect(rc);
	}

	RenderPassList passes;
	/*
	{
		RenderPassList::iterator passIt=m_shadowPasses.begin();
		for (;passIt!=m_shadowPasses.end();++passIt)
			passes.push_back(*passIt);
	}
	{
		RenderPassList::iterator passIt=m_passes.begin();
		for (;passIt!=m_passes.end();++passIt)
			passes.push_back(*passIt);
	}
	*/
	_collectPasses(passes);
	vp->getCamera()->GetRenderPasses(passes);

	IViewNode*oldVN=0;
	RenderPassList::iterator passIt=passes.begin();
	for (;passIt!=passes.end();++passIt)
	{
		if(!(*passIt)->IsEnabled())
			continue;
		m_currentPass=*passIt;
		m_currentPass->setActiveCamera(m_activeCamera);

		m_currentPass->beginPass(this,vp);

		RequestToRenderVisitor preRenderVisitor(m_currentPass);
		RenderSceneVisitor renderVisitor(m_currentPass);

		m_renderManager->clear();
		m_jobsThreadsManager->Reset();
		OnVisit(&preRenderVisitor);
	//	m_jobsThreadsManager->BeginProcessing();
		//	if(oldFrust!=m_activeFrustrum)
		{

			m_renderManager->startSorting();
			oldVN=m_activeViewnode;
		}


		getDevice()->clearBuffer(video::EDB_DEPTH);
		_setupLightsToRender(m_currentPass);

//		m_jobsThreadsManager->EndProcessing();
		m_renderManager->endSorting();

		if(m_activeCamera)
			m_activeCamera->render(m_currentPass);
		m_renderManager->renderAll(m_currentPass);
		//OnVisit(&renderVisitor);
		m_currentPass->endPass();
	}
	m_currentPass=0;

	m_renderManager->clear();

	m_lightNodesToRender.clear();

	if(m_debugDrawManager)
	{
		//draw primitives
		m_debugDrawManager->EndDraw();
	}
	
	//update camera view
//	if(ActiveCamera)
//		ActiveCamera->updateView();

	
}

void SceneManager::update(float dt)
{
	_AUTO_BENCHMARK(m_updateBI);
	UpdateSceneVisitor visitor(dt,m_jobsThreadsManager);
	m_jobsThreadsManager->Reset();
	OnVisit(&visitor);
	m_jobsThreadsManager->BeginProcessing();

//	m_jobsThreadsManager->EndProcessing();
	/*
	{
		SceneNodeMap::iterator it= m_Children.begin();
		for (;it!=m_Children.end();++it)
		{
			it->second->OnVisit(&visitor);
		}
	}*/

	/*
	ulong s1=gTimer.getActualTime();
	synchronized(m_nodesMutex)
	{
		SceneNodeList::iterator itc=m_Children.begin();
		for(;itc!=m_Children.end();++itc)
		{
			(*itc)->update(dt);
		}
	}
	m_benchmarkItem->benchmarkAddSample(mT("update"),(gTimer.getActualTime()-s1));*/

}



/*
void SceneManager::addNodeToRender(ISceneNode* node,NODE_RENDER_TYPE nodeType)
{
	switch(nodeType)
	{
	case NRT_SOLID:
		if(isNodeCulled(node))return;
		m_layer.solid.push_back(SolidNode(node,0));
		break;
	case NRT_TRANSPARENT:
		if(isNodeCulled(node))return;
		if(ActiveCamera)
			m_layer.transparent.push_back(TransparentNode(node,ActiveCamera->position()));
		break;
	case NRT_CAMERA:
		if(!ActiveCamera)
			setActiveCamera(dynamic_cast<CameraNode*>(node));
		cameraNodes.push_back(node);
		break;
	case NRT_LIGHT:
		{
			LightNode*light=dynamic_cast<LightNode*>(node);
			GCPtr<IRenderPass> pass=light->getShadowRenderPass();
			if(light->isCastShadows() && !haveRenderPass(pass)){
				addRenderPass(pass);
			}else if(!light->isCastShadows() && haveRenderPass(pass)){
				removeRenderPass(pass);
			}
			lightNodes.push_back(light);
		}
		break;
	};
}*/

void SceneManager::addPassSubscriber(IRenderPassSubscriber*rps)
{
	if(!rps)
		return;
	std::list<IRenderPassSubscriber*>::iterator it= m_passSubscribers.begin();
	for(;it!=m_passSubscribers.end();++it)
	{
		if((*it)==rps)
		{
			return;
		}
	}
	m_passSubscribers.push_back(rps);
}
void SceneManager::removePassSubscriber(IRenderPassSubscriber*rps)
{
	std::list<IRenderPassSubscriber*>::iterator it= m_passSubscribers.begin();
	for(;it!=m_passSubscribers.end();++it)
	{
		if((*it)==rps)
		{
			m_passSubscribers.erase(it);
			return;
		}
	}
}


CameraNode *SceneManager::getActiveCamera()
{
	return m_activeCamera;
}
void SceneManager::setActiveCamera(CameraNode* camera)
{
	m_activeCamera=camera;
	if(m_activeCamera){
		setActiveViewnode(m_activeCamera);
	}else{
		setActiveViewnode(0);
	}
}

void SceneManager::setActiveViewnode(IViewNode*node){
	m_activeViewnode=node;
}

IViewNode* SceneManager::getActiveViewnode(){
	return m_activeViewnode;
}

bool SceneManager::isNodeCulled(ICullable*node)
{
	/*
	if(m_activeFrustrum){
		if(!node)return 1;
		if(node->getCullingType()==SCT_NONE)return false;

		math::box3d box=node->getTransformedBoundingBox();

		bool c=!m_activeFrustrum->boxInFrustrum(box);//boundingBox.intersectWithBox(box);

		if(!c && node->getViewDistance()>0){
			float d=(node->getAbsolutePosition()-m_activeFrustrum->viewPos).LengthSQ();
			if(d>=node->getViewDistance()*node->getViewDistance())
				c=true;
		}

		return c;
	}
	return false;*/
	if(m_activeViewnode)
		return m_activeViewnode->isCulling(node);
	else return false;
}


void SceneManager::addRenderPass(const IRenderPassPtr& pass)
{
/*	if(pass->isPrepareationPass())
		m_passes.push_front(pass);
	else
		m_passes.push_back(pass);*/
}
void SceneManager::removeRenderPass(IRenderPass* pass)
{/*
	RenderPassList::iterator it=m_passes.begin();
	for(;it!=m_passes.end();++it)
	{
		if((*it)==pass){
			m_passes.erase(it);
			return;
		}
	}*/
}

bool SceneManager::haveRenderPass(IRenderPass* pass)
{/*
	RenderPassList::iterator it=m_passes.begin();
	for(;it!=m_passes.end();++it)
	{
		if((*it)==pass){
			return true;
		}
	}*/
	return false;
}
IRenderPass* SceneManager::getCurrnetRenderPass()
{
	return m_currentPass;
}


void SceneManager::OnVisit(ISceneVisitor*visitor)
{
	visitor->Visit(this);
}


}
}