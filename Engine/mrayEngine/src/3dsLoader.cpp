#include "stdafx.h"

#include "3dsLoader.h"


#include "IStream.h"
#include "ILogManager.h"
#include "SMeshManipulator.h"
#include "IFileSystem.h"
#include "ITexture.h"
#include "TextureResourceManager.h"
#include "MeshBufferData.h"
#include "RenderTechnique.h"
#include "RenderPass.h"


#include "StringUtil.h"

namespace mray{
namespace loaders{

	//>------ Primary Chunk, at the beginning of each file
#define _3DS_PRIMARY       0x4D4D

	//>------ Main Chunks
#define _3DS_OBJECTINFO    0x3D3D				// This gives the version of the mesh and is found right before the material and object information
#define _3DS_VERSION       0x0002				// This gives the version of the .3ds file
#define _3DS_EDITKEYFRAME  0xB000				// This is the header for all of the key frame info

	//>------ sub defines of OBJECTINFO
#define _3DS_MATERIAL	  0xAFFF				// This stored the texture info
#define _3DS_OBJECT		  0x4000				// This stores the faces, vertices, etc...

	//>------ sub defines of MATERIAL
#define _3DS_MATNAME       0xA000				// This holds the material name
#define _3DS_MATAMBIENT    0xA010				// This holds the ambient color of the object/material
#define _3DS_MATDIFFUSE    0xA020				// This holds the diffuse color of the object/material
#define _3DS_MATSPECULAR   0xA030				// This holds the specular color of the object/material
#define _3DS_MATSHININESS  0xA040				// This holds the shininess of the object/material
#define _3DS_MATTRANSPARENCY  0xA050
#define _3DS_MATTWOSIDES  0xA081
#define _3DS_MATWIRE	  0xA085
#define _3DS_MATSHADING	  0xA100

#define _3DS_MATMAP        0xA200				// This is a header for a new material
#define _3DS_MATMAPFILE    0xA300				// This holds the file name of the texture

#define _3DS_OBJECT_MESH   0x4100				// This lets us know that we are reading a new object

	//>------ sub defines of OBJECT_MESH
#define _3DS_OBJECT_VERTICES    0x4110			// The objects vertices
#define _3DS_OBJECT_FACES		0x4120			// The objects faces
#define _3DS_OBJECT_MATERIAL	0x4130			// This is found if the object has a material, either texture map or color
#define _3DS_OBJECT_UV			0x4140			// The UV texture coordinates


	//>------ color types
#define _3DS_COL_RGB			0x0010
#define _3DS_COL_TRU			0x0011
#define _3DS_COL_LIN_24			0x0012
#define _3DS_COL_LIN_F			0x0013

