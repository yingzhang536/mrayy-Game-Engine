

/********************************************************************
	created:	2013/03/12
	created:	12:3:2013   14:41
	filename: 	D:\Development\mrayEngine\Projects\TELUBee\TBeeOptiTrackObject.h
	file path:	D:\Development\mrayEngine\Projects\TELUBee
	file base:	TBeeOptiTrackObject
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/
#ifndef _TBeeOptiTrackObject_
#define _TBeeOptiTrackObject_



namespace mray
{
namespace animation
{
	class OptiTrackClient;
}
namespace TBee
{

	class TBeeOptiTrackObjectListener;
class TBeeOptiTrackObject
{
protected:
	TBeeOptiTrackObjectListener* m_listener;
	animation::OptiTrackClient* m_tracker;
	math::vector3d m_pos;
	math::vector3d m_rot;
public:
	TBeeOptiTrackObject();
	virtual~TBeeOptiTrackObject();

	void Connect(const core::string& serverIP,const core::string& localIP);
	void Disconnect();
	bool IsConnected();

	const math::vector3d& GetPos(){return m_pos;}
	const math::vector3d& GetRotation(){return m_rot;}

	void DataReceived(const math::vector3d &pos,const math::vector3d &rot);
};


}
}

#endif
