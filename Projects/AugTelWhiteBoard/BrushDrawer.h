

#ifndef BrushDrawer_h__
#define BrushDrawer_h__

#include "ILineDrawer.h"

namespace mray
{
	
class BrushDrawer:public ILineDrawer
{
protected:
	struct GroupInfo
	{
		int id;
		GCPtr<video::IRenderTarget> texture;
		math::vector2di currentPos;
		math::vector2di lastPos;

		video::SColor clr;
	};
	std::map<int, GroupInfo> m_groups;

	video::ITexturePtr m_brushTex;

	virtual void _ReceiveData(OS::IStream* stream);

	int m_activeGroup;

public:
	BrushDrawer();
	virtual ~BrushDrawer();


	virtual void AddGroup(int group) ;

	virtual void RemoveAt(const math::vector2d& pos) ;

	virtual bool RemoveLast(int group) ;

	virtual void DrawAt(const math::vector2d& pos, int group, const video::SColor& clr);
	virtual void DrawLine(const math::vector2d& a, const math::vector2d& b, int group, const video::SColor& clr);

	virtual bool OnEvent(Event* e) ;

	virtual void Draw(const math::rectf& vp) ;

	virtual void Update(float dt) ;
};

}

#endif // BrushDrawer_h__
