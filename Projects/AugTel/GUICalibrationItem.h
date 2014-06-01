
#ifndef GUICalibrationItem_h__
#define GUICalibrationItem_h__

#include "IGUIPanelElement.h"
#include "GUICalibrationItemLayout.h"
#include "GUIElementFactoryDef.h"

namespace mray
{
namespace GUI
{
	
class GUICalibrationItem :public IGUIPanelElement, public GUICalibrationItemLayout
{
	DECLARE_RTTI;
public:
	static const GUID ElementType;
protected:
public:
	GUICalibrationItem(IGUIManager* m);
	virtual~GUICalibrationItem();

};
DECLARE_ELEMENT_FACTORY(GUICalibrationItem);

}
}

#endif // GUICalibrationItem_h__
