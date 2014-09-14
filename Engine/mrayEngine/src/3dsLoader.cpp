
#include "stdafx.h"
#include "3DSLoader.h"
#include "ILogManager.h"

#include "lib3ds.h"
#include "IStream.h"
#include "MeshBufferData.h"
#include "IMeshBuffer.h"
#include "RenderMaterial.h"
#include "RenderTechnique.h"
#include "RenderPass.h"


namespace mray
{
namespace loaders
{

	static long
	fileio_seek_func(void *self, long offset, Lib3dsIoSeek origin) {
		OS::IStream *f = (OS::IStream*)self;
		OS::EStreamSeek o;
		switch (origin) {
		case LIB3DS_SEEK_SET:
			o = OS::ESeek_Set;
			break;

		case LIB3DS_SEEK_CUR:
			o = OS::ESeek_Cur;
			break;

		case LIB3DS_SEEK_END:
			o = OS::ESeek_End;
			break;
		}
		return f->seek(offset, o);
	}


	static long
		fileio_tell_func(void *self) {
			OS::IStream *f = (OS::IStream*)self;
			return f->getPos();
	}


	static size_t
		fileio_read_func(void *self, void *buffer, size_t size) {
			OS::IStream *f = (OS::IStream*)self;
			return f->read(buffer, size);
	}


	static size_t
		fileio_write_func(void *self, const void *buffer, size_t size) {
			OS::IStream *f = (OS::IStream*)self;
			return f->write(buffer, size);
	}


	static void
		fileio_log_func(void *self, Lib3dsLogLevel level, int indent, const char *m)
	{
		ELogLevel level_str[] = {
			ELL_ERROR,ELL_WARNING, ELL_SUCCESS, ELL_INFO
		};
		core::string msg;
		for (int i = 1; i < indent; ++i) 
			msg+="\t";
		msg += m;
		gLogManager.log(msg, level_str[level]);
	}



_3DSLoader::_3DSLoader()
{
}

_3DSLoader::~_3DSLoader()
{
}

scene::SMesh* _3DSLoader::load(const  core::string& name)
{
	return 0;
}

scene::SMesh* _3DSLoader::load(OS::IStream* stream)
{
	if (!stream)
		return 0;
	/*
	Lib3dsFile *f = 0;
	Lib3dsIo io;
	f = lib3ds_file_new();

	memset(&io, 0, sizeof(io));
	io.self = stream;
	io.seek_func = fileio_seek_func;
	io.tell_func = fileio_tell_func;
	io.read_func = fileio_read_func;
	io.write_func = fileio_write_func;
	io.log_func = fileio_log_func;
	int result = lib3ds_file_read(f, &io);

	stream->close();
	if (!result) {
		gLogManager.log("Failed to load 3ds mesh file:" + stream->getStreamName(), ELL_WARNING);
		lib3ds_file_free(f);
		return 0;
	}

	scene::SMesh* mesh = new scene::SMesh();

	for (int i = 0; i < f->meshes_size; ++i)
	{
		Lib3dsMesh* m= f->meshes[i];
		scene::MeshBufferData* bd= mesh->addNewBuffer();
		scene::IMeshBuffer* b = bd->getMeshBuffer();
		b->setMeshName(m->name);
		
		{
			math::matrix4x4 trans;
			trans.loadMatrix((float*)m->matrix);
			video::IHardwareStreamBuffer* posStream = b->createStream(0, video::EMST_Position, video::ESDT_Point3f, m->nvertices, video::IHardwareBuffer::EUT_Static, false);
			video::IHardwareStreamBuffer* uvStream = b->createStream(0, video::EMST_Texcoord, video::ESDT_Point2f, m->nvertices, video::IHardwareBuffer::EUT_Static, false);

			math::vector3d* pPtr = (math::vector3d*)posStream->lock(0, m->nvertices, video::IHardwareBuffer::ELO_Discard);
			math::vector2d* uvPtr = (math::vector2d*)uvStream->lock(0, m->nvertices, video::IHardwareBuffer::ELO_Discard);


			for (int v = 0; v < m->nvertices; ++v)
			{
				(*pPtr).set(m->vertices[v][0], m->vertices[v][1], m->vertices[v][2]);
				(*uvPtr).set(m->texcos[v][0], m->texcos[v][1]);

				(*pPtr) = trans*(*pPtr);

				++pPtr;
				++uvPtr;
			}
			posStream->unlock();
			uvStream->unlock();
		}
		{
			video::IHardwareIndexBuffer* index= b->createIndexBuffer(video::IHardwareIndexBuffer::EIT_32Bit, m->nfaces*3, video::IHardwareBuffer::EUT_Static, false);
			uint* idx = (uint*)index->lock(0, m->nfaces * 3, video::IHardwareBuffer::ELO_Discard);
			uint*ptr = idx;
			for (int j = 0; j < m->nfaces; ++j)
			{
				*ptr++ = m->faces[j].index[0];
				*ptr++ = m->faces[j].index[1];
				*ptr++ = m->faces[j].index[2];
			}

		}
	}*/
	return 0;
}

bool _3DSLoader::canLoad(const  core::string& name)
{
	return 0;
}

const core::string& _3DSLoader::getExtension()
{
	return core::string::Empty;
}

}
}


