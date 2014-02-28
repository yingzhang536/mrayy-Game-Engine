

/********************************************************************
	created:	2014/01/18
	created:	18:1:2014   20:36
	filename: 	C:\Development\mrayEngine\Projects\TelubeeLib\IHeadController.h
	file path:	C:\Development\mrayEngine\Projects\TelubeeLib
	file base:	IHeadController
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IHeadController__
#define __IHeadController__




namespace mray
{
namespace TBee
{
class IHeadController
{
protected:
public:
	IHeadController(){}
	virtual~IHeadController(){}


	virtual math::quaternion GetHeadOrientation() = 0;
	virtual math::vector3d GetHeadPosition() = 0;
};

}
}


#endif
