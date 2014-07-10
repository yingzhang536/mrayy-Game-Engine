

#ifndef VTBaseState_h__
#define VTBaseState_h__



namespace mray
{
namespace AugTel
{
	
class VTBaseState 
{
protected:
public:
	VTBaseState();
	virtual ~VTBaseState();

	void OnInit();
	void OnEnter();
	void OnExit();
	
	void Update(float dt);

	void CalibratePosition();
	void CalibrateGloves();
};

}
}

#endif // VTBaseState_h__