	//>------ percentage types
#define _3DS_PER_INT			0x0030			//integer
#define _3DS_PER_FLT			0x0031			//float

_3dsLoader::RenderMaterialInfo::RenderMaterialInfo()
{
	mat=new video::RenderMaterial();
	mat->CreateTechnique(mT(""))->CreatePass(mT(""));
}
scene::SMesh* _3dsLoader::load(const  mchar*filename)
{
	if(!canLoad(filename))return 0;
	m_FilePointer=gFileSystem.openFile(filename);
	if(!m_FilePointer->isOpen())return 0;
	return load(m_FilePointer);
}

scene::SMesh*_3dsLoader::load(OS::IStream* stream)
{
	m_FilePointer=stream;
	m_CurrentChunk=new tChunk;
	m_TempChunk=new tChunk;
	matGroup.clear();
	materials.clear();
	ReadChunk(m_CurrentChunk);


	if(m_CurrentChunk->ID!=_3DS_PRIMARY){
		gLogManager.log(mT("Unable to read Primary Chunk from 3ds file"),ELL_WARNING);
		return 0;
	}
	scene::SMesh*Mesh=new scene::SMesh();
#ifdef OLD_MESH_CODE
	scene::SCycle>cycles=new scene::SCycle();
	Mesh->setCycles(cycles);

	cycles->CycleProp.resize(1);//=new scene::SCycle::scene::SCycleProp[1];

#endif
	ProcessNextChunk(Mesh,m_CurrentChunk);
	materials.clear();
	delete m_TempChunk;
	delete m_CurrentChunk;
	m_FilePointer->close();
//	AnimMesh->animate(0);
	scene::SMeshManipulator::getInstance().recalcNormalsSmooth(Mesh);
	
	gLogManager.log(mT("3DS mesh file loaded"),stream->getStreamName(),ELL_SUCCESS);
	return Mesh;

}
bool _3dsLoader::canLoad(const  mchar*name)
{
	core::string str=core::StringUtil::ToLower(name);
	return strSearch(str.c_str(),mT(".3ds"))!=0;
}


void _3dsLoader::ReadChunk(tChunk *pChunk)
{
	pChunk->bytesRead = m_FilePointer->read(&pChunk->ID, 2);

	pChunk->bytesRead += m_FilePointer->read(&pChunk->length, 4);
}

void _3dsLoader::ReadPercentageChunk(float &out, tChunk *pChunk)
{
	ReadChunk(m_TempChunk);
	short iout;
	float fout;
	switch(m_TempChunk->ID)
	{
	case _3DS_PER_INT:
		m_FilePointer->read(&iout,2);
		out=(float)iout/100.0f;
		m_TempChunk->bytesRead+=2;
		break;
	case _3DS_PER_FLT:
		m_FilePointer->read(&fout,sizeof(float));
		out=fout;
		m_TempChunk->bytesRead+=sizeof(float);
		break;
	default:
		m_FilePointer->seek(m_CurrentChunk->length - m_CurrentChunk->bytesRead,OS::ESeek_Cur);
		m_TempChunk->bytesRead +=m_CurrentChunk->length - m_CurrentChunk->bytesRead;
	}
	pChunk->bytesRead+=m_TempChunk->bytesRead;
}


void _3dsLoader::ProcessNextChunk(scene::SMesh*pModel, tChunk *pPreviousChunk)
{
	scene::IMeshBuffer* newObject ;					
	RenderMaterialInfo newTexture ;				
	int version = 0;					
	int buffer[2000] = {0};					

	m_CurrentChunk=new tChunk;
	core::string tmpStr;

	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read next Chunk
		ReadChunk(m_CurrentChunk);

		// Check the chunk ID
		switch (m_CurrentChunk->ID)
		{
		case _3DS_VERSION:
			m_CurrentChunk->bytesRead += m_FilePointer->read(&version, m_CurrentChunk->length - m_CurrentChunk->bytesRead);

			// If the file version is over 3, give a warning that there could be a problem
			if (version > 0x03)
				gLogManager.log(mT("This 3DS file is over version 3 so it may load incorrectly"),ELL_WARNING);
			break;

		case _3DS_OBJECTINFO:
			// Read the next chunk
			ReadChunk(m_TempChunk);

			// Get the version of the mesh
			m_TempChunk->bytesRead += m_FilePointer->read(&version,  m_TempChunk->length - m_TempChunk->bytesRead);

			// Increase the bytesRead by the bytes read from the last chunk
			m_CurrentChunk->bytesRead += m_TempChunk->bytesRead;

			// Go to the next chunk, which is the object has a texture, it should be MATERIAL, then OBJECT.
			ProcessNextChunk(pModel, m_CurrentChunk);
			break;

		case _3DS_MATERIAL:	
			materials.push_back(newTexture);

			// Proceed to the material loading function
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case _3DS_OBJECT:


			// Add a new tObject node to our list of objects (like a link list)
			newObject=pModel->addNewBuffer()->getMeshBuffer();
			
			// Get the name of the object and store it, then add the read bytes to our byte counter.
			m_CurrentChunk->bytesRead += GetString(tmpStr);
			newObject->setMeshName(tmpStr);
			
			// Now proceed to read in the rest of the object information
			ProcessNextObjectChunk(pModel, newObject, m_CurrentChunk);

		//	composeObject(mesh,mesh->Mesh.size() - 1);
			break;

		case _3DS_EDITKEYFRAME:
			 m_FilePointer->seek(m_CurrentChunk->length - m_CurrentChunk->bytesRead,OS::ESeek_Cur);
			 m_CurrentChunk->bytesRead +=m_CurrentChunk->length - m_CurrentChunk->bytesRead;
			break;

		default: 
			 m_FilePointer->seek(m_CurrentChunk->length - m_CurrentChunk->bytesRead,OS::ESeek_Cur);
			 m_CurrentChunk->bytesRead +=m_CurrentChunk->length - m_CurrentChunk->bytesRead;
			 break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}






void _3dsLoader::ProcessNextObjectChunk(scene::SMesh*pModel, scene::IMeshBuffer *pObject, tChunk *pPreviousChunk)
{
//	int buffer[10000] = {0};					// This is used to read past unwanted data

	// Allocate a new chunk to work with
	m_CurrentChunk = new tChunk;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read
		switch (m_CurrentChunk->ID)
		{
		case _3DS_OBJECT_MESH:					// This lets us know that we are reading a new object
		
			// We found a new object, so let's read in it's info using recursion
			ProcessNextObjectChunk(pModel, pObject, m_CurrentChunk);
			break;

		case _3DS_OBJECT_VERTICES:				// This is the objects vertices
			ReadVertices(pObject, m_CurrentChunk);
			break;

		case _3DS_OBJECT_FACES:					// This is the objects face information
			ReadVertexIndices(pObject, m_CurrentChunk);
			break;

		case _3DS_OBJECT_MATERIAL:				
			// We now will read the name of the material assigned to this object
			ReadMaterialGroup(pModel, pObject, m_CurrentChunk);			
			break;

		case _3DS_OBJECT_UV:						
			// This chunk holds all of the UV coordinates for our object.  Let's read them in.
			ReadUVCoordinates(pObject, m_CurrentChunk);
			break;

		default:  

			// Read past the ignored or unknown chunks
			 m_FilePointer->seek(m_CurrentChunk->length - m_CurrentChunk->bytesRead,OS::ESeek_Cur);
			 m_CurrentChunk->bytesRead +=m_CurrentChunk->length - m_CurrentChunk->bytesRead;
			break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}


void _3dsLoader::ProcessNextMaterialChunk(scene::SMesh*pModel, tChunk *pPreviousChunk)
{
	int buffer[10000] = {0};					// This is used to read past unwanted data
	char strBuffer[256];
	// Allocate a new chunk to work with
	m_CurrentChunk = new tChunk;

	video::SColor tmpC;
	float tmpF;

	// Continue to read these chunks until we read the end of this sub chunk
	while (pPreviousChunk->bytesRead < pPreviousChunk->length)
	{
		// Read the next chunk
		ReadChunk(m_CurrentChunk);

		// Check which chunk we just read in
		switch (m_CurrentChunk->ID)
		{
		case _3DS_MATNAME:							// This chunk holds the name of the material
			
			// Here we read in the material name
			m_CurrentChunk->bytesRead += m_FilePointer->read(strBuffer,  m_CurrentChunk->length - m_CurrentChunk->bytesRead);
			materials[materials.size()- 1].Name=strBuffer;
			break;
		case _3DS_MATAMBIENT:						
			// This holds the R G B color of our object
			ReadColorChunk(&tmpC, m_CurrentChunk);
			materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->SetAmbient(tmpC);
			break;

		case _3DS_MATDIFFUSE:						
			// This holds the R G B color of our object
			ReadColorChunk(&tmpC, m_CurrentChunk);
			materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->SetDiffuse(tmpC);
			break;
		case _3DS_MATSPECULAR:		
			ReadColorChunk(&tmpC, m_CurrentChunk);		
			materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->SetSpecular(tmpC);
			break;
		case _3DS_MATSHININESS:						
			ReadPercentageChunk(tmpF, m_CurrentChunk);
			materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->SetShininess(tmpF);
			break;
		case _3DS_MATTWOSIDES:
			materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setRenderState(video::RS_CullFace,video::ES_DontUse);
			break;
		case _3DS_MATWIRE:
			materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setRenderState(video::RS_Wireframe,video::ES_Use);
			break;
		case _3DS_MATSHADING:
			short per;
			m_FilePointer->read(&per,2);
			switch(per)
			{
			case 0:
				materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setRenderState(video::RS_Wireframe,video::ES_Use);
				break;
			case 1:
				materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setRenderState(video::RS_Wireframe,video::ES_DontUse);
				materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setRenderState(video::RS_Smooth,video::ES_DontUse);
				break;
			case 2:
				materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setRenderState(video::RS_Wireframe,video::ES_DontUse);
				materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setRenderState(video::RS_Smooth,video::ES_Use);
				break;				 
			};
			 m_CurrentChunk->bytesRead +=m_CurrentChunk->length - m_CurrentChunk->bytesRead;
			break;
		case _3DS_MATMAP:							
			// Proceed to read in the material information
			ProcessNextMaterialChunk(pModel, m_CurrentChunk);
			break;

		case _3DS_MATMAPFILE:{						
			// Here we read in the material's file name
			m_CurrentChunk->bytesRead += m_FilePointer->read(strBuffer, m_CurrentChunk->length - m_CurrentChunk->bytesRead);
			core::string temp;
			core::char_to_string(strBuffer,temp);
			video::ITexturePtr  tex=gTextureResourceManager.loadTexture2D(temp);
			materials[materials.size()- 1].mat->GetActiveTechnique()->GetPassAt(0)->setTexture(tex,0);
			}
			break;
		
		default:  

			// Read past the ignored or unknown chunks
			 m_FilePointer->seek(m_CurrentChunk->length - m_CurrentChunk->bytesRead,OS::ESeek_Cur);
			 m_CurrentChunk->bytesRead +=m_CurrentChunk->length - m_CurrentChunk->bytesRead;
			 break;
		}

		// Add the bytes read from the last chunk to the previous chunk passed in.
		pPreviousChunk->bytesRead += m_CurrentChunk->bytesRead;
	}

	// Free the current chunk and set it back to the previous chunk (since it started that way)
	delete m_CurrentChunk;
	m_CurrentChunk = pPreviousChunk;
}

int _3dsLoader::GetString(core::string &out)
{
	int index = 0;
	char pBuffer[1024];
	// Read 1 byte of data which is the first letter of the string
	m_FilePointer->read(pBuffer, 1);

	// Loop until we get NULL
	while (*(pBuffer + index++) != 0) {

		// Read in a character at a time until we hit NULL.
		m_FilePointer->read(pBuffer + index, 1);
	}
	out=pBuffer;

	// Return the string length, which is how many bytes we read in (including the NULL)
	return strlen(pBuffer) + 1;
}


void _3dsLoader::ReadColorChunk(video::SColor *clr, tChunk *pChunk)
{
	// Read the color chunk info
	ReadChunk(m_TempChunk);
	uchar ucolor[3];
	float fcolor[3];
	switch(m_TempChunk->ID){
		case _3DS_COL_TRU:
		case _3DS_COL_LIN_24:
			{
				m_TempChunk->bytesRead += m_FilePointer->read(ucolor,m_TempChunk->length - m_TempChunk->bytesRead);
				clr->Set(ucolor[0],ucolor[1],ucolor[2],255);
				pChunk->bytesRead += m_TempChunk->bytesRead;
			}
			break;
		case _3DS_COL_RGB:
		case _3DS_COL_LIN_F:
			{
				m_TempChunk->bytesRead += m_FilePointer->read(fcolor,m_TempChunk->length - m_TempChunk->bytesRead);
				clr->Set((int)(fcolor[0]*255.0f),(int)(fcolor[1]*255.0f),(int)(fcolor[2]*255.0f),255);
		 		pChunk->bytesRead += m_TempChunk->bytesRead;
			}
			break;
		default:
			 m_TempChunk->bytesRead +=  m_FilePointer->seek(m_TempChunk->length - m_TempChunk->bytesRead,OS::ESeek_Cur);
		 	 pChunk->bytesRead		+= m_TempChunk->bytesRead;
	}
	// Read in the R G B color (3 bytes - 0 through 255)
//	m_TempChunk->bytesRead += m_FilePointer->read(color, m_TempChunk->length - m_TempChunk->bytesRead);
//	clr->Set(color[0],color[1],color[2],255);
//	pChunk->bytesRead += m_TempChunk->bytesRead;
}


void _3dsLoader::ReadVertexIndices(scene::IMeshBuffer *pObject, tChunk *pPreviousChunk)
{
	unsigned short index = 0;

	int nov=0;

	// Read in the number of faces that are in this object (int)
	pPreviousChunk->bytesRead += m_FilePointer->read(&nov, 2);
	//pObject->NumberOfTri=nov;
	// Alloc enough memory for the faces and initialize the structure


	m_tmpIndex.resize(3*nov);
	ushort*idx=&m_tmpIndex[0];
	// Go through all of the faces in this object
	for(int i = 0; i < nov; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			// Read the first vertice index for the current face 
			pPreviousChunk->bytesRead += m_FilePointer->read(&index, sizeof(index));

			if(j < 3)
			{
				// Store the index in our face structure.
				idx[3*i+j]= index;
			}
		}
		math::Swap<ushort>(idx[3*i+1],idx[3*i+2]);
	}
	video::IHardwareIndexBuffer* idata= pObject->createIndexBuffer(video::IHardwareIndexBuffer::EIT_16Bit,3*nov,video::IHardwareIndexBuffer::EUT_Static,false);
	idata->writeData(0,idata->getSizeInBytes(),&m_tmpIndex[0]);
}

void _3dsLoader::ReadUVCoordinates(scene::IMeshBuffer *pObject, tChunk *pPreviousChunk)
{
	int nov=0;
	// Read in the number of UV coordinates there are (int)
	pPreviousChunk->bytesRead += m_FilePointer->read(&nov, 2);
	int NumberOfUV=nov;
	// Allocate memory to hold the UV coordinates
	math::vector2d coord;
//	pObject->setOrginalUVCount(nov);
// 	math::vector2d*tc=pObject->getOrginalUV();
// 	for(int i=0;i<nov;++i)
// 	{
// 		pPreviousChunk->bytesRead+=m_FilePointer->read(&coord,2*sizeof(float));
// 		tc[i]=(coord);
// 	}

}


void _3dsLoader::ReadVertices(scene::IMeshBuffer *pObject, tChunk *pPreviousChunk)
{
	// Allocate the memory for the verts and initialize the structure
	//scene::SFrame currFrame;
	// Read in the number of vertices (int)
	int nov=0;
	pPreviousChunk->bytesRead += m_FilePointer->read(&nov, 2);

	int NumberOfVertices=nov;

	video::IHardwareStreamBuffer* vStream=pObject->createStream(0,video::EMST_Position,video::ESDT_Point3f,nov,
		video::IHardwareBuffer::EUT_Static,true);
	video::IHardwareStreamBuffer* nStream=pObject->createStream(0,video::EMST_Normal,video::ESDT_Point3f,nov,
		video::IHardwareBuffer::EUT_Static,true);
	video::IHardwareStreamBuffer* uvStream=pObject->createStream(0,video::EMST_Texcoord,video::ESDT_Point2f,nov,
		video::IHardwareBuffer::EUT_Static,true);

//	currFrame.Vertices.resize(nov);
	math::vector3d vert;
	for(int i=0;i<nov;++i)
	{
		pPreviousChunk->bytesRead+=m_FilePointer->read(&vert,3*sizeof(float));

		/*math::Swap<float>(vert.Pos.y,vert.Pos.z);
		math::Swap<float>(vert.Normal.y,vert.Normal.z);*/

		//currFrame.Vertices.push_back(scene::SFrame::SKey(vert.Pos,vert.Normal));
	}
	//pObject->addFrame(currFrame);
}


void _3dsLoader::ReadMaterialGroup(scene::SMesh*pModel, scene::IMeshBuffer *pObject, tChunk *pPreviousChunk)
{
	RenderMaterialGroup group;			// This is used to hold the objects material name
	int buffer[10000] = {0};				// This is used to read past unwanted data

	pPreviousChunk->bytesRead += GetString(group.matName);

	m_FilePointer->read(&group.faceCount,sizeof(group.faceCount));
	pPreviousChunk->bytesRead+=sizeof(group.faceCount);
	group.faces=new short[group.faceCount];

	pPreviousChunk->bytesRead+=m_FilePointer->read(group.faces,sizeof(short)*group.faceCount);

	matGroup.push_back(group);
}	


void _3dsLoader::composeObject(scene::SMesh*mesh,int m)
{
	video::RenderMaterial* mat;
	scene::IMeshBuffer*mb=0;
	scene::IMeshBuffer*mb2=mesh->getBuffer(m);
	for(int i=0;i<matGroup.size();++i){
		for(int j=0;j<materials.size();j++){
			if(matGroup[i].matName==materials[j].Name){
				mat=materials[j].mat;
				mb=mesh->addNewBuffer()->getMeshBuffer();
				//mb->addFrame(scene::SFrame());
				mb->setMeshName(mb2->getMeshName());
				break;
			}
		}
		if(!mb)return;


		video::IHardwareStreamBuffer* vStream=mb->getStream(0,video::EMST_Position);
		video::IHardwareStreamBuffer* nStream=mb->getStream(0,video::EMST_Normal);
		video::IHardwareStreamBuffer* uvStream=mb->getStream(0,video::EMST_Texcoord);
		math::vector3d* verts=(math::vector3d*)vStream->lock(0,0,video::IHardwareStreamBuffer::ELO_ReadOnly);
		math::vector3d* norm=(math::vector3d*)nStream->lock(0,0,video::IHardwareStreamBuffer::ELO_ReadOnly);
		math::vector2d* tc=(math::vector2d*)uvStream->lock(0,0,video::IHardwareStreamBuffer::ELO_ReadOnly);

		int vCnt=vStream->length();
//		int cCnt=mb2->getOrginalUVCount();
		math::vector2d uv;
		for(int f=0;f<matGroup[i].faceCount;f++){

#ifdef __TODO__
			for(int v=0;v<3;v++){
				int idx=mb2->Indices[matGroup[i].faces[f]+v];
				if(idx<vCnt){
					vtx.Pos=mb2->Frames[0].Vertices[idx].pos;
				}
				if(idx<cCnt){
					uv=mb2->UVCoords[idx];
				}
				mb->UVCoords.push_back(uv);
				vpi[v]=mb->Frames[0].Vertices.size();
				mb->Frames[0].Vertices.push_back(scene::SFrame::SKey(vtx.Pos,vtx.Normal));
			}
			math::Plane p(mb->Frames[0].Vertices[vpi[0]].pos,mb->Frames[0].Vertices[vpi[1]].pos,mb->Frames[0].Vertices[vpi[2]].pos);
			mb->Frames[0].Vertices[vpi[0]].normal=p.Normal;
			mb->Frames[0].Vertices[vpi[1]].normal=p.Normal;
			mb->Frames[0].Vertices[vpi[2]].normal=p.Normal;

			mb->Indices.push_back(vpi[0]);
			mb->Indices.push_back(vpi[1]);
			mb->Indices.push_back(vpi[2]);
#endif
		}
		//mb->NumberOfTri=matGroup[i].faceCount;
		//vStream->reserve(mb->Frames[0].Vertices.size());

		vStream->unlock();
		nStream->unlock();
		uvStream->unlock();
	}


	mesh->removeBuffer(m);
	matGroup.clear();

}

const core::string g_sJPGExt=mT("3ds");

const core::string&_3dsLoader::getExtension(){
	return g_sJPGExt;
}
/*
//////////////////////////////	Math Functions  ////////////////////////////////*

void _3dsLoader::ComputeNormals(t3DModel *pModel)
{
	tVector3 vVector1, vVector2, vNormal, vPoly[3];

	// If there are no objects, we can skip this part
	if(pModel->numOfObjects <= 0)
		return;

	// What are vertex normals?  And how are they different from other normals?
	// Well, if you find the normal to a triangle, you are finding a "Face NormamT(".
	// If you give OpenGL a face normal for lighting, it will make your object look
	// really flat and not very round.  If we find the normal for each vertex, it makes
	// the smooth lighting look.  This also covers up blocky looking objects and they appear
	// to have more polygons than they do.    Basically, what you do is first
	// calculate the face normals, then you take the average of all the normals around each
	// vertex.  It's just averaging.  That way you get a better approximation for that vertex.

	// Go through each of the objects to calculate their normals
	for(int index = 0; index < pModel->numOfObjects; index++)
	{
		// Get the current object
		t3DObject *pObject = &(pModel->pObject[index]);

		// Here we allocate all the memory we need to calculate the normals
		tVector3 *pNormals		= new tVector3 [pObject->numOfFaces];
		tVector3 *pTempNormals	= new tVector3 [pObject->numOfFaces];
		pObject->pNormals		= new tVector3 [pObject->numOfVerts];

		// Go though all of the faces of this object
		for(int i=0; i < pObject->numOfFaces; i++)
		{												
			// To cut down LARGE code, we extract the 3 points of this face
			vPoly[0] = pObject->pVerts[pObject->pFaces[i].vertIndex[0]];
			vPoly[1] = pObject->pVerts[pObject->pFaces[i].vertIndex[1]];
			vPoly[2] = pObject->pVerts[pObject->pFaces[i].vertIndex[2]];

			// Now let's calculate the face normals (Get 2 vectors and find the cross product of those 2)

			vVector1 = Vector(vPoly[0], vPoly[2]);		// Get the vector of the polygon (we just need 2 sides for the normal)
			vVector2 = Vector(vPoly[2], vPoly[1]);		// Get a second vector of the polygon

			vNormal  = Cross(vVector1, vVector2);		// Return the cross product of the 2 vectors (normalize vector, but not a unit vector)
			pTempNormals[i] = vNormal;					// Save the un-normalized normal for the vertex normals
			vNormal  = Normalize(vNormal);				// Normalize the cross product to give us the polygons normal

			pNormals[i] = vNormal;						// Assign the normal to the list of normals
		}

		//////////////// Now Get The Vertex Normals /////////////////

		tVector3 vSum = {0.0, 0.0, 0.0};
		tVector3 vmath::Zero = vSum;
		int shared=0;

		for (i = 0; i < pObject->numOfVerts; i++)			// Go through all of the vertices
		{
			for (int j = 0; j < pObject->numOfFaces; j++)	// Go through all of the triangles
			{												// Check if the vertex is shared by another face
				if (pObject->pFaces[j].vertIndex[0] == i || 
					pObject->pFaces[j].vertIndex[1] == i || 
					pObject->pFaces[j].vertIndex[2] == i)
				{
					vSum = AddVector(vSum, pTempNormals[j]);// Add the un-normalized normal of the shared face
					shared++;								// Increase the number of shared triangles
				}
			}      
			
			// Get the normal by dividing the sum by the shared.  We negate the shared so it has the normals pointing out.
			pObject->pNormals[i] = DivideVectorByScaler(vSum, float(-shared));

			// Normalize the normal for the final vertex normal
			pObject->pNormals[i] = Normalize(pObject->pNormals[i]);	

			vSum = vmath::Zero;									// Reset the sum
			shared = 0;										// Reset the shared
		}
	
		// Free our memory and start over on the next object
		delete [] pTempNormals;
		delete [] pNormals;
	}
}
*/

}
}