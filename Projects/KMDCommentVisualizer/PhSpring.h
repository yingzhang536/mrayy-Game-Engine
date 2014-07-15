

#ifndef PhSpring_h__
#define PhSpring_h__



namespace mray
{
namespace physics
{

	class PhNode;
	
class PhSpring
{
protected:
	PhNode* m_a;
	PhNode* m_b;
	float m_strength;
	float m_damp;
	float m_length;
public:
	PhSpring(PhNode* a, PhNode* b, float strength,float length);
	virtual ~PhSpring();

	void SetDamping(float d){ m_damp = d; }
	float GetDamping(){ return m_damp; }

	void SetStrength(float s){ m_strength = s; }
	float GetStrength(){ return m_strength; }

	void Update(float dt);
	
};

}
}

#endif // PhSpring_h__