

#include "stdafx.h"
#include "ARServiceProvider.h"

#include "INetwork.h"
#include "IReliableSocket.h"
#include "IThreadManager.h"
#include "IThreadFunction.h"
#include "dispObjSM.h"
#include "dispObjSMParser.h"

namespace mray
{
namespace NCam
{

	class ARProcessThread :public OS::IThreadFunction
	{
		ARServiceProvider* m_owner;
	public:
		ARProcessThread(ARServiceProvider* provider)
		{
			m_owner = provider;
		}
		virtual void execute(OS::IThread*caller, void*arg)
		{
			while (caller->isActive())
			{
				m_owner->UpdateThread();
				OS::IThreadManager::getInstance().sleep(100);
			}
		}
	};

ARServiceProvider::ARServiceProvider()
{
	OS::IThreadFunction* function = new ARProcessThread(this);
	m_commandsMutex = OS::IThreadManager::getInstance().createMutex();
	m_thread = OS::IThreadManager::getInstance().createThread(function);
	m_connection = network::INetwork::getInstance().createTCPSocket();
}

ARServiceProvider::~ARServiceProvider()
{
	Disconnect();
	delete m_connection;
	delete m_commandsMutex;
}


bool ARServiceProvider::Connect(const core::string& host, uint port)
{
	network::INetwork::getInstance().getHostAddress(host.c_str(), m_serverAddr);
	m_serverAddr.port=port;

	if (m_connection->connect(m_serverAddr) != 0)
	{
		m_thread->start(0);
		return true;
	}
	return false;
}

void ARServiceProvider::Disconnect()
{
	m_connection->closeConnection();
	m_thread->terminate();
}

bool ARServiceProvider::IsConnected()
{
	return m_connection->isConnected();
}


void ARServiceProvider::Update()
{
	std::list<IARCommand*> commands;
	m_commandsMutex->lock();
	commands = m_commands;
	m_commands.clear();
	m_commandsMutex->unlock();
	std::list<IARCommand*>::iterator it= commands.begin();
	for (; it != commands.end(); ++it)
	{
		if ((*it)->type == EARCommandType::EArData)
		{
			FIRE_LISTENR_METHOD(OnARContents, ((ARCommandAddData*)*it));
		}
		else if ((*it)->type == EARCommandType::EDeleteGroup)
		{
			FIRE_LISTENR_METHOD(OnDeletedGroup, ((ARCommandDeleteGroup*)*it));
		}
		delete *it;
	}

}

bool ParseCoordinate(uint origin, EARCoordinates& coord)
{

	switch (origin) {
	case ORIGIN_3DSPACE_3D_WORLD:
		coord= EARCoordinates::EWorldSpace;
		break;
	case ORIGIN_3DSPACE_3D_OWNCAR:
		coord= EARCoordinates::EOwnCar;
		break;
	case ORIGIN_WINDOW_3D_WORLD:
		coord = EARCoordinates::EWindow3DWorld;
		break;
	case ORIGIN_WINDOW_3D_OWNCAR:
		coord = EARCoordinates::EWindow3DOwnCar;
		break;
	case ORIGIN_WINDOW_2D_PIXEL:
		coord = EARCoordinates::EWindow2DPixel;
		break;
	case ORIGIN_WINDOW_2D_NORMALIZE:
		coord = EARCoordinates::EWindow2DNormalized;
		break;
	default:
		return false;
	}
	return true;
}

IARObject* ParseObject(DispObjHdr* pHdr)
{

	DispObjSMParser parser;
	DOSM_GLObject glObj;
	DOSM_Predefined predefined;
	DOSM_String str;
	IARObject* ret = 0;
	if (parser.asGLObject(pHdr, &glObj)) {

		ARMesh* mesh = new ARMesh();
		ret = mesh;

		ParseCoordinate(glObj.pHdr->attr.origin, mesh->coordinates);
		mesh->pos.set(glObj.pHdr->attr.posDir.pos.x,
			glObj.pHdr->attr.posDir.pos.y, glObj.pHdr->attr.posDir.pos.z);

		mesh->dir.set(glObj.pHdr->attr.posDir.heading,
			glObj.pHdr->attr.posDir.heading, glObj.pHdr->attr.posDir.heading);

		mesh->lineWidth = glObj.pGL->lineWidth;
		mesh->cullface = glObj.pGL->backCulling;

		switch (glObj.pGL->glObjType)
		{
		case DISPOBJ_GL_LINES:
			mesh->meshType = EARMeshType::ELines;
			break;
		case DISPOBJ_GL_LINE_LOOP:
			mesh->meshType = EARMeshType::ELineLoop;
			break;
		case DISPOBJ_GL_LINE_STRIP:
			mesh->meshType = EARMeshType::ELineStrip;
			break;
		case DISPOBJ_GL_TRIANGLES:
			mesh->meshType = EARMeshType::ETriangles;
			break;
		case DISPOBJ_GL_TRIANGLE_STRIP:
			mesh->meshType = EARMeshType::ETriangleStrip;
			break;
		case DISPOBJ_GL_TRIANGLE_FAN:
			mesh->meshType = EARMeshType::ETriangleFan;
			break;
		default:
			break;
		}

		switch (glObj.pGL->normalType)
		{
		case DISPOBJ_NORMAL_NONE:
			mesh->normalType = EARNormalType::ENone;
			break;
		case DISPOBJ_NORMAL_VERTEX:
			mesh->normalType = EARNormalType::EVertex;
			break;
		case DISPOBJ_NORMAL_OVERALL:
			mesh->normalType = EARNormalType::EOverall;
			break;
		case DISPOBJ_NORMAL_AUTO_SMOOTH:
			mesh->normalType = EARNormalType::EAutoSmooth;
			break;
		case DISPOBJ_NORMAL_AUTO_FLAT:
			mesh->normalType = EARNormalType::EAutoFlat;
			break;
		default:
			break;
		}

		mesh->verticies.resize(glObj.points.size());
		for (size_t i = 0; i < glObj.points.size(); i++) {
			DispObjXYZ* pXYZ = &glObj.points[i];
			mesh->verticies[i].set(pXYZ->x, pXYZ->y, pXYZ->z);
		}
		mesh->normals.resize(glObj.normals.size());
		for (size_t i = 0; i < glObj.normals.size(); i++) {
			DispObjXYZ* pXYZ = &glObj.normals[i];
			mesh->normals[i].set(pXYZ->x, pXYZ->y, pXYZ->z);
		}
		mesh->colors.resize(glObj.colors.size());
		for (size_t i = 0; i < glObj.colors.size(); i++) {
			DispObjRGBA* pXYZ = &glObj.colors[i];
			mesh->colors[i].Set(pXYZ->r, pXYZ->g, pXYZ->b, pXYZ->a);
		}
	}
	else if (parser.asPredefined(pHdr, &predefined)) {
		ARPredef* mesh = new ARPredef();
		ret = mesh;

		ParseCoordinate(predefined.pHdr->attr.origin, mesh->coordinates);
		mesh->pos.set(predefined.pHdr->attr.posDir.pos.x,
			predefined.pHdr->attr.posDir.pos.y, predefined.pHdr->attr.posDir.pos.z);

		mesh->dir.set(predefined.pHdr->attr.posDir.heading,
			predefined.pHdr->attr.posDir.heading, predefined.pHdr->attr.posDir.heading);

		mesh->name = predefined.pPredefined->name;
		mesh->scale.set(predefined.pPredefined->scaleX, predefined.pPredefined->scaleY, predefined.pPredefined->scaleZ);

	}
	else if (parser.asString(pHdr, &str)) {
		ARString* mesh = new ARString();
		ret = mesh;

		ParseCoordinate(str.pHdr->attr.origin, mesh->coordinates);
		mesh->pos.set(str.pHdr->attr.posDir.pos.x,
			str.pHdr->attr.posDir.pos.y, str.pHdr->attr.posDir.pos.z);

		mesh->dir.set(str.pHdr->attr.posDir.heading,
			str.pHdr->attr.posDir.heading, str.pHdr->attr.posDir.heading);

		mesh->fontSize = str.pString->fontSize;
		mesh->fontType = str.pFont;
		switch (str.pString->fontSizeType)
		{
		case DISPOBJ_FONTSIZETYPE_PIXEL:
			mesh->fontSizeType = EARFontSize::EPixel;
			break;;
		case DISPOBJ_FONTSIZETYPE_NORMALIZE:
			mesh->fontSizeType = EARFontSize::ENormalized;
			break;;
		default:
			break;
		}

		mesh->colors.resize(str.pString->numColor);
		for (int i = 0; i < str.pString->numColor; i++) {
			DispObjRGBA* pXYZ = &str.pString->color[i];
			mesh->colors[i].Set(pXYZ->r, pXYZ->g, pXYZ->b, pXYZ->a);
		}
		mesh->text = str.pData;
	}
	else {
	}
	return ret;
}

void ARServiceProvider::UpdateThread()
{

#define CMD_GET_AR 0
#define CMD_DISCONNECT 1

#define DATA_END	0	// end of data
#define DATA_AR		1	// AR data for 1 group
#define DATA_DELETED_GROUP		2	// deleted group ID


	if (!m_connection->isConnected())
		return;
	uint packet[] = { 4, CMD_GET_AR };
	m_connection->send(packet, sizeof(packet), true, m_serverAddr);

	bool doneFlag = false;
	bool hadData = false;
	char *data = NULL;
	while (!doneFlag) {
		network::SPacket* recPack = m_connection->popMessage();
		if (!recPack)
		{
			//problem occurred, we should receive DATA_END
			break;
		}

		data =(char*) recPack->data.pointer();
		uint *p = (uint *)data;
		uint dataType = *p++;

		IARCommand* cmd=0;

		if (dataType == DATA_END) {
			doneFlag = true;
			continue;	// i.e. breaks loop
		}
		else if (dataType == DATA_DELETED_GROUP) {
			hadData = true;

			ARCommandDeleteGroup* c = new ARCommandDeleteGroup();
			cmd = c;
			c->groupID = *p;

		}
		else if (dataType == DATA_AR) {
			hadData = true;
			ARCommandAddData* c = new ARCommandAddData();
			cmd = c;
			ARGroup* group = new ARGroup();
			c->group = group;
			group->groupID = *p++;
			DispSMHdr *pSMHdr = (DispSMHdr *)p;
			group->seqID = pSMHdr->seq;

			DispObjHdr *pObj = pSMHdr->toFirstDispObj();
			for (size_t i = 0; i < pSMHdr->numObjects;
				i++, pObj = pObj->toNext()) {
				IARObject* o = ParseObject(pObj);
				if (o)
					group->objects.push_back(o);
			}
		}
		if (cmd)
		{
			m_commandsMutex->lock();
			m_commands.push_back(cmd);
			m_commandsMutex->unlock();
		}

		delete recPack;
	}
}

}
}
