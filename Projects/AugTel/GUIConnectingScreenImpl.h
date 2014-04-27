

#ifndef GUIConnectingScreenImpl_h__
#define GUIConnectingScreenImpl_h__


#include "GUIConnectingScreen.h"

namespace mray
{
namespace GUI
{
	
class GUIConnectingScreenImpl:public GUIConnectingScreen
{
protected:
	core::UTFString m_status;
public:
	GUIConnectingScreenImpl();
	virtual ~GUIConnectingScreenImpl();

	void SetStatus(const core::UTFString& st,bool connecting);

	void ClearStatus();

	virtual void DoneLoadingElements();
};

}
}

#endif // GUIConnectingScreenImpl_h__
