

#ifndef ___SGLCGShaderProgram___
#define ___SGLCGShaderProgram___


#include <IGPUShaderProgram.h>
#include <cg\cg.h>
#include <cg\cgGL.h>
#include <map>

#include "compileConfig.h"

namespace mray{
namespace video{
	class IShaderConstantsCallback;
	class GLDev;

class MRAY_GL_DLL SGLCGShaderProgram:public IGPUShaderProgram
{

protected:

//#define MAX_MATRICIES 100

	class CGUniform:public GPUUniform
	{
	public:
		CGparameter param;
	};
	//UniformList UniformsArray;
	//typedef std::vector<CGUniform> UniformsArray;

	CGcontext m_context;
	static CGprofile	s_CgVertexProfile;
	static CGprofile	s_CgFragmentProfile;
	static CGprofile	s_CgGeometryProfile;
	static int	s_CgCounter;

	IVideoDevice* m_device;

	core::string m_Program;
	core::string m_entryPoint;
	bool m_loaded;

	CGprofile m_CgProfile;

	UniformList m_Uniforms;

	CGprogram	m_CgProgram;

	std::vector<GCPtr<video::ITexture>>textures;

	//for matricies..
	static std::vector<float> m_matrixArray;//[16*MAX_MATRICIES];

	EUniformType cgToUniform(CGtype type,size_t&outSize);
	void fillUniformArray(CGparameter param,UniformList*arr,size_t contextArraySize=1);

	bool checkForError(const mchar*msg);
	bool hasError();

	virtual uint calcSizeInternal();

	void loadUniforms();
	void setUniformVal(CGparameter param,EUniformType type,const float*vals,int count);

	static void _CG_ErrorHandler(CGcontext, CGerror, void *);
public:

	SGLCGShaderProgram(IVideoDevice*device,bool fromFile,const core::string&program,
		CGcontext context,EShaderProgramType type,const char* entryPoint);

	virtual~SGLCGShaderProgram();

	virtual const core::string& getShaderType();

	virtual const core::string& GetEntryPoint(){return m_entryPoint;}

	virtual void use();
	virtual void unuse();

	virtual void setTexture(const core::string&name,TextureUnit* tex);
	virtual void setMatrixArray(GPUUniform*u,const math::matrix4x4* arr,int count);
	virtual bool setConstant(GPUUniform*u,const float*vals,int count);
	virtual bool setConstant(const core::string&name,const float*vals,int count);

	virtual void createArray(const char*name,int length);
	virtual void setArrayElementValue(const char*name,int index,const float*vals,int count);

	// true if the Vertex and Pixel Uniforms are in the same list (ex. GLSL)
	virtual bool UniformListShared();

	virtual void setUniformAlias(const core::string&name,const core::string&alias);

	virtual int getUniformCount();
	virtual GPUUniform* getUniform(const core::string&name);
	virtual UniformListIT beginUniforms();
	virtual UniformListIT endUniforms();

	virtual int getAttributeIndex(EMeshStreamType attribute,int index);
	virtual bool isAttributeValid(EMeshStreamType attribute,int index);
};

}
}




#endif




