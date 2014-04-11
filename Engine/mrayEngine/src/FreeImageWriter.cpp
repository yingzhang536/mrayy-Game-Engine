#include "stdafx.h"

#include "FreeImageWriter.h"
#include "FreeImageCodec.h"
#include "PixelUtil.h"
#include "TextureResourceManager.h"
#include "StringUtil.h"

namespace mray
{
namespace loaders
{

	bool FreeImageWriter::m_inited=false;

unsigned DLL_CALLCONV FreeImageWriter::FI_WriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
	unsigned sz=0;
	OS::IStream* stream=(OS::IStream*)handle;
	for (int i=0;i<count;++i)
	{
		sz+=stream->write(buffer,size);
	}
	return sz;
}
int DLL_CALLCONV FreeImageWriter::FI_SeekProc(fi_handle handle, long offset, int origin)
{
	OS::IStream* stream=(OS::IStream*)handle;
	OS::EStreamSeek sk;
	switch (origin)
	{
	case SEEK_SET:
		sk=OS::ESeek_Set;
		break;
	case SEEK_END:
		sk=OS::ESeek_End;
		break;
	default:
		sk=OS::ESeek_Cur;
	}
	return stream->seek(offset,sk);
}
long DLL_CALLCONV FreeImageWriter::FI_TellProc(fi_handle handle)
{
	OS::IStream* stream=(OS::IStream*)handle;
	return stream->getPos();
}

FreeImageWriter::FreeImageWriter()
:m_isDummy(true),m_FTType(FIF_UNKNOWN)
{
	FreeImageCodec::IncreaseRef();
	if(!m_inited)
	{
		m_inited=true;
		Initialize();
	}
}
FreeImageWriter::FreeImageWriter(const core::string& ext,FREE_IMAGE_FORMAT ftType)
:m_isDummy(false),m_FTType(ftType),m_ext(ext)
{
	m_extDesc=m_ext+mT(" Image Writer");
	FreeImageCodec::IncreaseRef();
}

FreeImageWriter::~FreeImageWriter()
{
	FreeImageCodec::DecreaseRef();
}

void FreeImageWriter::Initialize()
{
	for(int i=0;i<FreeImage_GetFIFCount();++i){
		FREE_IMAGE_FORMAT fft=(FREE_IMAGE_FORMAT)i;
		if(fft==FIF_PGMRAW || fft==FIF_PPMRAW || fft==FIF_PBMRAW)
			continue;
		core::string ext=core::StringConverter::toString(FreeImage_GetFIFExtensionList(fft));

		
		std::vector<core::string> stringVec=core::StringUtil::Split(ext,mT(","));
		for(int j=0;j<stringVec.size();j++){
			FreeImageWriter* w=new FreeImageWriter(stringVec[j],(FREE_IMAGE_FORMAT)i);
			gTextureResourceManager.addTextureWriter(w);
		}
	}
}

bool FreeImageWriter::write(video::ImageInfo*tex,OS::IStream* stream,video::ETextureType target)
{
	if(m_isDummy)
		return false;
	FreeImageIO fio;/*
	switch(tex->format)
	{
	case video::EPixel_R5G6B5:
	case video::EPixel_R5G6B5:
	case video::EPixel_R8G8B8:
	case video::EPixel_B8G8R8:
	case video::EPixel_R8G8B8A8:
	case video::EPixel_B8G8R8A8:
	case video::EPixel_X8B8G8R8:
	case video::EPixel_X8R8G8B8:
	case video::EPixel_Alpha4Luminance4:

	}*/
	fio.read_proc=0;
	fio.write_proc=FreeImageWriter::FI_WriteProc;
	fio.tell_proc=FreeImageWriter::FI_TellProc;
	fio.seek_proc=FreeImageWriter::FI_SeekProc;

	FREE_IMAGE_TYPE imageType;
	switch (tex->format)
	{
	case video::EPixel_R8G8B8:
	default:
		imageType=FIT_BITMAP;

	}
	
	int bpp=video::PixelUtil::getPixelDescription(tex->format).elemSizeB*8;
	uint srcPitch=tex->getPitch();

	uchar* pSrc=tex->imageData;
	FIBITMAP *dib=0;
	if(true)
	{

		dib = FreeImage_ConvertFromRawBits(pSrc, tex->Size.x, tex->Size.y, srcPitch, bpp, 0xFF0000, 0x00FF00, 0x0000FF, false);
	}else
	{
		dib=FreeImage_AllocateT(imageType,tex->Size.x,tex->Size.y,bpp);
		uchar* pDst=FreeImage_GetBits(dib);
		uint dstPitch=FreeImage_GetPitch(dib);

		for(int y=0;y<tex->Size.y;++y)
		{
			memcpy(pDst,pSrc,srcPitch);
			pSrc+=srcPitch;
			pDst+=dstPitch;
		}

		//	memcpy(pDst,pSrc,tex->imageDataSize);
	}
	FIMEMORY* mem=FreeImage_OpenMemory();
	FreeImage_SaveToMemory(m_FTType,dib,mem);

	BYTE*data;
	DWORD size;

	FreeImage_AcquireMemory(mem,&data,&size);

	stream->write(data,size);

	FreeImage_CloseMemory(mem);
	FreeImage_Unload(dib);
	return true;
}
bool FreeImageWriter::support(video::ETextureType type)
{
	if(m_isDummy)
		return false;
	return (type==video::ETT_2DTex);
}

bool FreeImageWriter::canWrite(const core::string& fileName)
{
	if(m_isDummy)
		return false;
	return fileName.find(m_ext.c_str())!=-1;
}
const core::string& FreeImageWriter::getExt()
{
	return m_ext;
}

const core::string& FreeImageWriter::getDescription()
{
	return m_extDesc;
}

}
}

