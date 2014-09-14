

#include "stdafx.h"

#include "AImpSceneLoader.h"

#include "StringUtil.h"
#include "IFileSystem.h"
#include "MeshLoader.h"
#include "RenderMaterial.h"
#include "RenderPass.h"
#include "RenderTechnique.h"


#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/PostProcess.h"
#include "assimp/cimport.h"
#include "assimp/Scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "TextureResourceManager.h"
#include "IMeshBuffer.h"
#include "MeshBufferData.h"

namespace mray
{
namespace loaders
{


	//////////////////////////////////////////////////////////////////////////
	class AImpSharedObject :public ISingleton<AImpSharedObject>
	{
	protected:
		int _refCount;

		void createAILogger()
		{
			//Assimp::Logger::LogSeverity severity = Assimp::Logger::NORMAL;
			Assimp::Logger::LogSeverity severity = Assimp::Logger::VERBOSE;

			// Create a logger instance for Console Output
			Assimp::DefaultLogger::create("", severity, aiDefaultLogStream_STDOUT);

			// Create a logger instance for File Output (found in project folder or near .exe)
			Assimp::DefaultLogger::create("assimp_log.txt", severity, aiDefaultLogStream_FILE);

			// Now I am ready for logging my stuff
			Assimp::DefaultLogger::get()->info("this is my info-call");
		}
		void InitImporters(MeshLoader* l)
		{
			core::string ext;
			importer.GetExtensionList(ext);

			std::vector<core::string> extList = core::StringUtil::Split(ext, ";*.");
			for (int i = 0; i < extList.size(); ++i)
			{
				if (l->GetLoader(extList[i]))
					continue;
				AImpSceneLoader* imp = new AImpSceneLoader(extList[i]);
				l->addLoader(imp);
			}
		}
	public:

		Assimp::Importer importer;

		AImpSharedObject(MeshLoader* l)
		{
			_refCount = 1;
			createAILogger();
			InitImporters(l);
		}
		~AImpSharedObject()
		{
		}
		void ref()
		{
			++_refCount;
		}
		void release()
		{
			_refCount--;
			if (_refCount == 0)
			{
				Assimp::DefaultLogger::kill();
				delete this;
			}
		}
	};

	class AImpLoaderState
	{
	public:

		std::vector<video::RenderMaterialPtr> materials;

		video::RenderMaterial* loadMaterial(const aiMaterial* m)
		{
			aiColor4D clr;
			video::RenderMaterial* mtrl = new video::RenderMaterial();
			video::RenderPass* pass= mtrl->CreateTechnique("Default")->CreatePass("Default");
			aiString str;

			if (aiGetMaterialString(m, AI_MATKEY_NAME, &str) == AI_SUCCESS)
				mtrl->SetName(str.C_Str());

			if (aiGetMaterialColor(m, AI_MATKEY_COLOR_DIFFUSE, &clr) == AI_SUCCESS)
				pass->SetDiffuse(video::SColor(clr.r, clr.g, clr.b, clr.a));

			if (aiGetMaterialColor(m, AI_MATKEY_COLOR_AMBIENT, &clr) == AI_SUCCESS)
			{
				clr = 1;;
				pass->SetAmbient(video::SColor(clr.r, clr.g, clr.b, clr.a));
			}
			if (aiGetMaterialColor(m, AI_MATKEY_COLOR_SPECULAR, &clr) == AI_SUCCESS)
				pass->SetSpecular(video::SColor(clr.r, clr.g, clr.b, clr.a));

			if (aiGetMaterialColor(m, AI_MATKEY_COLOR_EMISSIVE, &clr) == AI_SUCCESS)
				pass->SetEmissive(video::SColor(clr.r, clr.g, clr.b, clr.a));

			if (aiGetMaterialColor(m, AI_MATKEY_COLOR_TRANSPARENT, &clr) == AI_SUCCESS)
			{
				if (clr.a < 1)
					pass->setMaterialRenderer(video::MRT_TRANSPARENT);
				pass->SetAlpha(clr.a);
			}
			float  shininess, strength=1;
			unsigned int max = 1;
			if (aiGetMaterialFloatArray(m, AI_MATKEY_SHININESS, &shininess, &max) == AI_SUCCESS)
			{
				max = 1;
				if(aiGetMaterialFloatArray(m, AI_MATKEY_SHININESS_STRENGTH, &strength, &max)==AI_SUCCESS)
					pass->SetShininess(shininess*strength);
				else 
					pass->SetShininess(shininess);
			}
			int state;
			if (AI_SUCCESS == aiGetMaterialIntegerArray(m, AI_MATKEY_ENABLE_WIREFRAME, &state, &max))
			{
				pass->setRenderState(video::RS_Wireframe, state ? video::ES_Use : video::ES_DontUse);
			}

			if (AI_SUCCESS == aiGetMaterialIntegerArray(m, AI_MATKEY_TWOSIDED, &state, &max))
			{
				pass->setRenderState(video::RS_CullFace, state ? video::ES_Use : video::ES_DontUse);
			}
			pass->setRenderState(video::RS_CullFace,  video::ES_DontUse);
		//	pass->setRenderState(video::RS_Lighting,  video::ES_DontUse);

			aiString texPath;
			aiReturn texFound = AI_SUCCESS;


			int i = 0;
			if (m->GetTexture(aiTextureType_DIFFUSE, i, &texPath) == AI_SUCCESS)
			{
// 			for (int i = 0; i < m->GetTextureCount(aiTextureType_DIFFUSE);++i)
// 			{
//				texFound = m->GetTexture(aiTextureType_DIFFUSE, i, &texPath);
				video::ITexture* t= gTextureResourceManager.loadTexture2D(texPath.C_Str());
			//	t->load(false);
				pass->setTexture(t, i);
			}
			return mtrl;
		}

