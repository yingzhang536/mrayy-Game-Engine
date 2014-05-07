

#ifndef SpeakerNode_h__
#define SpeakerNode_h__

namespace mray
{
	namespace ted
	{
		class CSpeaker;
	}
namespace scene
{

class SpeakerNode
{
protected:
	ted::CSpeaker* m_speaker;
public:
	SpeakerNode();
	virtual~SpeakerNode();

};

}
}

#endif // SpeakerNode_h__
