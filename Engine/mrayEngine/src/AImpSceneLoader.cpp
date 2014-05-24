

#include "stdafx.h"
#if 0

#include "AImpSceneLoader.h"

#include "StringUtil.h"
#include "IFileSystem.h"
#include "MeshLoader.h"
#include "RenderMaterial.h"
#include "RenderPass.h"
#include "RenderTechnique.h"


#include "assimp/Importer.hpp"	//OO version Header!
#include "assimp/PostProcess.h"
#include "assimp/Scene.h"
#include "assimp/DefaultLogger.hpp"
#include "assimp/LogStream.hpp"
#include "assimp/ScenePrivate.h"
#include "TextureResourceManager.h"

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
			_refCount = 0;
			createAILogger();
			InitImporters(l);
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

		void loadMaterial(const aiMaterial* m)
		{
			aiColor4D clr;
			video::RenderMaterial* mtrl = new video::RenderMaterial();
			video::RenderPass* pass= mtrl->CreateTechnique("Default")->CreatePass("Default");

			aiGetMaterialColor(m, AI_MATKEY_COLOR_DIFFUSE, &clr);
			pass->SetDiffuse(video::SColor(clr.r, clr.g, clr.b, clr.a));

			aiGetMaterialColor(m, AI_MATKEY_COLOR_AMBIENT, &clr);
			pass->SetAmbient(video::SColor(clr.r, clr.g, clr.b, clr.a));

			aiGetMaterialColor(m, AI_MATKEY_COLOR_SPECULAR, &clr);
			pass->SetSpecular(video::SColor(clr.r, clr.g, clr.b, clr.a));

			aiGetMaterialColor(m, AI_MATKEY_COLOR_EMISSIVE, &clr);
			pass->SetEmissive(video::SColor(clr.r, clr.g, clr.b, clr.a));

			aiGetMaterialColor(m, AI_MATKEY_COLOR_TRANSPARENT, &clr);
			pass->SetAlpha(clr.r);

			float  shininess, strength=1;
			unsigned int max = 1;
			aiGetMaterialFloatArray(m, AI_MATKEY_SHININESS, &shininess, &max);
			max = 1;
			aiGetMaterialFloatArray(m, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
			pass->SetShininess(shininess*strength);

			int state;
			if (AI_SUCCESS == aiGetMaterialIntegerArray(m, AI_MATKEY_ENABLE_WIREFRAME, &state, &max))
			{
				pass->setRenderState(video::RS_Wireframe, state ? video::ES_Use : video::ES_DontUse);
			}

			if (AI_SUCCESS == aiGetMaterialIntegerArray(m, AI_MATKEY_TWOSIDED, &state, &max))
			{
				pass->setRenderState(video::RS_CullFace, state ? video::ES_Use : video::ES_DontUse);
			}

			aiString texPath;
			aiReturn texFound = AI_SUCCESS;

			for (int i = 0; i < m->GetTextureCount(aiTextureType_DIFFUSE);++i)
			{
				texFound = m->GetTexture(aiTextureType_DIFFUSE, i, &texPath);
				video::ITexture* t= gTextureResourceManager.loadTexture2D(texPath.C_Str());
				pass->setTexture(t, i);
			}
		}

		void loadTexture(aiTexture* tex)
		{
		}

		void loadMesh(const aiMesh* m, scene::MeshBufferData* d)
		{

		}

		scene::SMesh* load(OS::IStream* stream)
		{
			if (!stream || !stream->length())
				return 0;
			uchar* data = new uchar[stream->length()];
			const aiScene*	scene;
			stream->read(data, stream->length());
			scene = AImpSharedObject::getInstance().importer.ReadFileFromMemory(data, stream->length(), aiProcessPreset_TargetRealtime_Quality);

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
					loadMaterial(scene->mMaterials[i]);
				}
				for (int i = 0; i < scene->mNumMeshes; ++i)
				{
					const aiMesh* m= scene->mMeshes[i];
					loadMesh(m, ret->addNewBuffer());
				}
			}

			delete scene;
			delete[] data;
			return ret;
		}
	};

AImpSceneLoader::AImpSceneLoader(const core::string& ext)
{
	m_ext = ext;
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

}
}





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
}

#endif