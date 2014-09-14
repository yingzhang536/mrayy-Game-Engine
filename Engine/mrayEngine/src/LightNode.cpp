#include "stdafx.h"


#include "LightNode.h"
#include "ISceneManager.h"
#include "RenderMaterial.h"
#include "IVideoDevice.h"
#include "IMeshBuffer.h"
#include "ShadowRenderPass.h"
#include "MathUtil.h"

#include "EnumManager.h"
#include "ISceneVisitor.h"
#include "ISceneObjectsDesc.h"

namespace mray{
namespace scene{

	


LightNode::LightNode(const  core::string&name,int id,ISceneManager* sceneMngr):
ISceneNode(name,id,sceneMngr)
{

	fillProperties();
	m_ambientColor=PropertyTypeAmbient::instance.GetDefaultValue();
	m_diffuseColor=PropertyTypeDiffuse::instance.GetDefaultValue();
	m_specularColor=PropertyTypeSpecular::instance.GetDefaultValue();
	m_target=PropertyTypeTarget::instance.GetDefaultValue();
	m_radius=PropertyTypeRadius::instance.GetDefaultValue();
	m_castShadows=PropertyTypeCastShadows::instance.GetDefaultValue();
	m_enabled=PropertyTypeEnable::instance.GetDefaultValue();

	m_renderPass=new ShadowRenderPass(this);

	projectionMatrix=math::MathUtil::CreateProjectionMatrixPerspectiveFov(math::OneFourthPI32,4.0f/3.0f,1,500);
	updateFrustrum();
	sceneMngr->addPassSubscriber(this);

}

LightNode::~LightNode()
{
	m_sceneMngr->removePassSubscriber(this);
}

int LightNode::GetNodeType()
{
	return ENodeType_Light;
}

bool LightNode::isEnabled()const
{
	return m_enabled;
}
bool LightNode::setEnabled(const bool& e)
{
	m_enabled=e;
	return true;
}

void LightNode::fillProperties()
{
	CPropertieDictionary* dic=0;
	if(CreateDictionary(&dic))
	{
		PropertyTypeType::instance.SetEnum(EnumManager::getInstance().getEnumInfo(mT("ELightType")));
		dic->addPropertie(&PropertyTypeEnable::instance,mT("Light"));
		dic->addPropertie(&PropertyTypeCastShadows::instance,mT("Light"));
		dic->addPropertie(&PropertyTypeTarget::instance,mT("Light"));
		dic->addPropertie(&PropertyTypeRadius::instance,mT("Light"));
		dic->addPropertie(&PropertyTypeAmbient::instance,mT("Light"));
		dic->addPropertie(&PropertyTypeDiffuse::instance,mT("Light"));
		dic->addPropertie(&PropertyTypeSpecular::instance,mT("Light"));
		dic->addPropertie(&PropertyTypeType::instance,mT("Light"));
	}
}

void LightNode::preRender()
{
	ISceneNode::preRender();
}
 
void LightNode::render(IRenderPass*pass)
{
	video::IVideoDevice*dev=m_sceneMngr->getDevice();
	if(!dev)return;
	
	//dev->useMaterial(m_mat);
	if(m_type==LT_DirectionalLight)
	{/*
		math::vector3d t=m_target;
		t.invert();
		lightProp.target=t;
		if(lightProp.target().LengthSQ()==0)
			lightProp.target=math::vector3d(0,-1,0);
		else {
			t=lightProp.target;
			lightProp.target=t.Normalize();
		}*/
	}/*
	if(isDrawBoundingBox())
	{
	//	dev->pushMatrix();
	//	dev->setTranslation(position);
		math::box3d box;
		box.MinP=-0.1;
		box.MaxP=0.1;
		dev->drawBox(box,getAbsoluteTransformation(),MR_LINES);
	//	dev->popMatrix();
	}*/
	dev->addDynamicLight(this);
	ISceneNode::render(pass);
}

void LightNode::update(float dt)
{
	ISceneNode::update(dt);

	m_dir=getViewPos()-getTarget();
	m_dir.Normalize();

}
void LightNode::updateFrustrum()
{
	m_prevViewMatrix=viewMatrix;
	math::vector3d pos=getAbsolutePosition();

	math::vector3d up=getOrintation()*math::vector3d(0,1,0);
	//	up.Normalize();

	//	math::vector3d tg=target;
	//tg.Normalize();

	float dp=m_target.dotProduct(up);
	if ((dp > -1.0001f && dp < -0.9999f) ||
		(dp < 1.0001f && dp > 0.9999f))
		up.x += 0.5f;

	viewMatrix=math::MathUtil::CreateLookAtMatrix(pos,m_target,up);

	math::matrix4x4 mat=viewMatrix*projectionMatrix;
	m_frustrum.viewPos=getAbsolutePosition();
	m_frustrum.set(mat);

}

const math::vector3d& LightNode::getDir()const
{
	return m_dir;
}


void LightNode::setProjection(const math::matrix4x4&m)
{
	projectionMatrix=m;
}


const math::matrix4x4& LightNode::getProjectionMatrix()const{return projectionMatrix;}
const math::matrix4x4& LightNode::getViewMatrix()const{return viewMatrix;}
const math::matrix4x4& LightNode::getPrevViewMatrix()const{return m_prevViewMatrix;}

const math::matrix4x4& LightNode::getShadowViewProjMatrix(){return m_shadowViewProjMatrix;}

math::vector3d LightNode::getViewPos()const{return getAbsolutePosition();}
const math::Frustum*LightNode::getViewFrustrum()const {return &m_frustrum;}

math::box3d LightNode::getTransformedBoundingBox(){
	return getBoundingBox()+getPosition();
}

void LightNode::GetRenderPasses(std::list<IRenderPass*>& pOut)
{
	if(m_castShadows && isEnabled())
	{
		pOut.push_back(m_renderPass);
	}
}


void LightNode::setShadowMap(const video::IRenderTargetPtr& m)
{
	m_shadowMap=m;
}
const video::IRenderTargetPtr& LightNode::getShadowMap()const
{
	return m_shadowMap;
}


bool LightNode::setTarget(const math::vector3d& val){
	m_target=val;
	return true;
}
const math::vector3d& LightNode::getTarget()const
{
	return m_target;
}

bool LightNode::setType(const ELightType& type){m_type=type;return true;}
ELightType LightNode::getType()const{return m_type;}

bool LightNode::isCastShadows()const{return m_castShadows;}
bool LightNode::setCastShadows(const bool& cs){m_castShadows=cs;return true;}


bool LightNode::setAmbient(const video::SColor&  val){
	m_ambientColor=val;
	return true;
}
const video::SColor& LightNode::getAmbient()const{
	return m_ambientColor;
}
bool LightNode::setDiffuse(const video::SColor&  val){
	m_diffuseColor=val;
	return true;
}
const video::SColor&  LightNode::getDiffuse()const{
	return m_diffuseColor;
}
bool LightNode::setSpecular(const video::SColor&  val){
	m_specularColor=val;
	return true;
}
const  video::SColor&  LightNode::getSpecular()const{
	return m_specularColor;
}
bool LightNode::setRadius(const float &val){
	m_radius=val;
	return true;
}
float LightNode::getRadius()const{
	return m_radius;
}


void LightNode::prepareForShadowPass(IViewNode* cam,IRenderPass*pass)
{
	if(!cam)
		return;
	math::vector3d lightPosition;
	math::matrix4x4 lightRotation;
#define UNIT_CUBE
#ifdef UNIT_CUBE
	math::matrix4x4 invLightRotation;
	math::matrix4x4 lightView ;
	math::matrix4x4 lightProjection ;

	math::vector3d lightDir=getViewPos() - getTarget();
	lightDir.Normalize();

	//lightDir=-lightDir;

	lightRotation=math::MathUtil::CreateLookAtMatrix(math::vector3d::Zero,lightDir,math::vector3d::YAxis);
	//lightRotation=math::MathUtil::CreateLookAtMatrix(getViewPos(),getTarget(),math::vector3d::YAxis);
	lightRotation.getInverse(invLightRotation);

	math::vector3d frustumCorners[8];
	////math::Frustum *camFrust=ActiveCamera->getViewFrustrum();//&m_lightFrust;//
	math::Frustum camFrust;
	camFrust.set(getProjectionMatrix()*cam->getViewMatrix());

	camFrust.boundingBox.getEdges(frustumCorners);

	for(int i=0;i<8;i++){
		frustumCorners[i]=lightRotation*(frustumCorners[i]);
	}
	math::box3d lightBox;
	lightBox.reset(frustumCorners[0]);
	for(int i=1;i<8;i++){
		lightBox.AddPoint(frustumCorners[i]);
	}


	math::vector3d boxSize = (lightBox.MaxP-lightBox.MinP);
	math::vector3d halfBoxSize = boxSize * 0.5f;


	 lightPosition=lightBox.MinP+halfBoxSize;

	lightPosition.z=lightBox.MinP.z;

	lightPosition=invLightRotation*(lightPosition);
 
	lightView=math::MathUtil::CreateLookAtMatrix(lightPosition, 
		lightPosition - lightDir, 
		math::vector3d::YAxis);

	// Create the projection matrix for the light
	// The projection is orthographic since we are using a directional light
	lightProjection=math::MathUtil::CreateProjectionMatrixOrtho(-boxSize.x,boxSize.x,-boxSize.y, boxSize.y, 
		-boxSize.z, boxSize.z);


	m_shadowViewProjMatrix= lightProjection * lightView;
#else

	lightPosition=getAbsolutePosition();

	lightRotation=math::MathUtil::CreateLookAtMatrix(lightPosition,getAbsoluteOrintation());
	//projectionMatrix=math::MathUtil::CreateProjectionMatrixPerspectiveFov(math::HalfPI32,1,0.1,10);

	m_shadowViewProjMatrix= projectionMatrix*lightRotation ;
#endif
	m_frustrum.viewPos=lightPosition;
	m_frustrum.set(m_shadowViewProjMatrix);
}

void LightNode::OnVisit(ISceneVisitor*visitor)
{
	ISceneNode::OnVisit(visitor);
	visitor->Visit(this);
}


bool LightNode::isCulling(ICullable *node)const
{
	if(!node)return 1;
	if(node->getCullingType()==SCT_NONE)return false;

	//culling by bounding box

	math::box3d box=node->getTransformedBoundingBox();
	/*	node->getAbsoluteTransformation().transformBox(box);

	math::matrix4x4 &mat=node->getAbsoluteTransformation();
	math::vector3d edges[8];
	box.getEdges(edges);
	mat.transformVectPre(edges[0]);

	box.reset(edges[0]);
	for(int i=1;i<8;++i){
	mat.transformVectPre(edges[i]);
	box.AddPoint(edges[i]);
	}
	*/
	return !getViewFrustrum()->boundingBox.intersectWithBox(box);
}


void LightNode::SaveToDesc(LightNodeDesc* desc)const
{
	ISceneNode::SaveToDesc(desc);
	desc->enabled=isEnabled();
	desc->castShadows=isCastShadows();
	desc->ambient=getAmbient();
	desc->diffuse=getDiffuse();
	desc->specular=getSpecular();
	desc->target=getTarget();
	desc->radius=getRadius();
	
}

void LightNode::LoadFromDesc(const LightNodeDesc* desc)
{
	ISceneNode::LoadFromDesc(desc);
	setEnabled(desc->enabled);
	setCastShadows(desc->castShadows);
	setAmbient(desc->ambient);
	setDiffuse(desc->diffuse);
	setSpecular(desc->specular);
	setTarget(desc->target);
	setRadius(desc->radius);
}

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Enable,LightNode,bool,mT("Enable"),EPBT_Basic,mT("Enable light"),true);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Enable,LightNode,bool,setEnabled,isEnabled,core::StringConverter::toString,core::StringConverter::toBool,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(CastShadows,LightNode,bool,mT("CastShadows"),EPBT_Basic,mT("Enable shadows"),false);
IMPLEMENT_PROPERTY_TYPE_GENERIC(CastShadows,LightNode,bool,setCastShadows,isCastShadows,core::StringConverter::toString,core::StringConverter::toBool,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Target,LightNode,vector3d,mT("Target"),EPBT_Struct,mT("Enable shadows"),);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Target,LightNode,math::vector3d,setTarget,getTarget,core::StringConverter::toString,core::StringConverter::toVector3d,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Ambient,LightNode,SColor,mT("Ambient"),EPBT_Struct,mT("Ambient color"),(1,1,1,0));
IMPLEMENT_PROPERTY_TYPE_GENERIC(Ambient,LightNode,video::SColor,setAmbient,getAmbient,core::StringConverter::toString,core::StringConverter::toColor,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Diffuse,LightNode,SColor,mT("Diffuse"),EPBT_Struct,mT("Diffuse color"),);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Diffuse,LightNode,video::SColor,setDiffuse,getDiffuse,core::StringConverter::toString,core::StringConverter::toColor,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Specular,LightNode,SColor,mT("Specular"),EPBT_Struct,mT("Specular color"),);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Specular,LightNode,video::SColor,setSpecular,getSpecular,core::StringConverter::toString,core::StringConverter::toColor,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Radius,LightNode,float,mT("Radius"),EPBT_Basic,mT("Lighting Radius"),100);
IMPLEMENT_PROPERTY_TYPE_GENERIC(Radius,LightNode,float,setRadius,getRadius,core::StringConverter::toString,core::StringConverter::toFloat,false)

