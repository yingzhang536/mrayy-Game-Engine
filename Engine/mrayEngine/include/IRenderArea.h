

/********************************************************************
	created:	2012/07/11
	created:	11:7:2012   0:02
	filename: 	d:\Development\mrayEngine\Engine\mrayEngine\include\IRenderArea.h
	file path:	d:\Development\mrayEngine\Engine\mrayEngine\include
	file base:	IRenderArea
	file ext:	h
	author:		MHD YAMEN SARAIJI
	
	purpose:	
*********************************************************************/
#ifndef ___IRenderArea___
#define ___IRenderArea___

#include "GCPtr.h"

#include "SColor.h"
#include "ITexture.h"


namespace mray
{
namespace video
{

class IRenderArea
{
protected:
public:
	IRenderArea(){}
	virtual~IRenderArea()
	{
	}
	//virtual math::vector2di getPosition(){ return math::vector2di::Zero; }
	virtual math::vector2di getSize()=0;

	virtual const ITexturePtr& getColorTexture(int i=0)=0;

	virtual void GetParameter(const core::string& name,void* param){}
	
};

}
}

#endif