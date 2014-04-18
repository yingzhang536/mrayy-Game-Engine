
#ifndef ___MESH_LOADER___
#define ___MESH_LOADER___


#include "MString.h"

namespace mray
{
	namespace scene
	{
		class SMesh;
	}
	namespace OS
	{
		class IStream;
	}
namespace loaders
{
	class IMeshLoader;

class MRAY_DLL MeshLoader
{
	std::list<IMeshLoader*> Loaders;
public:
	MeshLoader(){}
	virtual ~MeshLoader();
	void addLoader(IMeshLoader* loader);
	scene::SMesh* Load(const  core::string&name);
	scene::SMesh* Load(OS::IStream* file);
};

};//loaders
};//mray

#endif




