

#include "stdafx.h"
#include "OptiTrackHeadController.h"
#include "AppData.h"
#include "OptiTrackDataSource.h"

namespace mray
{
namespace TBee
{

OptiTrackHeadController::OptiTrackHeadController(int headId)
{
	m_headID = headId;
}
OptiTrackHeadController::~OptiTrackHeadController()
{
}


math::quaternion OptiTrackHeadController::GetHeadOrientation()
{
	math::quaternion* q= AppData::Instance()->optiDataSource->GetOrientationByID(m_headID);
	if (!q)
		return math::quaternion::Identity;
	math::vector3d a;
	q->toEulerAngles(a);
	
	return math::quaternion(-a.x,a.y,-a.z);
}
math::vector3d OptiTrackHeadController::GetHeadPosition()
{
	math::vector3d* v=AppData::Instance()->optiDataSource->GetPositionByID(m_headID);
	if (!v)
		return math::vector3d::Zero;
	return *v;
}

}
}
