
#include "stdafx.h"

#include "ISceneNode.h"
#include "ISceneManager.h"
#include "MaterialResourceManager.h"
#include "ISceneVisitor.h"
//#include "MountPoint.h"

#include "SMesh.h"
#include "ISceneObjectsDesc.h"
#include "IDebugDrawManager.h"

namespace mray{
namespace scene{


ISceneNode::ISceneNode(const  core::string&name,int id,ISceneManager* sceneMngr):
	m_isNeedUpdate(1),m_updateChilds(1),m_parent(0),m_sceneMngr(sceneMngr),m_scale(1),
		m_isVisible(true),m_Name(name),m_id(id),m_drawBoundingBox(false)
{
	setCullingType(SCT_BBOX);
	m_drownLastFrame=false;

	updateAbsoluteTransformation();
		
	fillProperties();
};
ISceneNode::~ISceneNode()
{
	//removeAll();
}

void ISceneNode::SetDrown(bool d)
{
	m_drownLastFrame=d;
}

bool ISceneNode::IsDrown()
{
	return m_drownLastFrame;
}

int ISceneNode::GetNodeType()
{
	return ENodeType_SimpleNode;
}

const core::string& ISceneNode::getNodeName()const{
	return m_Name;
}
bool ISceneNode::setNodeName(const core::string&name){
	m_Name=name;
	return true;
}

bool ISceneNode::setID(const uint& val){
	m_id=val;
	return true;
}
const uint& ISceneNode::getID(){
	return m_id;
}


bool ISceneNode::setVisible(const bool& val){
	m_isVisible=val;
	return true;
}

bool ISceneNode::setVisible(bool v,bool cascade)
{
	m_isVisible=v;
	if(cascade)
	{
		MovableNodeList::iterator it=m_Children.begin();
		MovableNodeList::iterator end=m_Children.end();
		for(;it!=end;++it)
		{
			ISceneNode* n=dynamic_cast<ISceneNode*>((*it).pointer());
			if(n)
				n->setVisible(v,true);
		}
	}
	return true;
}

bool ISceneNode::isVisible()const
{
	return m_isVisible;
}


bool ISceneNode::setDrawBoundingBox(const bool& val){
	m_drawBoundingBox=val;
	return true;
}
const bool& ISceneNode::getDrawBoundingBox(){
	return m_drawBoundingBox;
}
void ISceneNode::fillProperties()
{

	CPropertieDictionary* dic=0;
	if(CreateDictionary(&dic))
	{
		dic->addPropertie(&PropertyTypeID::instance,mT("Design"));
		dic->addPropertie(&PropertyTypeName::instance,mT("Design"));

		dic->addPropertie(&PropertyTypeDrawBoundingBox::instance,mT("Debug"));

		dic->addPropertie(&PropertyTypePosition::instance,mT("Transformation"));
		dic->addPropertie(&PropertyTypeOrintation::instance,mT("Transformation"));
		dic->addPropertie(&PropertyTypeScale::instance,mT("Transformation"));

		dic->addPropertie(&PropertyTypeVisible::instance,mT("State"));
	}
}



math::vector3d ISceneNode::getAbsolutePosition()const{
	return getAbsoluteTransformation().getTranslation();
}

math::quaternion ISceneNode::getAbsoluteOrintation()const{
	return m_absOrintaion;
}
math::vector3d ISceneNode::getAbsoluteScale()const{
	return m_absScale;
}

const math::vector3d& ISceneNode::getPosition()const{
	return m_position;
}
const math::quaternion& ISceneNode::getOrintation()const{
	return m_orintation;
}
const math::vector3d& ISceneNode::getScale()const{
	return m_scale;
}

bool ISceneNode::setPosition(const math::vector3d&v){
	m_position=v;
	return true;
}
bool ISceneNode::setScale(const math::vector3d& v){
	m_scale=v;
	return true;
}

bool ISceneNode::setOrintation(const math::quaternion& q){
	m_orintation=q;
	return true;
}


void ISceneNode::setTranformsAsInitial(){
	m_initialPosition=m_position;
	m_initialOrintation=m_orintation;
	m_absScale=m_scale;
}
void ISceneNode::resetTranformsToInitial(){
	m_position=m_initialPosition;
	m_orintation=m_initialOrintation;
	m_scale=m_absScale;

}


ISceneManager* ISceneNode::getSceneManager(){return m_sceneMngr;}

void ISceneNode::SetSceneManager(ISceneManager* mngr)
{
	m_sceneMngr=mngr;
}

const math::box3d& ISceneNode::getBoundingBox()
{/*
	math::box3d bb;
	SMesh*m=0;
	bool first=true;
	for (int i=0;i<m_attachedNodes.size();++i)
	{
		m=m_attachedNodes[i]->getMesh();
		if(m){
			if(first){
				bb=m->getBoundingBox();
				first=false;
			}else
				bb.AddBox(m->getBoundingBox());
		}
	}
	return math::box3d(0,0);*/
	return m_boundingBox;
};
math::box3d ISceneNode::getTransformedBoundingBox(){
	math::box3d box=getBoundingBox();
	const math::matrix4x4 &mat=getAbsoluteTransformation();
	math::vector3d edges[8];
	box.getEdges(edges);
	edges[0]=mat*edges[0];
	box.reset(edges[0]);
	for(int i=1;i<8;++i){
		edges[i]=mat*edges[i];
		box.AddPoint(edges[i]);
	}
	return box;
}


const MovableNodeList& ISceneNode::getChildren(){
	return m_Children;
};



IMovable* ISceneNode::getParent(){return m_parent;}

void ISceneNode::setParent(IMovable*p){
	if (!p && m_sceneMngr && m_parent != m_sceneMngr->getRootNode())
	{
		m_parent = 0;
		m_sceneMngr->getRootNode()->addChild(this);
	}
	else if (p && m_parent == m_sceneMngr->getRootNode())
	{
		m_parent->removeChild(this);
		m_parent = p;
	}else
		m_parent = p;
}

std::list<IAnimatorsPtr>* ISceneNode::getAnimators()
{
	return &Animators;
}

void ISceneNode::removeAll()
{
	{
		MovableNodeList::iterator it=m_Children.begin();
		MovableNodeList::iterator end=m_Children.end();
		for(;it!=end;++it)
		{
			ISceneNode*node=dynamic_cast<ISceneNode*>((*it).pointer());
			if(node)
			{
				if( m_sceneMngr)
					m_sceneMngr->removeSceneNode(node);
			}
			(*it)->setParent(0);
		}
	}
	{
		MountPointsList::iterator it=m_mountPoints.begin();
		for (;it!=m_mountPoints.end();++it)
		{
			delete *it;
		}
		m_mountPoints.clear();
	}
	m_Children.clear();
	Animators.clear();
	ClearAttachedNodes();
}


void ISceneNode::AttachNode(const IRenderablePtr &node)
{
	if(node.isNull())
		return;
	m_attachedNodes.push_back(node);
	node->setOwner(this);
	_UpdateBoundingBox();
}
void ISceneNode::RemoveNode(IRenderable*node)
{
	if(!node)
		return;
	AttachNodesList::iterator it= m_attachedNodes.begin();
	for(;it!=m_attachedNodes.end();++it)
	{
		if((*it).pointer()==node)
		{
			node->setOwner(0);
			m_attachedNodes.erase(it);
			_UpdateBoundingBox();
			return;
		}
	}
}
int ISceneNode::GetAttachedNodesCount()
{
	return m_attachedNodes.size();
}
const AttachNodesList& ISceneNode::GetAttachedNodes()
{
	return m_attachedNodes;
}
IRenderable* ISceneNode::GetAttachedNode(int i)
{
	if(i>=m_attachedNodes.size())return 0;
	return m_attachedNodes[i].pointer();
}
void ISceneNode::ClearAttachedNodes()
{
	for (int i=0;i<m_attachedNodes.size();++i)
	{
		m_attachedNodes[i]->setOwner(0);
	}
	m_attachedNodes.clear();
	_UpdateBoundingBox();
}



void ISceneNode::addAnimator(const IAnimatorsPtr& anim)
{
	if(anim){
		Animators.push_back(anim);
	}
}

void ISceneNode::removeAnimator(IAnimators*  anim)
{
	if(anim){
		std::list<GCPtr<IAnimators>>::iterator it=Animators.begin();
		for(;it!=Animators.end();++it)
			if((*it).pointer()==anim)
			{
				Animators.erase(it);
				return;
			}
	}
}


void ISceneNode::preRender()
{
	if(!isVisible())return;
	/*
	if(getSceneManager()->isNodeCulled(this))
		return;
	{

		core::IteratorPair<SceneNodeList> it(m_Children);
		for(;!it.done();++it)
		{
			(*it)->preRender();
		}
	}
	{
		for(int i=0;i<m_attachedNodes.size();++i)
			m_attachedNodes[i]->preRender();
	}*/
	{

		MountPointsList::iterator it=m_mountPoints.begin();
		MountPointsList::iterator end=m_mountPoints.end();
		for(;it!=end;++it)
		{
			(*it)->preRender();
		}
	}

}
void ISceneNode::update(float dt)
{
	m_prevTransformation=absTransformation;

	{
		std::list<GCPtr<IAnimators>>::iterator it=Animators.begin();
		for(;it!=Animators.end();++it)
		{
			
			(*it)->animate(this,dt);
		}
	}

//	updatePos();
	updateAbsoluteTransformation();
/*
	{
	for(int i=0;i<m_attachedNodes.size();++i)
	m_attachedNodes[i]->update(dt);
	}

	{
		core::IteratorPair<SceneNodeList> cit(m_Children);
		for(;!cit.done();++cit)
		{
			(*cit)->update(dt);
		}
	}*/
	{

		MountPointsList::iterator it=m_mountPoints.begin();
		MountPointsList::iterator end=m_mountPoints.end();
		for(;it!=end;++it)
		{
			(*it)->update(dt);
		}
	}
	_UpdateBoundingBox();

}

void ISceneNode::render(IRenderPass*pass)
{
	if(!isVisible())
		return;
	if(getDrawBoundingBox())
	{
		//getSceneManager()->getDevice()->useMaterial(0);
		if(getSceneManager()->GetDebugDrawManager())
		{
			getSceneManager()->GetDebugDrawManager()->AddOBB(getAbsoluteTransformation(),getTransformedBoundingBox().getExtent(),1);
		}
//		getSceneManager()->getDevice()->drawBox(getTransformedBoundingBox(),math::matrix4x4::Identity,video::MR_LINES);
	}
/*
	video::RenderMaterial* mtrl=gMaterialResourceManager.getMaterial(mT("DebuDrawMtrl"));
	if(!mtrl){
		mtrl=new video::RenderMaterial();
		gMaterialResourceManager.addResource(mtrl,mT("DebuDrawMtrl"));
		mtrl->setRenderState(video::RS_Lighting,video::ES_DontUse);
	}

	if(DrawDebugObjects()){
		m_device->useMaterial(mtrl);
		if(DrawBoundingBox()){
			math::box3d box=getBoundingBox();
			const math::matrix4x4 &mat=getAbsoluteTransformation();
			math::vector3d edges[8];
			box.getEdges(edges);
			edges[0]=mat*edges[0];
			box.reset(edges[0]);
			for(int i=1;i<8;++i){
				edges[i]=mat*edges[i];
				box.AddPoint(edges[i]);
			}
			m_device->drawBox(box,math::matrix4x4(),video::MR_LINES);
		//	device->drawBox(box,absTransformation,MR_LINES);
		}
		
	}
	
	{
		for(int i=0;i<m_attachedNodes.size();++i)
			m_attachedNodes[i]->render(pass);
	}
*/
	/*
	std::list<GCPtr<ISceneNode>>::iterator it=Children.begin();
	for(;it!=Children.end();++it)
	{
		(*it)->render();
	}*/
}


void ISceneNode::removeFromParent(bool parentSpace)
{
	if(m_parent){
		m_parent->removeChild(this,parentSpace);
		//m_parent=0;
	}
}

void ISceneNode::removeChild(IMovable*elem,bool parentSpace)
{
	MovableNodeList::iterator it=m_Children.begin();
	MovableNodeList::iterator end=m_Children.end();
	for(;it!=end;++it)
	{
		if((*it).pointer()==elem)
		{
			/*
			if(sceneMngr)
				sceneMngr->removeSceneNode(elem);*/
			if(parentSpace)
			{
				elem->setPosition(elem->getAbsolutePosition());
				elem->setOrintation(elem->getAbsoluteOrintation());
				elem->setScale(elem->getAbsoluteScale());
			}
			elem->setParent(0);
			m_Children.erase(it);
			return;
		}
	}
}

void ISceneNode::addChild(IMovableCRef elem,bool parentSpace)
{
	if(elem)
	{
		if (elem->getParent())
		{
			elem->removeFromParent();
		}
		if(parentSpace)
		{
			elem->setPosition(elem->getAbsolutePosition()-getAbsolutePosition());
			elem->setOrintation(getAbsoluteOrintation().inverse()*elem->getAbsoluteOrintation());
			if(getAbsoluteScale().x!=0 && getAbsoluteScale().y!=0 && getAbsoluteScale().z!=0)
				elem->setScale(elem->getAbsoluteScale()/getAbsoluteScale());
		}
		elem->setParent(this);
		m_Children.push_back(elem);
	}
}

math::matrix4x4&ISceneNode::getAbsoluteTransformation(){
// 	if(hasChanged())
 	updateAbsoluteTransformation();
	return absTransformation;
}
const math::matrix4x4&ISceneNode::getAbsoluteTransformation()const
{
	return absTransformation;
}
const math::matrix4x4&ISceneNode::getPrevAbsoluteTransformation()const
{
	return m_prevTransformation;
}

const math::matrix4x4 &ISceneNode::getRelativeTransformation()const
{
	return relTransformation;
}
void ISceneNode::updateRelativeTransformation()
{
	if(!hasChanged())return;

	m_orintation.toMatrix(relTransformation);
	if(m_scale!=1)
	{
		math::matrix4x4 m2;
		m2.setScale(m_scale);
		relTransformation=relTransformation*m2;
	}
	relTransformation.setTranslation(m_position);
	m_updateChilds=true;

	m_lastPosition=m_position;
	m_lastOrintation=m_orintation;
	m_lastScale=m_scale;
}

void ISceneNode::updateAbsoluteTransformation()
{
	bool shouldUpdate=false;
	shouldUpdate=hasChanged();
	if(getParent()){
		shouldUpdate=shouldUpdate || getParent()->NeedChildUpdate();
	}
	if(!shouldUpdate && false){
		m_updateChilds=false;
		return;
	}
	updateRelativeTransformation();

	math::vector3d absPos;
	if(m_parent)
	{
		absTransformation=m_parent->getAbsoluteTransformation()*getRelativeTransformation();

		m_absScale=m_scale*m_parent->getAbsoluteScale();
		m_absOrintaion=m_parent->getAbsoluteOrintation()*m_orintation;
		absPos = absTransformation.getTranslation();
		/*
		absPos=m_position*m_scale;
		parent->getAbsoluteTransformation().transformVectPost(absPos);

		m_absOrintaion.toMatrix(absTransformation);
		absTransformation.setTranslation(absPos);
		if(m_scale!=1)
		{
			math::matrix4x4 m2;
			m2.setScale(m_scale);
			absTransformation*=m2;
		}*/
	}
	else{
		absTransformation=getRelativeTransformation();
		absPos=m_position*m_scale;
		m_absScale=getScale();
		m_absOrintaion=m_orintation;
	}


	m_updateChilds=true;
	m_isNeedUpdate=false;

}
/*
void ISceneNode::updatePos()
{
	
	if(parent && parent!=(GCPtr<ISceneNode>)sceneMngr)
	{
		setParentPos(parent->getPos());
	}

	std::list<GCPtr<ISceneNode>>::iterator it=Children.begin();
	for(;it!=Children.end();++it)
		(*it)->updatePos();
}
*/

IMovable* ISceneNode::getElementById(int id,bool searchChildren)
{
	ISceneNode* n;

	MovableNodeList::iterator it=m_Children.begin();
	MovableNodeList::iterator end=m_Children.end();
	for(;it!=end;++it)
	{
		if((*it)->getID()==id)
			return (*it).pointer();
		if(searchChildren && (n=dynamic_cast<ISceneNode*>((*it).pointer())))
		{
			IMovable* m=n->getElementById(id,true);
			if(m)
				return m;
		}
	}
	return 0;
}


bool ISceneNode::NeedChildUpdate(){
	return m_updateChilds;
}
bool ISceneNode::hasChanged(){
	return m_lastPosition!=m_position ||
		m_lastOrintation!=m_orintation ||
		m_lastScale != m_scale;
}
void ISceneNode::_UpdateBoundingBox()
{
	if(m_attachedNodes.size()>0)
		m_boundingBox=m_attachedNodes[0]->getBoundingBox();
	else m_boundingBox=math::box3d::Empty;
	for(int i=1;i<m_attachedNodes.size();++i)
	{
		m_boundingBox.AddBox(m_attachedNodes[i]->getBoundingBox());
	}
}

xml::XMLElement* ISceneNode::loadXMLSettings(xml::XMLElement*root)
{
	xml::XMLElement*elem=IObject::loadXMLSettings(root);
	if(!elem)return 0;
	xml::XMLAttribute*attr=elem->getAttribute(mT("Orientation"));
	if(attr)
		setOrintation(core::StringConverter::toQuaternion(attr->value));

	xml::XMLElement* mpoints=elem->getSubElement(mT("MountPoints"));
	if(mpoints)
	{
		xml::xmlSubElementsMapIT it= mpoints->getElementsBegin();
		xml::xmlSubElementsMapIT end= mpoints->getElementsEnd();
		for(;it!=end;++it)
		{
			if((*it)->GetType()!=xml::ENT_Element)continue;
			xml::XMLElement* mp=dynamic_cast<xml::XMLElement*>(*it);
			if(mp->getName().equals_ignore_case(mT("MountPoint")))
			{
				ISceneNode*mpnt=new ISceneNode(mT(""),0,0);
				mpnt->loadXMLSettings(mp);
				addMountPoint(mpnt);
			}
		}
		
	}
	return elem;
}

xml::XMLElement* ISceneNode::exportXMLSettings(xml::XMLElement*elem){

	xml::XMLElement*node=IObject::exportXMLSettings(elem);

	node->addAttribute(mT("Orientation"),core::StringConverter::toString(getOrintation()));
	if(m_Children.size()>0)
	{
		xml::XMLElement* child=new  xml::XMLElement(mT("Children"));
		node->addSubElement(child);
		MovableNodeList::iterator it=m_Children.begin();
		for(;it!=m_Children.end();++it){
			ISceneNode* node=dynamic_cast<ISceneNode*>((*it).pointer());
			if(node)
				node->exportXMLSettings(child);
		}
	}
	if(m_mountPoints.size()>0){
		xml::XMLElement* mpoints=new xml::XMLElement(mT("MountPoints"));
		MountPointsList::iterator it=m_mountPoints.begin();
		MountPointsList::iterator end=m_mountPoints.end();
		for(;it!=end;++it)
		{
			xml::XMLElement* mp=new xml::XMLElement(mT("MountPoint"));
			ISceneNode*mpnt=(*it);
			mpnt->exportXMLSettings(mp);
			mpoints->addSubElement(mp);
		}
		node->addSubElement(mpoints);
	}

	return node;
}



void ISceneNode::removeMountPoint(const core::string&name){
	MountPointsList::iterator it=m_mountPoints.begin();
	MountPointsList::iterator end=m_mountPoints.end();
	for(;it!=end;++it)
	{
		if((*it)->getNodeName().equals_ignore_case(name))
		{
			m_mountPoints.erase(it);
			return;
		}
	}
}
void ISceneNode::addMountPoint(ISceneNode *p){
	m_mountPoints.push_back(p);
	p->setParent(this);
}

ISceneNode* ISceneNode::getMountPoint(const core::string&point){
	MountPointsList::iterator p=m_mountPoints.begin();
	MountPointsList::iterator end=m_mountPoints.end();
	for(;p!=end;++p)
	{
		if((*p)->getNodeName().equals_ignore_case(point))
		{
			return (*p);
		}
	}
	return 0;
}

void ISceneNode::attachToMountPoint(const core::string&point,IMovable*elem){

	ISceneNode* p=getMountPoint(point);
	if(p)
		p->addChild(elem);
}
const MountPointsList& ISceneNode::getMountPoints(){
	return m_mountPoints;
}



void ISceneNode::OnVisit(ISceneVisitor*visitor)
{
	//IMovable::OnVisit(visitor,true);

	//if(!visited)
	visitor->Visit(this);

	MovableNodeList::iterator it = m_Children.begin();
	MovableNodeList::iterator end = m_Children.end();
	for (; it != end; ++it)
	{
		(*it)->OnVisit(visitor);
	}
	/*
	for (int i=0;i<GetAttachedNodesCount();++i)
	{
		GetAttachedNode(i)->OnVisit(visitor);
	}
*/
}

void ISceneNode::SaveToDesc(ISceneNodeDesc* desc)const
{
	desc->position=getPosition();
	desc->orintation=getOrintation();
	desc->scale=getScale();
	desc->visible=isVisible();
	desc->cullType=getCullingType();
	desc->viewDist=getViewDistance();
}
void ISceneNode::LoadFromDesc(const ISceneNodeDesc* desc)
{
	setPosition(desc->position);
	setOrintation(desc->orintation);
	setScale(desc->scale);

	setVisible(desc->visible);
	setCullingType(desc->cullType);
	setViewDistance(desc->viewDist);
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Name,ISceneNode,string,mT("Name"),EPBT_Struct,mT("Node Name"),mT(""));
IMPLEMENT_PROPERTY_TYPE_GENERIC(Name,ISceneNode,core::string,setNodeName,getNodeName,,,false)



//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Position,ISceneNode,vector3d,mT("Position"),EPBT_Struct,mT("Node Position"),);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Position,ISceneNode,math::vector3d,setPosition,getPosition,core::StringConverter::toString,core::StringConverter::toVector3d,false)


//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Orintation,ISceneNode,quaternion,mT("Orintation"),EPBT_Struct,mT("Node Orintation"),);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Orintation,ISceneNode,math::quaternion,setOrintation,getOrintation,core::StringConverter::toString,core::StringConverter::toQuaternion,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Scale,ISceneNode,vector3d,mT("Scale"),EPBT_Struct,mT("Node Scale"),);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Scale,ISceneNode,math::vector3d,setScale,getScale,core::StringConverter::toString,core::StringConverter::toVector3d,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(ID,ISceneNode,uint,mT("ID"),EPBT_Basic,mT("Node ID"),0);
IMPLEMENT_PROPERTY_TYPE_GENERIC(ID,ISceneNode,uint,setID,getID,core::StringConverter::toString,core::StringConverter::toUInt,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(DrawBoundingBox,ISceneNode,bool,mT("DrawBoundingBox"),EPBT_Basic,mT("Draw Node's Bounding Box"),false);
IMPLEMENT_PROPERTY_TYPE_GENERIC(DrawBoundingBox,ISceneNode,bool,setDrawBoundingBox,getDrawBoundingBox,core::StringConverter::toString,core::StringConverter::toBool,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Visible,ISceneNode,bool,mT("Visible"),EPBT_Basic,mT("Node Visibility"),true);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Visible,ISceneNode,bool,setVisible,isVisible,core::StringConverter::toString,core::StringConverter::toBool,false)


}
}

