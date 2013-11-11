

#include "stdafx.h"
#include "TBeeOptiTrackObject.h"
#include "OptiTrackClient.h"

namespace mray
{
namespace TBee
{

	class TBeeOptiTrackObjectListener:public animation::IOptiTrackClientListener
	{
		TBeeOptiTrackObject* m_owner;
	public:
		TBeeOptiTrackObjectListener(TBeeOptiTrackObject* o)
		{
			m_owner=o;
		}
		virtual void OnOptiTrackData(animation::OptiTrackClient*client,const animation::OptiTrackRigidBody& body)
		{
			math::vector3d angles;
			body.GetOrintation().toEulerAngles(angles);
			m_owner->DataReceived(body.GetPosition(),angles);
		}

	};

TBeeOptiTrackObject::TBeeOptiTrackObject()
{
	m_listener=new TBeeOptiTrackObjectListener(this);
	m_tracker=new animation::OptiTrackClient();
	m_tracker->AddListener(m_listener);
}
TBeeOptiTrackObject::~TBeeOptiTrackObject()
{
	delete m_tracker;
	delete m_listener;
}

void TBeeOptiTrackObject::Connect(const core::string& serverIP,const core::string& localIP)
{
	m_tracker->Connect(animation::Opti_Unicast,serverIP,localIP,"239.255.30.99");
}
void TBeeOptiTrackObject::Disconnect()
{
	m_tracker->Disconnect();
}
bool TBeeOptiTrackObject::IsConnected()
{
	return m_tracker->IsConnected();
}

void TBeeOptiTrackObject::DataReceived(const math::vector3d &pos,const math::vector3d &rot)
{
	m_pos=pos*1000;
	m_rot=rot;
	m_rot.x=math::toRad(rot.x);
	m_rot.y=math::toRad(rot.y);
	m_rot.z=math::toRad(rot.z);
}

}
}

