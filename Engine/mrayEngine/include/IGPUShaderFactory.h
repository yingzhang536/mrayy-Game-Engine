


#ifndef ___IGPUShaderFactory___
#define ___IGPUShaderFactory___

#include "GCPtr.h"
#include "mTypes.h"
#include "IGPUShaderProgram.h"

namespace mray{
	namespace OS{
		class IStream;
	}
namespace video{
	class IVideoDevice;

class MRAY_DLL IGPUShaderFactory
{
public:
	IGPUShaderFactory(){}
	virtual ~IGPUShaderFactory(){}

	virtual const core::string&getType()=0;

	virtual bool canCreateType(const core::string& type)=0;

	virtual IGPUShaderProgram* createShader(video::IVideoDevice*device,EShaderProgramType type,bool fromFile,
		const core::string&program,const char*entryPoint)=0;

	virtual IGPUShaderProgram* createShader(video::IVideoDevice*device,EShaderProgramType type,OS::IStream* programStream,const char*entryPoint)=0;
};

}
}


#endif