		void loadTexture(aiTexture* tex)
		{
		}

		void loadMesh(const aiMesh* m, scene::MeshBufferData* d,  math::matrix4x4 &trans)
		{

			bool ident = false;;
			if (trans.checkIdentity())
				ident = true;
			scene::IMeshBuffer* buff= d->getMeshBuffer();
			buff->setMeshName(m->mName.C_Str());
			
			if (m->HasPositions())
			{
				video::IHardwareStreamBuffer* posStream = buff->createStream(0, video::EMST_Position, video::ESDT_Point3f, m->mNumVertices, video::IHardwareBuffer::EUT_StaticWriteOnly, false, true);
				math::vector3d* pPtr = (math::vector3d*)posStream->lock(0, m->mNumVertices, video::IHardwareBuffer::ELO_Discard);

				aiVector3D* vert = m->mVertices;
				math::box3d bb;

				for (int v = 0; v < m->mNumVertices; ++v)
				{
					(*pPtr).set((*vert).x, (*vert).z, -(*vert).y);
					if (!ident)
						(*pPtr) = trans*(*pPtr);
					if (v==0)
						bb.reset(math::vector3d((*pPtr).x, (*pPtr).y, (*pPtr).z));
					else
						bb.AddPoint(*pPtr);
					++vert;
					++pPtr;
				}
				buff->setBoundingBox(bb);

				posStream->unlock();
			}
			if (m->mColors[0])
			{
				video::IHardwareStreamBuffer* clrStream = buff->createStream(0, video::EMST_Color, video::ESDT_Point4f, m->mNumVertices, video::IHardwareBuffer::EUT_StaticWriteOnly, false,true);
				video::SColor* pPtr = (video::SColor*)clrStream->lock(0, m->mNumVertices, video::IHardwareBuffer::ELO_Discard);

				aiColor4D* vert = m->mColors[0];

				for (int v = 0; v < m->mNumVertices; ++v)
				{
					(*pPtr++).Set((*vert).r, (*vert).g, (*vert).b, (*vert).a);
					++vert;
				}
				clrStream->unlock();
			}
			if (m->HasNormals())
			{
				video::IHardwareStreamBuffer* normalStream = buff->createStream(0, video::EMST_Normal, video::ESDT_Point3f, m->mNumVertices, video::IHardwareBuffer::EUT_StaticWriteOnly, false, true);
				math::vector3d* pPtr = (math::vector3d*)normalStream->lock(0, m->mNumVertices, video::IHardwareBuffer::ELO_Discard);

				aiVector3D* vert = m->mNormals;

				for (int v = 0; v < m->mNumVertices; ++v)
				{
					(*pPtr++).set((*vert).x, (*vert).z, -(*vert).y);
					++vert;
				}

				normalStream->unlock();
			}
			if (m->HasTangentsAndBitangents())
			{
				video::IHardwareStreamBuffer* tanStream = buff->createStream(0, video::EMST_Tangent, video::ESDT_Point3f, m->mNumVertices, video::IHardwareBuffer::EUT_StaticWriteOnly, false, true);
				video::IHardwareStreamBuffer* binStream = buff->createStream(0, video::EMST_Binormal, video::ESDT_Point3f, m->mNumVertices, video::IHardwareBuffer::EUT_StaticWriteOnly, false, true);
				math::vector3d* tan = (math::vector3d*)tanStream->lock(0, m->mNumVertices, video::IHardwareBuffer::ELO_Discard);
				math::vector3d* bin = (math::vector3d*)binStream->lock(0, m->mNumVertices, video::IHardwareBuffer::ELO_Discard);


				for (int v = 0; v < m->mNumVertices; ++v)
				{
					(*tan++).set((m->mTangents[v]).x, m->mTangents[v].y, m->mTangents[v].z);
					(*bin++).set((m->mBitangents[v]).x, m->mBitangents[v].y, m->mBitangents[v].z);
				}

				tanStream->unlock();
				binStream->unlock();
			}

			for (int i = 0; i < m->GetNumUVChannels();++i)
			{
				video::EStreamDataType dt=video::ESDT_Point2f;
				if (m->mNumUVComponents[i]==3)
					dt = video::ESDT_Point3f;
				video::IHardwareStreamBuffer* uvStream = buff->createStream(i, video::EMST_Texcoord, dt, m->mNumVertices, video::IHardwareBuffer::EUT_StaticWriteOnly, false, true);
				void* ptr = uvStream->lock(0, m->mNumVertices, video::IHardwareBuffer::ELO_Discard);

				aiVector3D* src = m->mTextureCoords[i];

				if (dt == video::ESDT_Point2f)
				{
					math::vector2d* uv = (math::vector2d*)ptr;

					for (int v = 0; v < m->mNumVertices; ++v)
					{
						(*uv++).set((*src).x, (*src).y);
						++src;
					}
				}

				if (dt == video::ESDT_Point3f)
				{
					math::vector3d* uv = (math::vector3d*)ptr;

					for (int v = 0; v < m->mNumVertices; ++v)
					{
						(*uv++).set((*src).x, (*src).y, (*src).z);
						++src;
					}
				}

				uvStream->unlock();
			}
			{
				uint indexCount = m->mNumFaces;
				video::EMeshRenderType rt;
				int compCount = 1;
				if (m->mPrimitiveTypes & aiPrimitiveType_POINT)
				{
					compCount = 1;
					rt = video::MR_POINTS;
				}else if (m->mPrimitiveTypes & aiPrimitiveType_LINE)
				{
					compCount = 2;
					rt = video::MR_LINES;
				}
				else if (m->mPrimitiveTypes & aiPrimitiveType_TRIANGLE)
				{
					compCount = 3;
					rt = video::MR_TRIANGLES;
				}

				buff->setRenderType(rt);

				indexCount *= compCount;

				video::IHardwareIndexBuffer* index = buff->createIndexBuffer(video::IHardwareIndexBuffer::EIT_32Bit, indexCount, video::IHardwareBuffer::EUT_Static, false);
				uint* idx = (uint*)index->lock(0, indexCount, video::IHardwareBuffer::ELO_Discard);
				uint*ptr = idx;
				for (int j = 0; j < m->mNumFaces; ++j)
				{
					*ptr++ = m->mFaces[j].mIndices[0];
					*ptr++ = m->mFaces[j].mIndices[1];
					*ptr++ = m->mFaces[j].mIndices[2];
				}
				index->unlock();
			}
			{
				if (m->mMaterialIndex>=0)
				{
					d->setMaterial(materials[m->mMaterialIndex]);
				}
			}
		}

