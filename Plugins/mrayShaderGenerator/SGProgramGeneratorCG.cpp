
#include "stdafx.h"
#include "StringConverter.h"

#include "SGProgramGeneratorCG.h"

#include "SGProgram.h"
#include "SGFunction.h"
#include "SGFunctionCall.h"
#include "SGForStatement.h"
#include "SGOperation.h"
#include "SGValue.h"
#include "SGVariable.h"
#include "SGDirectValue.h"

namespace mray
{
namespace shader
{


const core::string SGProgramGeneratorFactoryCG::Type=mT("CG");

const core::string SGProgramGeneratorCG::s_GPUTypesStr[]=
{
	mT("int"),
	mT("int2"),
	mT("int3"),
	mT("int4"),
	mT("float"),
	mT("float2"),
	mT("float3"),
	mT("float4"),
	mT("sampler1D"),
	mT("sampler2D"),
	mT("sampler3D"),
	mT("samplerCUBE"),
	mT("float2x2"),
	mT("float2x3"),
	mT("float2x4"),
	mT("float3x3"),
	mT("float3x4"),
	mT("float4x2"),
	mT("float4x3"),
	mT("float4x4")
};

const core::string SGProgramGeneratorCG::s_GPUTypes_halfStr[]=
{
	mT("short"),
	mT("short2"),
	mT("short3"),
	mT("short4"),
	mT("half"),
	mT("half2"),
	mT("half3"),
	mT("half4"),
	mT("sampler1D"),
	mT("sampler2D"),
	mT("sampler3D"),
	mT("samplerCUBE"),
	mT("half2x2"),
	mT("half2x3"),
	mT("half2x4"),
	mT("half3x3"),
	mT("half3x4"),
	mT("half4x2"),
	mT("half4x3"),
	mT("half4x4")
};
const core::string SGProgramGeneratorCG::s_GPUSemanticStr[]=
{
	mT(""),
	mT("POSITION"),
	mT("NORMAL"),
	mT("TEXCOORD"),
	mT("COLOR"),
	mT("BLENDWEIGHT"),
	mT("BLENDINDICES"),
	mT("BINORMAL"),
	mT("TANGENT")
};

SGProgramGeneratorCG::SGProgramGeneratorCG():m_withComments(true),m_useHalfDataType(true),m_tabsCount(0)
{
}
SGProgramGeneratorCG::~SGProgramGeneratorCG()
{
}

const core::string& SGProgramGeneratorCG::GetType()const
{
	return SGProgramGeneratorFactoryCG::Type;
}

const core::string& SGProgramGeneratorCG::GenerateShader(SGProgram*program)
{
	m_shader=mT("");
	m_tabsCount=0;
	program->Visit(this);
	return m_shader;
}


void SGProgramGeneratorCG::OnVisit(SGProgram*v)
{
	//write header
	m_shader=mT("/************************************************************************\n");
	m_shader+=mT("*************** Generated By MRAY's Shader Generator ********************\n");
	m_shader+=mT("*************************************************************************\n");

	if(m_withComments)
	{
		m_shader+=mT("Program Type: ");
		switch(v->GetType())
		{
		case EPT_VERTEX:m_shader+=mT("Vertex Program\n");break;
		case EPT_FRAGMENT:m_shader+=mT("Fragment Program\n");break;
		case EPT_GEOMETRY:m_shader+=mT("Geometry Program\n");break;
		}
		if(v->GetDescription()!=mT(""))
			m_shader+=mT("Description: ")+v->GetDescription()+mT("\n");

		m_shader+=mT("************************************************************************/\n");
	}

	//write dependencies
	m_shader+=mT("\n\n");
	if(m_withComments)
		m_shader+=mT("//Dependencies \n");
	const std::vector<core::string>& dep=v->GetDependencies();
	for(int i=0;i<dep.size();++i)
	{
		m_shader+=mT("#include \"")+dep[i]+mT(".cg\"\n");
	}

	//write uniforms
	m_shader+=mT("\n\n");
	const std::vector<SGVariablePtr>& uniforms=v->GetUniforms();
	for(int i=0;i<uniforms.size();++i)
	{
		WriteValue(uniforms[i]->IsConst(),uniforms[i]->GetName(),uniforms[i]->GetType(),uniforms[i]->GetIndex());
		m_shader+=mT(";");

		if(m_withComments)
		{
			if(uniforms[i]->GetDescription()!=mT(""))
			{
				m_shader+=mT("//")+uniforms[i]->GetDescription();
			}
			m_shader+=mT("\n");
		}
	}
}

void SGProgramGeneratorCG::OnVisit(SGFunction*v)
{
	const std::vector<SGVariablePtr>& inputs=v->GetInputVars();
	const std::vector<SGVariablePtr>& outputs=v->GetOutputVars();
	const std::vector<SGVariablePtr>& values=v->GetVariables();
	const std::vector<SGIStatementPtr>& statements=v->GetStatements();
	SGIValueCRef retType= v->GetReturnValue();

	if(m_withComments)
	{
		m_shader+=mT("// Function Name : ")+v->GetName()+mT("\n");

		for(int i=0;i<inputs.size();++i)
		{
			if(inputs[i]->GetUsage()==EVU_UNIFORM)
				m_shader+=mT("// Uniform Input[ ");
			else 
				m_shader+=mT("//         Input[ ");
			m_shader+=inputs[i]->GetName()+mT("]");
			if(inputs[i]->GetDescription()!=mT(""))
				m_shader+=mT(" : ")+inputs[i]->GetDescription();
			m_shader+=mT("\n");
		}

		for(int i=0;i<outputs.size();++i)
		{
			m_shader+=mT("//        Output[ ");
			m_shader+=outputs[i]->GetName()+mT("]");
			if(inputs[i]->GetDescription()!=mT(""))
				m_shader+=mT(" : ")+outputs[i]->GetDescription();
			m_shader+=mT("\n");
		}
		if(v->GetDescription()!=mT(""))
			m_shader+=mT("// Description   : ")+v->GetDescription();
	}
	if(retType.isNull())
		m_shader+=mT("void ");
	else
	{
		if(m_useHalfDataType)
			m_shader+=s_GPUTypes_halfStr[retType->GetType()];
		else
			m_shader+=s_GPUTypesStr[retType->GetType()];
		m_shader+=mT(" ");
	}
	m_shader+=v->GetName();
	m_shader+=mT("(");
	for(int i=0;i<inputs.size();++i)
	{
		if(i>0)
			m_shader+=mT(",");
		m_shader+=mT("\n\t\t");
		if(inputs[i]->GetUsage()==EVU_UNIFORM)
			m_shader+=mT("uniform ");
		WriteValue(false,inputs[i]->GetName(),inputs[i]->GetType(),inputs[i]->GetIndex());
		if(inputs[i]->GetUsage()==EVU_INPUT && inputs[i]->GetSemantic()!=EUS_UNKOWN)
		{
			m_shader+=mT(":")+s_GPUSemanticStr[inputs[i]->GetSemantic()];
		}
	}
	if(inputs.size()>0 && outputs.size()>0)
		m_shader+=mT(",\n\t\t");
	for(int i=0;i<outputs.size();++i)
	{
		if(i>0)
			m_shader+=mT(",\n\t\t");
		m_shader+=mT("out ");
		WriteValue(false,outputs[i]->GetName(),outputs[i]->GetType(),outputs[i]->GetIndex());
	}
	m_shader+=mT(")\n");
	m_shader+=mT("{\n");

	++m_tabsCount;
	WriteTabs();
	if(m_withComments)
	{
		m_shader+=mT("//Variables\n");
	}
	for(int i=0;i<values.size();++i)
	{
		WriteTabs();
		WriteValue(values[i]->IsConst(),values[i]->GetName(),values[i]->GetType(),values[i]->GetIndex());
		m_shader+=mT(";");
		if(values[i]->GetDescription()!=mT(""))
		{
			m_shader+=mT("//")+values[i]->GetDescription();
		}
		m_shader+=mT("\n");
	}

	m_shader+=mT("\n\n");

	WriteStatements(statements);

	if(!retType.isNull())
	{
		WriteTabs();
		m_shader+=mT("return ");
		retType->Visit(this);
		m_shader+=mT(";\n");
	}

	--m_tabsCount;
	m_shader+=mT("}//")+v->GetName();
}

void SGProgramGeneratorCG::OnVisit(SGFunctionCall*v)
{
	const std::vector<SGIValuePtr>& inputs=v->GetInputValues();
	const std::vector<SGVariablePtr>& outputs=v->GetOutputValues();
	m_shader+=v->GetFunctionName();
	m_shader+=mT("(");
	for(int i=0;i<inputs.size();++i)
	{
		if(i>0)m_shader+=mT(",");
		inputs[i]->Visit(this);
	}
	if(inputs.size()>0 && outputs.size()>0)
		m_shader+=mT(",");
	for(int i=0;i<outputs.size();++i)
	{
		if(i>0)m_shader+=mT(",");
		outputs[i]->Visit(this);
	}
	m_shader+=mT(")");
}
void SGProgramGeneratorCG::OnVisit(SGOperation*v)
{
	static const core::string operationsStr[]=
	{
		mT("+"),
		mT("-"),
		mT("*"),
		mT("/"),
		mT("")
	};
 
	const std::vector<SGIValuePtr>& inputs=v->GetInputValues();
	if(inputs.size()==0 || v->GetOperation()!=EOT_ASSIGN && inputs.size()!=2)
	{
		printf("%d\n",inputs.size());
		gVideoLoggerSystem.log(mT("SGProgramGeneratorCG::OnVisit(): Operation doesn't inputs!"),
			ELL_WARNING);
		return;
	}
	if(v->GetOperation()==EOT_ASSIGN && v->GetOutputValues().size()==0)
	{
		gVideoLoggerSystem.log(mT("SGProgramGeneratorCG::OnVisit(): Assign operation doesn't have outputs!"),
			ELL_WARNING);
		return;
	}
	if(v->GetOperation()==EOT_ASSIGN)
	{
		m_shader+=v->GetOutputValues()[0]->GetName();
		m_shader+=mT("=");
		inputs[0]->Visit(this);
	}else
	{
		inputs[0]->Visit(this);
		m_shader+=operationsStr[v->GetOperation()];
		inputs[1]->Visit(this);
	}

}

void SGProgramGeneratorCG::OnVisit(SGForStatement*v)
{
	WriteTabs();
	m_shader+=mT("for(short ")+v->GetVarName()+mT("=")+core::StringConverter::toString(v->GetStart())+mT(";")+
		v->GetVarName()+mT("<")+core::StringConverter::toString(v->GetEnd())+mT(";")+
		v->GetVarName()+mT("+=")+core::StringConverter::toString(v->GetIncrement())+mT(")\n");
	WriteTabs();
	m_shader+=mT("{");

	++m_tabsCount;

	WriteStatements(v->GetStatements());
	--m_tabsCount;

	WriteTabs();
	m_shader+=mT("}");
}

void SGProgramGeneratorCG::OnVisit(SGValue*v)
{
	const std::vector<SGIValuePtr>&  vals=v->GetValues();
	if(v->GetType()!=video::EUT_Float1 && v->GetType()!=video::EUT_Int1)
	{
		if(m_useHalfDataType)
		{
			m_shader+=s_GPUTypes_halfStr[v->GetType()];
		}else
		{
			m_shader+=s_GPUTypesStr[v->GetType()];
		}
		m_shader+=mT("(");
	}
	for(int i=0;i<vals.size();++i)
	{
		if(i>0)m_shader+=mT(",");
		vals[i]->Visit(this);
	}
	if(v->GetType()!=video::EUT_Float1 && v->GetType()!=video::EUT_Int1)
	{
		m_shader+=mT(")");
	}
}
void SGProgramGeneratorCG::OnVisit(SGVariable*v)
{
	m_shader+=v->GetName();
}
void SGProgramGeneratorCG::OnVisit(SGDirectValue*v)
{
	m_shader+=v->GetValue();
}
void SGProgramGeneratorCG::WriteTabs()
{
	for(int i=0;i<m_tabsCount;++i)
		m_shader+=mT("\t");
}
void SGProgramGeneratorCG::WriteValue(bool isConst,const core::string&name,video::EUniformType type,ushort index)
{
	if(isConst)
	{
		m_shader+=mT("const\t";)
	}
	if(m_useHalfDataType)
		m_shader+=s_GPUTypes_halfStr[type];
	else m_shader+=s_GPUTypesStr[type];

	m_shader+=mT("\t");
	m_shader+=name;
	switch(type)
	{
	case video::EUT_Sampler1D:
	case video::EUT_Sampler2D:
	case video::EUT_Sampler3D:
	case video::EUT_SamplerCube:
		m_shader+=mT(" : register(s")+core::StringConverter::toString(index)+mT(")");
		break;
	}
}
void SGProgramGeneratorCG::WriteStatements(const std::vector<SGIStatementPtr>& statements)
{
	for(int i=0;i<statements.size();++i)
	{
		WriteTabs();
		statements[i]->Visit(this);
		m_shader+=mT(";");
		if(statements[i]->GetDescription()!=mT(""))
		{
			m_shader+=mT("//")+statements[i]->GetDescription();
		}
		m_shader+=mT("\n");
	}
}
//////////////////////////////////////////////////////////////////////////

const core::string& SGProgramGeneratorFactoryCG::GetType()const
{
	return Type;
}
SGIProgramGenerator* SGProgramGeneratorFactoryCG::CreateGenerator()const
{
	return new SGProgramGeneratorCG();
}

}
}