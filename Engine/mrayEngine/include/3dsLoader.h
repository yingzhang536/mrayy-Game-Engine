

#ifndef ___3DSLOADER___
#define ___3DSLOADER___

#include "SMesh.h"
#include "IMeshLoader.h"
#include "IMeshBuffer.h"

#include "RenderMaterial.h"

namespace mray{
namespace loaders{

class MRAY_DLL _3dsLoader:public IMeshLoader
{
	struct tChunk
	{
		unsigned short int ID;					// The chunk's ID		
		unsigned int length;					// The length of the chunk
		unsigned int bytesRead;					// The amount of bytes read within that chunk
	};
	struct RenderMaterialGroup{
		core::string matName;
		ushort faceCount;
		short*faces;
	};

	struct RenderMaterialInfo{
		RenderMaterialInfo();
		~RenderMaterialInfo()
		{
		//	delete mat;
		}
		core::string Name;
		video::RenderMaterial* mat;
		core::string texName[5];
	};

	// This reads in a string and saves it in the char array passed in
	int GetString(core::string &str);

	// This reads the next chunk
	void ReadChunk(tChunk *);

	void ReadPercentageChunk(float &out, tChunk *);

	// This reads the next large chunk
	void ProcessNextChunk(scene::SMesh*pModel, tChunk *);

	// This reads the object chunks
	void ProcessNextObjectChunk(scene::SMesh*pModel, scene::IMeshBuffer *pObject, tChunk *);

	// This reads the material chunks
	void ProcessNextMaterialChunk(scene::SMesh*pModel, tChunk *);

	// This reads the RGB value for the object's color
	void ReadColorChunk(video::SColor *clr, tChunk *pChunk);

	// This reads the objects vertices
	void ReadVertices(scene::IMeshBuffer *pObject, tChunk *);

	// This reads the objects face information
	void ReadVertexIndices(scene::IMeshBuffer *pObject, tChunk *);

	// This reads the texture coodinates of the object
	void ReadUVCoordinates(scene::IMeshBuffer *pObject, tChunk *);

	// This reads in the material name assigned to the object and sets the materialID
	void ReadMaterialGroup(scene::SMesh*pModel, scene::IMeshBuffer *pObject, tChunk *pPreviousChunk);
	
	// This computes the vertex normals for the object (used for lighting)
	void ComputeNormals(scene::SMesh*pModel);

	
	void composeObject(scene::SMesh*mesh,int m);

	// This frees memory and closes the file
	void CleanUp();
	
	// The file pointer
	OS::IStream* m_FilePointer;
	tChunk *m_CurrentChunk;;
	tChunk *m_TempChunk;

	std::vector<ushort> m_tmpIndex;

	std::vector<RenderMaterialGroup> matGroup;
	std::vector<RenderMaterialInfo> materials;
public:
	_3dsLoader(){};
	virtual scene::SMesh* load(const  core::string& name);
	virtual scene::SMesh* load(OS::IStream* stream);
	virtual bool canLoad(const  core::string& name);
	virtual const core::string&getExtension();
};
};//loaders
};//mray

#endif