



/********************************************************************
	created:	2013/04/03
	created:	3:4:2013   17:08
	filename: 	F:\Development\mrayEngine\Projects\TELUBee\IHMDRenderer.h
	file path:	F:\Development\mrayEngine\Projects\TELUBee
	file base:	IHMDRenderer
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __IHMDRenderer__
#define __IHMDRenderer__


#include "IRenderingState.h"

namespace mray
{
	namespace video
	{
		class IRenderArea;
	}
	namespace xml
	{
		class XMLElement;
	}
namespace TBee
{

	class Application;
class IHMDRenderer
{
protected:
public:
	IHMDRenderer(){}
	virtual ~IHMDRenderer(){}

	virtual void Init(Application*app)=0;
	virtual void Render(const math::rectf &rc,ETargetEye eye)=0;
	virtual void Update(float dt)=0;

	virtual void Enter()=0;
	virtual void Exit()=0;

	virtual void LoadFromXML(xml::XMLElement* e)=0;
};

}
}

#endif
