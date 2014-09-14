

#ifndef ___3DSLoader_h__
#define ___3DSLoader_h__


#include "SMesh.h"
#include "IMeshLoader.h"
#include "IMeshBuffer.h"

#include "RenderMaterial.h"

namespace mray
{
namespace loaders
{
	
class MRAY_DLL _3DSLoader
{
protected:
public:
	_3DSLoader();
	virtual ~_3DSLoader();

	virtual scene::SMesh* load(const  core::string& name);
	virtual scene::SMesh* load(OS::IStream* stream);
	virtual bool canLoad(const  core::string& name);
	virtual const core::string&getExtension();
	
};

}
}

#endif // 3DSLoader_h__
