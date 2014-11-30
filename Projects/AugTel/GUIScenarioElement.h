


#ifndef GUIScenarioElement_h__
#define GUIScenarioElement_h__

#include "GUIScenarioLayout.h"

namespace mray
{
namespace GUI
{
	
class GUIScenarioElement :public IGUIPanelElement, public GUIScenarioLayout
{
	DECLARE_RTTI;
	static const GUID ElementType;
protected:
	struct ScenarioInfo
	{
		core::string image;
	};

	enum EState
	{
		EIdle,
		EStart,
		EDone,
		EWait
	} m_state;


	int m_currentScenario;
	std::vector<ScenarioInfo> m_scenarios;

	float m_maxTime;
	float m_startTime;
	float m_fadeSpeed;
public:
	GUIScenarioElement(IGUIManager* m);
	virtual ~GUIScenarioElement();

	void AddScenario(const core::string& image);

	void Reset();

	bool IsDone();

	//return true if not done
	bool Next();

	virtual void Update(float dt);

	virtual void DoneLoadingElements();
	virtual void LoadXMLSettings(xml::XMLElement*e);
};
DECLARE_ELEMENT_FACTORY(GUIScenarioElement);

}
}

#endif // GUIScenarioElement_h__
