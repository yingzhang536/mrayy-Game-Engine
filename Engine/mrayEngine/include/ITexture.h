#ifndef ___ITexture___
#define ___ITexture___

#include "mString.h"

#include "SColor.h"
//#include "EDeviceType.h"

#include "IResource.h"
#include "videoCommon.h"

#include "PixelFormat.h"
#include "IHardwarePixelBuffer.h"
#include "ImageInfo.h"
#include "ListenerContainer.h"

namespace mray{
	namespace OS{
		class IStream;
	}
namespace video{
class IVideoDevice;
class ITexture;

class ITextureListener
{
public:
	virtual void OnTextureChanged(ITexture* tex){}
	virtual void OnTextureLoaded(ITexture* tex){}
	virtual void OnTextureUnloaded(ITexture* tex){}
};

class MRAY_DLL ITexture:public IResource,public ListenerContainer<GCPtr<ITextureListener>>
{
protected:
	struct rgba{uchar r,g,b,a;};
	struct rgb {uchar r,g,b;};

	uint m_maxMipmaps;
	bool m_isRenderTarget;
	bool m_resourceCreated;

	std::vector<OS::IStream*> m_loadStreams;
	std::vector<ImageInfoPtr> m_imageIfo;
	GCPtr<PixelFormat> m_pixelFormat;

	EPixelFormat m_format;

	ETextureType m_type;

	math::vector3di m_size;

	bool m_internal;


	virtual void _innerCreateTexture()=0;
	virtual void preLoadInternal(bool async);
	virtual void loadInternal(); 
	virtual void postLoadInternal();

	bool m_BilinearFilter;
	bool m_TrilinearFilter;
	bool m_MipmapFilter;
	bool m_AnisotropicFilter;

	DECLARE_FIRE_METHOD(OnTextureChanged,(ITexture* tex),(tex));
	DECLARE_FIRE_METHOD(OnTextureLoaded,(ITexture* tex),(tex));
	DECLARE_FIRE_METHOD(OnTextureUnloaded,(ITexture* tex),(tex));

	friend class TextureAsyncLoader;

public:
	ITexture(ETextureType type,bool internal);
	virtual~ITexture();

	ETextureType getType();

	//Async Load Stream
	void AddLoadStream(OS::IStream* stream);
	void ClearLoadStreams();

	bool IsInternal()const{return m_internal;}
	void SetInternal(bool i){m_internal=i;}

	//must be called before loading the texture
	void SetNumberOfMipmaps(int cnt){m_maxMipmaps=cnt;}
	int GetNumberOfMipmaps(){return m_maxMipmaps;}

	virtual void createTexture(const math::vector3di&sz,EPixelFormat pf)=0;

	virtual IHardwarePixelBuffer* getSurface(int face=0)=0;
	virtual const math::vector3di& getSize();

	virtual int getSurfaceCount()=0;

	virtual void onUse()=0;

	int getColorDepth();

	virtual void setRenderTarget(bool rt)=0;
	virtual bool isRenderTarget()=0;

	virtual void setMipmapsFilter(bool e){
		m_MipmapFilter=e;
	}
	bool getMipmapsFilter()const{return m_MipmapFilter;}

	virtual void setBilinearFilter(bool e){m_BilinearFilter=e;}
	bool getBilinearFilter()const{return m_BilinearFilter;}

	virtual void setTrilinearFilter(bool e){m_TrilinearFilter=e;}
	bool getTrilinearFilter()const{return m_TrilinearFilter;}

	virtual void setAnisotropicFilter(bool e){m_AnisotropicFilter=e;}
	bool getAnisotropicFilter()const{return m_AnisotropicFilter;}


	virtual const core::string& getDeviceType()const=0;

	virtual bool loadSurfaces(const ImageInfo** surfaces,int count);

	//return the number of faces that's this texture type support
	int getTexNumberOfFaces();
	
	EPixel_TYPE getDataType()const ;
	EPixelFormat getImageFormat()const;

	void setPixelFormat(PixelFormat*format){m_pixelFormat=format;}
	PixelFormat*getPixelFormat(){return m_pixelFormat;}

	virtual ITexture* getDuplicate() =0;


};

MakeSharedPtrType(ITexture)

}
}


#endif