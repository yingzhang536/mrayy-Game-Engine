
#include "stdafx.h"
#include "PAVelocityOverTimeInterpolater.h"


namespace mray
{
namespace scene
{


PAVelocityOverTimeInterpolater::PAVelocityOverTimeInterpolater()
{
}


PAVelocityOverTimeInterpolater::~PAVelocityOverTimeInterpolater()
{
}



void PAVelocityOverTimeInterpolater::applyOnParticle(IParticle*p, float dt)
{
	float r = math::Randomizer::rand01();
	p->velocity+=(m_minVel*r+m_maxVel*(1-r))*dt;
}


}
}

