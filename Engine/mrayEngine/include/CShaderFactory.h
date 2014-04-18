


#ifndef ___CShaderFactory___
#define ___CShaderFactory___



#include "IGPUShaderFactory.h"



namespace mray{
namespace video{

class CShaderFactory
{
	std::map<core::string,IGPUShaderFactory*> m_factories;
public:
	CShaderFactory();
	virtual ~CShaderFactory();

	void clear();

	void addFactory(IGPUShaderFactory *factory);

	IGPUShaderFactory *getFactory(const core::string&type);


	IGPUShaderProgram* createShader(const core::string&type,video::IVideoDevice*device,EShaderProgramType programType,bool fromFile,
		const core::string&program,const char*entryPoint);

	IGPUShaderProgram* createShader(const core::string&type,video::IVideoDevice*device,EShaderProgramType programType,OS::IStream* programStream,const char*entryPoint);
};

}
}



#endif



