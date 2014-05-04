
#include "stdafx.h"
#include "AtlasPacker.h"

#include "BinPacker.h"
#include "Engine.h"



namespace mray
{
namespace video
{


class AtlasPackerImpl
{
public:
	AtlasPackerImpl()
	{
	}

	video::ITexturePtr texture;
	math::vector2d maxSize;
	std::vector<math::rectf> packedTextures;
	int padding;

	core::BinPacker packer;
};


AtlasPacker::AtlasPacker()
{
	m_impl = new AtlasPackerImpl();
}

AtlasPacker::~AtlasPacker()
{
	delete m_impl;
}


void AtlasPacker::SetMaxSize(const math::vector2d& size)
{
	m_impl->maxSize = size;

	m_impl->texture = Engine::getInstance().getDevice()->createEmptyTexture2D(false);
	m_impl->texture->setMipmapsFilter(false);
	m_impl->texture->createTexture(math::vector3d(size.x, size.y, 1), video::EPixel_R8G8B8A8);
}

const math::vector2d& AtlasPacker::GetMaxSize()const
{
	return m_impl->maxSize;
}

video::ITexturePtr AtlasPacker::GetTexture()const
{
	return m_impl->texture;
}


bool AtlasPacker::AddTexture(video::ITexture* tex)
{
	const math::vector3d& sz = tex->getSize();
	core::BinPackerOutRect pack;
	return m_impl->packer.AddToPack(math::vector2d(sz.x, sz.y), pack,true);
}

bool AtlasPacker::PackTextures(video::ITexture** textures, int count)
{
	std::vector<math::vector2df> rects;
	rects.reserve(count);
	for (int i = 0; i < count; ++i)
	{
		const math::vector3d& sz = textures[i]->getSize();
		rects.push_back(math::vector2d(sz.x, sz.y));
	}

	std::vector<core::BinPackerOutRect> packs;
	bool ret= m_impl->packer.Pack(rects, packs, m_impl->maxSize);
	

	video::LockedPixelBox box= m_impl->texture->getSurface(0)->lock(math::box3d(0, math::vector3d(m_impl->maxSize.x, m_impl->maxSize.y,1 )),video::IHardwareBuffer::ELO_Normal);

	for (int i = 0; i < packs.size(); ++i)
	{
		video::ITexture* tex = textures[packs[i].ID];
		const math::vector3d& sz = tex->getSize();
		video::LockedPixelBox src = tex->getSurface(0)->lock(math::box3d(0, sz), video::IHardwareBuffer::ELO_ReadOnly);

		uchar* dptr = (uchar*)box.data;
		uchar* sptr = (uchar*)src.data;

		dptr += (int)(packs[i].pos.x + box.box.getWidth()*packs[i].pos.y) * 4;
		for (int y = 0; y < sz.y; ++y)
		{
			memcpy(dptr,sptr,(int)src.box.getWidth()*4);
			sptr += (int)src.box.getWidth() * 4;
			dptr += (int)(box.box.getWidth()*packs[i].pos.y) * 4;
		}

		tex->getSurface(0)->unlock();
	}

	m_impl->texture->getSurface(0)->unlock();
	return true;
}

void AtlasPacker::Clear()
{
	m_impl->packedTextures.clear();
}


int AtlasPacker::GetTexturesCount()const
{
	return m_impl->packedTextures.size();
}

const math::rectf* AtlasPacker::GetTextureCoords()const
{
	if (m_impl->packedTextures.size() == 0)
		return 0;
	return &m_impl->packedTextures[0];
}

const math::rectf* AtlasPacker::GetTextureCoord(int texture)const
{
	if (m_impl->packedTextures.size() <=texture)
		return 0;
	return &m_impl->packedTextures[texture];

}

}
}

