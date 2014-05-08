

#ifndef SpeakerNode_h__
#define SpeakerNode_h__

#include "ITedNode.h"

namespace mray
{
	namespace ted
	{
		class CSpeaker;
	}
namespace scene
{

class SpeakerNode :public ITedNode
{
protected:
	ted::CSpeaker* m_speaker;

	msa::physics::Particle2D* m_phNode;
public:
	SpeakerNode(ted::CSpeaker* s);
	virtual~SpeakerNode();

	void SetPhysics(msa::physics::Particle2D* n){ m_phNode = n; }

	void Draw();
};

}
}

#endif // SpeakerNode_h__