		void LoadNodes(scene::SMesh* ret, const aiScene* scene, aiNode* nd, const math::matrix4x4& parent)
		{
			aiMatrix4x4 m = nd->mTransformation;
//			aiTransposeMatrix4(&m);
			math::matrix4x4 trans
				(
				m.a1, m.a2, m.a3, m.a4,
				m.b1, m.b2, m.b3, m.b4,
				m.c1, m.c2, m.c3, m.c4,
				m.d1, m.d2, m.d3, m.d4
				);

			trans = parent*trans;

			for (int i = 0; i < nd->mNumMeshes; ++i)
			{
				const aiMesh* m = scene->mMeshes[nd->mMeshes[i]];
				scene::MeshBufferData* b = ret->addNewBuffer();
				loadMesh(m, b,trans);
				b->getMeshBuffer()->setMeshName(nd->mName.C_Str());
			}

			for (int i = 0; i < nd->mNumChildren; ++i)
			{
				LoadNodes(ret, scene, nd->mChildren[i],trans);
			}
		}

		scene::SMesh* load(OS::IStream* stream)
		{
			if (!stream || !stream->length())
				return 0;
//			uchar* data = new uchar[stream->length()];
// 			stream->read(data, stream->length());


			const aiScene*	scene;
			scene = AImpSharedObject::getInstance().importer.ReadFile(stream->getStreamName(), aiProcess_SortByPType | aiProcess_Triangulate | aiProcess_OptimizeMeshes); //
// 			scene = AImpSharedObject::getInstance().importer.ReadFileFromMemory(data, stream->length(),  aiProcess_SortByPType | aiProcess_Triangulate); //aiProcessPreset_TargetRealtime_Quality |
// 			delete[] data;
			scene::SMesh* ret = 0;

			if (scene && scene->HasMeshes())
			{
				ret = new scene::SMesh();
				for (int i = 0; i < scene->mNumTextures; ++i)
				{
					loadTexture(scene->mTextures[i]);
				}
				for (int i = 0; i < scene->mNumMaterials; ++i)
				{
					video::RenderMaterial* m = loadMaterial(scene->mMaterials[i]);
					materials.push_back(m);
				}
				LoadNodes(ret,scene, scene->mRootNode,math::matrix4x4::Identity);
				ret->calcCurrBoundingBox();
			}

			AImpSharedObject::getInstance().importer.FreeScene();
			return ret;
		}
	};

AImpSceneLoader::AImpSceneLoader(const core::string& ext)
{
	m_ext = ext;
	AImpSharedObject::getInstance().ref();
}

AImpSceneLoader::~AImpSceneLoader()
{
	AImpSharedObject::getInstance().release();
}


scene::SMesh* AImpSceneLoader::load(const  core::string& name)
{
	if (!canLoad(name))return 0;
	OS::IStreamPtr file = gFileSystem.openFile(name, OS::BIN_READ);
	if (!file)return 0;
	return load(file);
}

scene::SMesh* AImpSceneLoader::load(OS::IStream* stream)
{
	AImpLoaderState s;
	return s.load(stream);
}

bool AImpSceneLoader::canLoad(const  core::string& name)
{
	core::string path, ext;
	core::StringUtil::SplitPathExt(name, path, ext);
	return ext.equals_ignore_case(m_ext);
}

const core::string&AImpSceneLoader::getExtension()
{
	return m_ext;
}

void AImpSceneLoader::InitImporters(MeshLoader* l)
{
	if (!AImpSharedObject::isExist())
		new AImpSharedObject(l);
}

void AImpSceneLoader::DestroyImporter()
{
	if (AImpSharedObject::isExist())
	{
		AImpSharedObject::getInstance().release();
	}

}
}
}


