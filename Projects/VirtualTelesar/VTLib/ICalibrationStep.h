



/********************************************************************
	created:	2014/05/15
	created:	15:5:2014   19:23
	filename: 	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib\ICalibrationStep.h
	file path:	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib
	file base:	ICalibrationStep
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __ICalibrationStep__
#define __ICalibrationStep__




namespace mray
{
namespace GUI
{
	class IGUIElement;
	class IGUIManager;
}
namespace VT
{

class ICalibrationStep
{
protected:
	core::string m_name;
public:
	ICalibrationStep(const core::string& name):m_name(name){}
	virtual ~ICalibrationStep(){}

	const core::string& GetName()const{ return m_name; }

	virtual bool IsCompleted() const = 0;
	virtual void Update(float dt) = 0;

	virtual void LoadGUI(GUI::IGUIManager* m) = 0;
	virtual GUI::IGUIElement* GetGUIElement() = 0;
};

}
}

#endif
