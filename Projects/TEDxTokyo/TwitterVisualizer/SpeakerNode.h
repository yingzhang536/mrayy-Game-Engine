

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

public:
	SpeakerNode(ted::CSpeaker* s);
	virtual~SpeakerNode();

	void Draw();
};

}
}

#endif // SpeakerNode_h__
