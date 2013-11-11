#include "stdafx.h"


#include <GPUShader.h>
#include <IShaderConstantsCallback.h>
#include "SGLCGShaderFactory.h"
#include "GLDev.h"
#include "SGLCGShaderProgram.h"
#include "IStream.h"

namespace mray{
namespace video{
	const core::string s_shaderType=mT("cg");

SGLCGShaderFactory::SGLCGShaderFactory(){
	m_context=cgCreateContext();
}
SGLCGShaderFactory::~SGLCGShaderFactory(){
//	cgDestroyContext(m_context);
}

const core::string& SGLCGShaderFactory::getType(){
	return s_shaderType;
}

bool SGLCGShaderFactory::canCreateType(const core::string&type){
	return s_shaderType.equals_ignore_case(type);
}

IGPUShaderProgram* SGLCGShaderFactory::createShader(video::IVideoDevice*device,EShaderProgramType type,bool fromFile,
												 const core::string&program,const char*entryPoint)
{
	if(device->getDeviceType()==GLDev::m_deviceType)
	{
		IGPUShaderProgram*prog= new SGLCGShaderProgram(device,fromFile,
			program,m_context,type,entryPoint);
		
		return prog;
	}
	//add for D3DX here

	return 0;
}

IGPUShaderProgram* SGLCGShaderFactory::createShader(video::IVideoDevice*device,EShaderProgramType type,OS::IStream* programStream,const char*entryPoint)
{
	if(device->getDeviceType()==GLDev::m_deviceType)
	{
		int sz=programStream->length();
		programStream->seek(0,OS::ESeek_Set);
		char*data=new char[sz+1];
		int r=programStream->read(data,sz);
		core::string appData;
		core::char_to_string(data,appData);


		IGPUShaderProgram*prog= createShader(device,type,false,appData,entryPoint);

		delete [] data;
		
		return prog;
	}

	return 0;
}

}
}




