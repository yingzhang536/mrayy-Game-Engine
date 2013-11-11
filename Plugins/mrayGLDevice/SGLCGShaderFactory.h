


#ifndef ___SGLCGShaderFactory___
#define ___SGLCGShaderFactory___


#include <IGPUShaderFactory.h>
#include <cg\cg.h>
#include "compileConfig.h"

namespace mray{
namespace video{
	class mrayDev;
	class GPUShader;
	class IShaderConstantsCallback;
	class SGLMaterialRenderer;

class MRAY_GL_DLL SGLCGShaderFactory:public IGPUShaderFactory
{
	CGcontext m_context;
public:
	SGLCGShaderFactory();
	virtual ~SGLCGShaderFactory();

	virtual const core::string& getType();

	virtual bool canCreateType(const core::string&type);

	virtual IGPUShaderProgram* createShader(video::IVideoDevice*device,EShaderProgramType type,bool fromFile,
		const core::string&program,const char*entryPoint);
	virtual IGPUShaderProgram* createShader(video::IVideoDevice*device,EShaderProgramType type,OS::IStream* programStream,const char*entryPoint);
};

}
}


#endif




