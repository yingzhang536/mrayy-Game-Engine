


#ifndef AtlasPacker_h__
#define AtlasPacker_h__

#include "ITexture.h"

namespace mray
{
namespace video
{
	class AtlasPackerImpl;

class MRAY_DLL AtlasPacker
{
protected:
	AtlasPackerImpl* m_impl;
public:
	AtlasPacker();
	virtual ~AtlasPacker();

	void SetMaxSize(const math::vector2d& size);
	const math::vector2d& GetMaxSize()const;
	
	void SetPadding(int p);
	int GetPadding()const;
	
	video::ITexturePtr GetTexture()const;


	bool AddTexture(video::ITexture* tex);
	bool PackTextures(video::ITexture** textures,int count);
	void Clear();

	int GetTexturesCount()const;
	const math::rectf* GetTextureCoords()const;
	const math::rectf* GetTextureCoord(int texture)const;
};

}
}


#endif // AtlasPacker_h__
