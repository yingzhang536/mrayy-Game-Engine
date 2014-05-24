



/********************************************************************
	created:	2014/05/15
	created:	15:5:2014   19:26
	filename: 	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib\CalibrationManager.h
	file path:	F:\Development\mrayEngine\Projects\VirtualTelesar\VTLib
	file base:	CalibrationManager
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/

#ifndef __CalibrationManager__
#define __CalibrationManager__




namespace mray
{
namespace VT
{

	class ICalibrationStep;

	class CalibrationManager;
	class ICalibrationManagerListener
	{
	public:
		virtual void OnCalibrationReset(CalibrationManager* m){}
		virtual void OnCalibrationDone(CalibrationManager* m){}
		virtual void OnCalibrationNextStep(CalibrationManager* m,ICalibrationStep* s){}
	};

	// Used depending on the agent representation and required calibration steps
	class CalibrationManager :public ListenerContainer<ICalibrationManagerListener*>
{
protected:

	std::vector<ICalibrationStep*> m_calibSteps;
	int m_currentStep;

	DECLARE_FIRE_METHOD(OnCalibrationReset, (CalibrationManager* m), (m))
	DECLARE_FIRE_METHOD(OnCalibrationDone, (CalibrationManager* m), (m))
	DECLARE_FIRE_METHOD(OnCalibrationNextStep, (CalibrationManager* m, ICalibrationStep* s), (m,s))

	void _addCalibrationStep(ICalibrationStep* s);
public:
	CalibrationManager();
	virtual ~CalibrationManager();

	virtual void Reset();
	virtual bool IsDone();
	ICalibrationStep* GetCurrentCalibrationStep();

	virtual void Update(float dt);
};

}
}

#endif
