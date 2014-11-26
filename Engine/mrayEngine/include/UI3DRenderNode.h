

#ifndef UI3DRenderNode_h__
#define UI3DRenderNode_h__

#include "MeshRenderableNode.h"
#include "IRenderTarget.h"

namespace mray
{
	namespace GUI
	{
		class IGUIManager;
	}
	namespace video
	{
		class RenderPass;
	}
namespace scene
{
	
class MRAY_DLL UI3DRenderNode :public MeshRenderableNode
{

	DECLARE_RTTI
protected:

	GUI::IGUIManager* m_UIManager;
	math::vector2df m_UISize;
	video::IRenderTargetPtr m_UIRenderTarget;
	video::ITexturePtr m_UITexture;

	video::RenderPass* m_mtrl;
	bool m_updated;

	void _createMesh();
public:
	UI3DRenderNode();
	virtual ~UI3DRenderNode();

	void SetUISize(const math::vector2d& sz);

	void SetUIManager(GUI::IGUIManager* m);
	GUI::IGUIManager* GetUIManager(GUI::IGUIManager* m){ return m_UIManager; }

	virtual bool preRender(IRenderPass*pass);
	virtual bool render(IRenderPass*pass);
	virtual void update(float dt);
};

}
}

#endif // UI3DRenderNode_h__