//////////////////////////////////////////////////////////////////////////

IMPLEMENT_PROPERTY_TYPE_HEADER(Type,LightNode,enum,mT("Type"),EPBT_Enum,mT("Light type"),LT_PointLight);

LightNode::PropertyTypeType LightNode::PropertyTypeType::instance;
ELightType LightNode::PropertyTypeType::GetValue(CPropertieSet*object)const
{
	LightNode* o=dynamic_cast<LightNode*>(object);
	if(!o)
		return m_default;
	return o->getType();
}
bool LightNode::PropertyTypeType::SetValue(CPropertieSet*object,const ELightType& v)
{
	LightNode* o=dynamic_cast<LightNode*>(object);
	if(!o)return false;
	return o->setType(v);
}
core::string LightNode::PropertyTypeType::toString(CPropertieSet*object)const
{
	if(GetEnum())
		return GetEnum()->ToString(GetValue(object));
	return core::StringConverter::toString((int)GetValue(object),core::string(mT("ELightType")));
}
bool LightNode::PropertyTypeType::parse(CPropertieSet*reciver,const core::string&str)
{
	if(GetEnum())
	{
		if(!CheckIsValidValue(str))
			return false;
	}
	int v=0;
	if(GetEnum())
		v= GetEnum()->Parse(str);
	else
		core::StringConverter::parse(str,v,mT("ELightType"));
	return SetValue(reciver,(ELightType)v);
}
bool LightNode::PropertyTypeType::isDefault(CPropertieSet*reciver)const
{
	if(GetValue(reciver)==GetDefaultValue())
		return true;
	return false;
}

}
}

