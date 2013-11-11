

#ifndef ___GUIMANAGER___
#define ___GUIMANAGER___

#include "CompileConfig.h"

#include "IGUIManager.h"

namespace mray
{

class BenchmarkItem;

namespace video
{
	class ITexture;
}

namespace GUI
{

class IGUITheme;
class IFont;
class LanguageFontSpecification;

class IGUIRenderer;

class IGUIElementFactory;

class MRAYGUI_API GUIManager:public IGUIManager
{
protected:

	IGUITheme*		m_Skin;

	IGUIRenderer*	m_renderQueue;

	LanguageFontSpecification*	m_specifications;

	BenchmarkItem* m_benchmarkItem;
	BenchmarkItem* m_renderBI;
	BenchmarkItem* m_updateBI;

	void _loadDefaultFactories();

public:

	GUIManager(video::IVideoDevice*dev);
	virtual ~GUIManager();

	virtual void SetDevice(video::IVideoDevice* dev);

	virtual void		SetActiveTheme(IGUITheme* theme);
	virtual IGUITheme*	GetActiveTheme();

	virtual LanguageFontSpecification* GetLanguageSpecification();

	virtual void RegisterBenchmark();
	virtual void UnregisterBenchmark();

	virtual IGUIRenderer* GetRenderQueue();


	virtual IGUIElement*	GetElementFromPoint(float x,float y);

	virtual bool OnEvent(Event* event);
	virtual void DrawAll(video::IRenderArea*vp);

	virtual void Update(float dt);

	virtual IGUIElement* CreateElement(const GUID& type);

};

}
}

#endif
