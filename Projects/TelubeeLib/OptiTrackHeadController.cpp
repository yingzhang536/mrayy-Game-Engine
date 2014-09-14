

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


bool OptiTrackHeadController::GetHeadOrientation(math::quaternion& v)
{
	math::quaternion* q= AppData::Instance()->optiDataSource->GetOrientationByID(m_headID);
	if (!q)
		return false;
	math::vector3d a;
	q->toEulerAngles(a);
	
	v= math::quaternion(-a.x,a.y,-a.z);
	
	return true;
}
bool OptiTrackHeadController::GetHeadPosition(math::vector3d& v)
{
	math::vector3d* p=AppData::Instance()->optiDataSource->GetPositionByID(m_headID);
	if (!p)
		return false;
	v=*p;
	return true;
}

}
}
