

/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   20:38
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\OptiTrackHeadController.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	OptiTrackHeadController
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __OptiTrackHeadController__
#define __OptiTrackHeadController__


#include "IHeadController.h"

namespace mray
{
namespace TBee
{
class OptiTrackHeadController:public IHeadController
{
protected:
	int m_headID;
public:
	OptiTrackHeadController(int headId);
	virtual~OptiTrackHeadController();

	void SetHeadID(int id){ m_headID = id; }

	virtual math::quaternion GetHeadOrientation() ;
	virtual math::quaternion GetHeadPosition();
};

}
}


#endif
