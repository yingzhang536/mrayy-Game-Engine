


/********************************************************************
	created:	2011/12/03
	created:	3:12:2011   20:26
	filename: 	d:\Development\mrayEngine\Engine\mrayAI\BHSequenceNode.h
	file path:	d:\Development\mrayEngine\Engine\mrayAI
	file base:	BHSequenceNode
	file ext:	h
	author:		MHD Yamen Saraiji
	
	purpose:	
*********************************************************************/


#ifndef __BHSequenceNode__
#define __BHSequenceNode__

#include "IBHNode.h"

namespace mray
{
namespace AI
{

class MRAY_AI_DLL BHSequenceNode:public IBHNode
{
private:
protected:
	BHNodeList m_nodes;
public:
	BHSequenceNode();
	virtual~BHSequenceNode();

	virtual bool CheckCondition();
	virtual bool Access(float dt);

	void AddNode(IBHNode* node);
	void RemoveNode(IBHNode* node);
	void ClearNodes();

	const BHNodeList& GetNodes();
};

}
}

#endif