/*


// ------------------------------------------------------------------------------------------------
aiScene::aiScene()
: mFlags()
, mRootNode()
, mNumMeshes()
, mMeshes()
, mNumMaterials()
, mMaterials()
, mNumAnimations()
, mAnimations()
, mNumTextures()
, mTextures()
, mNumLights()
, mLights()
, mNumCameras()
, mCameras()
, mPrivate(new Assimp::ScenePrivateData())
{
}

// ------------------------------------------------------------------------------------------------
aiScene::~aiScene()
{
	// delete all sub-objects recursively
	delete mRootNode;

	// To make sure we won't crash if the data is invalid it's
	// much better to check whether both mNumXXX and mXXX are
	// valid instead of relying on just one of them.
	if (mNumMeshes && mMeshes)
	for (unsigned int a = 0; a < mNumMeshes; a++)
		delete mMeshes[a];
	delete[] mMeshes;

	if (mNumMaterials && mMaterials)
	for (unsigned int a = 0; a < mNumMaterials; a++)
		delete mMaterials[a];
	delete[] mMaterials;

	if (mNumAnimations && mAnimations)
	for (unsigned int a = 0; a < mNumAnimations; a++)
		delete mAnimations[a];
	delete[] mAnimations;

	if (mNumTextures && mTextures)
	for (unsigned int a = 0; a < mNumTextures; a++)
		delete mTextures[a];
	delete[] mTextures;

	if (mNumLights && mLights)
	for (unsigned int a = 0; a < mNumLights; a++)
		delete mLights[a];
	delete[] mLights;

	if (mNumCameras && mCameras)
	for (unsigned int a = 0; a < mNumCameras; a++)
		delete mCameras[a];
	delete[] mCameras;

	delete static_cast<Assimp::ScenePrivateData*>(mPrivate);
}*/

